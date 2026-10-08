# facility/ntp 时间同步功能 · 业务串讲

> **一句话定位**：自研 SNTP 客户端（RFC 5905 简化模式，48 字节手写报文 + UDP 123）+ 三路同步编排（开机 5s 快试→60s 慢试不放弃 / 24h 周期 / 网络恢复触发）——**全系统「时间可信」的供给源头**：签名、TLS 证书校验、指令过期、调度器时间门控全部依赖本模块把时钟拨对。
> **场次**：30-40 分钟（2 文件约 470 行，难点在四时标 offset 公式与「不放弃」编排）　**听众**：同组开发 / 评审人 / 后续接手者
> **对照模版**：`06.技术模版/04.业务串讲的模版.md` 第 08 章　**代码基线**：`palm_app_linux/src/facility/ntp/`（2 文件全量精读，2026-10-08 审阅；NTP 服务器规则与调用方已在前几篇核对）

---

## 1. 需求与业务背景  [8min · 先现象后价值]

### 1.1 需求来源与问题
- **来源**：设备没有 RTC 电池保证（或出厂时间随机），断电重启后时间不可信（1970）——而**时间不可信时全系统安全机制失效**：HTTP 签名时间戳错、TLS 证书 not-yet-valid（网络库 20105 的根因之一）、指令过期无法校验（指令串讲 3.2 时间守卫）、License 到期无法判断。
- **问题**：① 商用 NTP 服务器列表在私有化部署不可用——NTP 服务器必须由**后台域名**派生；② 开机时网络未必就绪，同步可能持续失败；③ 时间写入要落到 RTC 硬件，否则重启又错。
- **目标**：域名派生 NTP 服务器；开机快试 + 失败退避**永不放弃**；成功写系统时间 + 同步 RTC；boot 完成后转入 24h 周期；运行时再同步成功要通知消费方（License 重新校验）。

### 1.2 业务边界
- **做**：SNTP 请求/校验/offset 计算（四时标）、settimeofday + hwclock 写 RTC、boot/periodic/网络恢复三路触发、双回调机制（boot done one-shot / sync done persistent）。
- **不做**：多服务器池/交叉校验（单服务器 = `link-{host}`，与 MQTT broker 同域名族，:99-101）；时区设置（只管 UTC，激活串讲 A1 时区 bug 的「NTP 只同步时间不做时区」正是此意）；亚秒级精度保留（写入截断到秒，见 6.2-N5）；NTP 认证（无 symmetric key）。

### 1.3 关键业务规则
- **服务器规则**：`link-{GetDeviceDomainHost()}`（:99-107）——与 MQTT broker（Mqtt 串讲 2.3）、网络检测共用同一后台域名族；host 未配置则「server not configured」直接失败。
- **报文协议**（:26-47）：48 字节 `NtpPacket`（static_assert :44）；请求 LI=0/VN=4/Mode=3 → `0x23`（:183-185）；NTP epoch 1900 与 Unix 1970 差 `2208988800`（:47）。
- **响应校验**（:217-238）：mode ∈ {4,5}（server/symmetric-passive）；stratum 1-15；T2/T3 非零。**不校验**：来源地址（recvfrom 传 nullptr :205）、orig_ts 回显、LI 告警位——见 6.2-N1。
- **重试**：每轮 3 次、单次 SO_RCVTIMEO 3s、总超时 15s（h:88-90）；DoSync 全败返回**最后一次**结果（:153）。

### 1.4 调用方全景（前几篇已核对）
| 触发路径 | 位置 | 语义 |
|---|---|---|
| 开机 boot sync | StartBootSync（:60-88） | 5s 快试，3 败转 60s，**不放弃** |
| 24h 周期 | PeriodicSyncTask（:293-307） | 首次成功才注册；失败按连接态 10s/60s 重试 |
| 网络恢复 | OnNetworkConnected（:90-97） | 重置计数 + 恢复 5s 快试 |
| 激活前预同步 | activation_service.cpp:184-189 | 失败**不阻塞**激活（域名校验的证书有效期受影响，见激活 A1） |
| 平台指令 syncNtp | iot ntp_sync_cmd（指令串讲 3.4） | 人工触发一次 |
| 消费方回调 | LicenseManager（Story 136057693） | `RegisterSyncDoneCallback` → 运行时同步后 `RefreshAuthState(kNtpSync)` |

---

## 2. 整体设计与模块划分  [5min · 先图后讲]

### 2.1 架构与分层
```
 三路触发 ──┐
  boot(调度器) ─┤         ┌────────────────────────────┐
  24h 周期   ─┼→ SyncTime → DoSync(3 次/15s)           │
  指令/激活  ─┘    │     │ SendNtpRequest（UDP 123）   │
                  │     │  T1→sendto→recvfrom→T4      │
                  │     │  校验 mode/stratum/T2T3     │
                  │     │  offset=((T2-T1)+(T3-T4))/2 │
                  │     ▼ SetSystemRtcTime             │
                  │     │  settimeofday（CAP_SYS_TIME）│
                  │     │  system("hwclock --systohc") │
                  └──── 结果 → TimeStateManager 时间可信
                        → boot_sync_done_ / 双回调通知
```
分层关键：**「同步」与「触发」分离**——DoSync 是纯同步原语（可被任何路径调用），三路触发只是编排差异；回调机制（one-shot boot / persistent sync）把「时间可信了」这个事实广播给生态（LicenseManager 是已知消费方）。

### 2.2 核心数据流（一次开机同步）
```
boot → StartBootSync：调度器注册 ntp_boot_sync（5s，首次 3×5=15s 延迟）
 → tick 触发 → SyncTime → DoSync(link-{host})
    SendNtpRequest：T1 记录 → UDP 发 48B → 3s 收 → T4
    校验 → offset → corrected = T4 + offset - 2208988800
 → SetSystemRtcTime：settimeofday + hwclock --systohc
 → 成功：boot_sync_done_=true → 任务自禁用
        → NotifyBootSyncDoneCallbacks（one-shot，swap）
        → NotifySyncDoneCallbacks（persistent，copy）
        → RegisterPeriodicSyncTask（首次成功才注册 24h）
 → 失败×3：SetTaskMultiplier(60s) 慢试，永不放弃（C3 fix）
```

---

## 3. 逐模块实现讲解  [15min · 业务意图先行]

### 3.1 `SendNtpRequest`：四时标同步原语　:156-255
- **流程**：getaddrinfo（AF_INET/UDP）→ socket + SO_RCVTIMEO 3s → 记 T1（`GetNtpTimestamp()`，:188）→ sendto → recvfrom → 记 T4（steady_clock 差值，:206）→ 校验 → offset。
- **T4 的取法是亮点**（:242-243）：`t4 = t1 + (t4_steady - t1_steady)`——用 steady_clock 差值叠加到 T1，**避免第二次读系统钟**（settimeofday/跳变期间读两次系统钟会引入伪误差）。与网络库 trace 的 `substr` 手法同一哲学：避开不可信来源，用可信差值。
- **frac 精度**（:230-234）：小数部分 `/4294967296.0`（2^32）——亚秒精度完整参与 offset 计算；但写入时截断到秒（6.2-N5）。

### 3.2 `DoSync`：重试与总超时　:126-154
- 3 次/轮、15s 总超时（每轮前检查累计 elapsed :132-136）；成功即写 RTC + 注册周期 + 返回；全败返回最后一次错误（3 次错误可能不同，日志只见最后一条，6.2-N4）。

### 3.3 `SetSystemRtcTime`：两级写入　:257-274
- `settimeofday`（需 CAP_SYS_TIME，失败记 errno）→ `system("hwclock --systohc --utc")` 同步 RTC 硬件——**hwclock 失败仅 LOG_W（软失败）**：系统时间已对但 RTC 未写，重启后时间又错、下次 boot 再同步（可接受的降级链，但要知道存在，6.2-N3）。

### 3.4 三路编排（调度器消费现场）
- **boot**（:60-88）：kLow + kAsync（网络阻塞落调度器池）；3 败切 60s（**C3 fix 注释 :76-78 是本模块最重要的一段设计说明**：OnNetworkConnected 不可靠——「网络 connected 但 NTP unreachable」不会触发它；设备必须停在 kLicenseChecking 直到 NTP 成功——**宁可慢不可放弃**，因为时间可信是全系统安全前提）。
- **周期 24h**（:276-307）：`RegisterPeriodicSyncTask` 幂等（`periodic_task_registered_` 旗标）+ 注册即禁用（:283）——**首次成功才启用**（:145-146 调 EnablePeriodicSync），避免周期任务在时间不可信期空转；失败按 `NetworkInfo::IsConnected` 选 10s/60s 重试，成功恢复 24h——**退避旋钮全用 SetTaskMultiplier**（调度器串讲 4.4 的失败退梯模式）。
- **网络恢复**（:90-97）：重置计数 + 恢复 5s 快试（SetTaskEnabled true）。

### 3.5 双回调机制　:309-351
- **boot done**：one-shot——通知时 `swap` 清空（:327），回调只执行一次；注册时若已 done 立即执行（:312-315）。
- **sync done**：persistent——通知时 `copy` 不清空（:346 注释），每次同步成功（boot + 24h）都触发——LicenseManager 靠它做运行时重校验。
- 两族回调查锁后拷贝再触发（与 paas_shadow 信号通知同款手法）。

---

## 4. 核心代码实现细节与原理（重点）  [8min · 讲透原理]

### 4.1 深挖①：四时标 offset 公式
```cpp
// :245-248
double offset = ((t2 - t1) + (t3 - t4)) / 2.0;   // t1/t4 客户端钟，t2/t3 服务端钟
double corrected_ntp = t4 + offset;
int64_t corrected_unix = corrected_ntp - kNtpUnixEpochDiff;
```
- **为什么这个公式**：上下行延迟不对称时，`(T2-T1)` 含上行延迟 + 钟差，`(T3-T4)` 含负的下行延迟 + 负钟差——**两者相加平均，上下行延迟相互抵消，剩纯钟差**。这是 NTP 的标准算法（RFC 5905），精度受限于上下行延迟不对称度（路径抖动）。
- **corrected = T4 + offset 而非 T3**：T4 是客户端钟读数——在客户端钟上应用服务端视角的 offset，结果在客户端钟域内自洽。

### 4.2 深挖②：「永不放弃」的编排语义（C3 fix）
- 三态：5s 快试（0-2 败）→ 60s 慢试（3+ 败）→ 一直慢试到成功；OnNetworkConnected 尝试拉回快试但**不依赖它**。
- **为什么不能放弃**：本模块失败的下游不是「时间不准」而是**全系统安全降级**——LicenseManager 停在 kLicenseChecking（设备不可用）、指令时间守卫拒绝一切（指令串讲 5.1）、TLS/签名失败。放弃重试 = 设备永久不可用；60s 慢试的成本可忽略。**「关键基础设施的重试策略：退避到可忽略的频率，但永不停止」**。

### 4.3 深挖③：时间可信生态位
- 本模块是**供给侧**：settimeofday 之后 `TimeStateManager` 观测到时间可信（Story 136057693 的另一半）。
- 消费侧全景（前几篇串讲已逐一见过）：调度器 `requires_time_synced` 门控、MQTT RunLoop 时间门、激活 `IsTimeTrusted` 守卫与域名校验、指令过期校验、SpeedTester Schedule 时间门——**全部阻塞在本模块成功之前**。讲透这一点，听众才理解为什么这个 470 行的小模块是关键路径。

---

## 5. 边界、异常与测试  [5min · 别一笔带过]

### 5.1 异常与降级
- host 未配置：`ERR_PALM_NTP_SERVER_NOT_CONFIGURED`（未激活设备必然失败——依赖激活注入域名）。
- settimeofday 失败（无 CAP_SYS_TIME）：记 errno 返回 false → `ERR_PALM_NTP_SET_RTC_FAILED`。
- hwclock 失败：软失败（仅警告）——系统时间对、RTC 未写、重启又错再同步。
- 响应超时/尺寸错/mode/stratum/T2T3 零：各自独立错误码（`ERR_PALM_NTP_TIMEOUT/RESPONSE_INVALID`）。

### 5.2 边界条件
| 场景 | 行为 |
|---|---|
| 3 次错误各不相同 | 只返回/记录最后一次（:153，6.2-N4） |
| 响应来自非请求目标主机 | **不校验来源**，照收（6.2-N1） |
| offset 异常大（被劫持） | **无阈值**，照写 RTC（6.2-N2） |
| 亚秒精度 | 参与计算、写入截断（6.2-N5） |
| settimeofday 后时间回拨大跳 | TimeStateManager 信任判定基于可观测范围——大幅跳变由消费方各自守卫兜底 |

### 5.3 测试覆盖
- **零单测**。offset 公式、报文构建、校验分支全是纯逻辑，**最适合单测却没写**——与激活 A1（时区 bug）、调度器分频公式同病：小模块纯函数无护栏，改动靠人肉。补测优先级：offset 公式（构造 T1-T4 四元组断言 corrected）> 校验分支（构造畸形响应字节流）。

---

## 6. 踩坑与接手注意事项（含审阅发现）  [审阅 + 雷区]

### 6.1 开发中坑
- **改 NtpPacket 结构**：48 字节 static_assert 是唯一护栏——字段顺序/对齐改动立即暴露，**别删这个断言**（:44）。
- **`network_sync_retries_` 的三处写入**（任务里 ++ :74、恢复时置 0 :93）分别跑在调度器池线程与调用线程——非 atomic，小竞态面（6.2-N9）。

### 6.2 接手者雷区地图（⚠️ 本次审阅发现）
| # | 现象 | 代码位置 | 风险 / 建议 |
|---|---|---|---|
| **N1** 🔴 | **响应未验来源**：`recvfrom(..., nullptr, nullptr)`（:205）不校验来源地址=请求目标；且不校验 orig_ts 回显（防响应串扰）与 **LI 告警位**（leap=3 = 服务端自认时钟不可信，我们照收） | :205/:218-238 | UDP 123 无认证本身是 NTP 局限，但**来源校验是一行改动**（recvfrom 填 addr 与 ai_addr 比对）；LI≠0 应拒收。私有化内网风险低，公网部署前必修 |
| **N2** 🔴 | **offset 无 sanity 阈值**：劫持/错误 NTP 一次就把系统时间打偏（TLS/签名/指令过期连锁全错）——本模块是全系统时间可信的唯一来源，**单点无防护** | :246-253 | 建议：\|offset\| 超阈值（如 10 年）拒收并告警；或与 RTC/上次可信时间比对。与 N1 组合是「恶意 NTP 控制设备时间」的完整攻击面 |
| **N3** | `system("hwclock --systohc --utc")`：固定命令无注入面，但 hwclock 失败**软失败**——RTC 未写则重启后时间又错，每次开机重复同步（可接受但要知晓）；`system()` 调用方式与指令模块 RCE 教训同族，建议换直接 API | :268-271 | 软失败加一次性 LOG_E（当前 LOG_W 易被过滤）；长期换 `ioctl(RTC_SET_TIME)` |
| **N4** | DoSync 全败只返回最后一次错误（3 次错误可能不同） | :149-153 | 3 次都记日志（当前有 LOG_W :149，够用）；result 聚合语义注释化 |
| **N5** | 亚秒精度丢弃：frac 参与计算，写入 `tv_usec=0` + 秒级截断（:248 静态截断非四舍五入） | :259/:248 | 对签名/TLS 秒级足够；若未来做亚秒敏感业务（毫秒级排序）需保留 usec——`corrected` 已有小数，写入时 `tv_usec = frac×1e6` 即可 |
| **N6** | 单服务器设计（`link-{host}`）：无多源冗余——后台域名解析故障 = 永远无法对时（退避 60s 死循环，设备停 kLicenseChecking） | :99-107 | 私有化可接受；公网场景评估兜底源（如运营商 NTP） |
| **N7** | mode 校验收 4 或 5（:219）——5 是 peer 模式，RFC 建议 client 收 4；宽松可接受但非标准 | :219 | 收紧为 mode==4 或注释声明宽松理由 |
| **N8** | `periodic_task_registered_` 无锁 bool + `network_sync_retries_` 非 atomic | h:104/:105 | 竞态窗口小（首次注册单线程期 + 任务/恢复两线程）；补 atomic 成本一行 |
| **N9** | 首次延迟用 `kBootSyncInitialDelayMultiplier=3` 表达（3×5=15s）——复用 multiplier 语义而非 offset 参数，可读性差 | h:95 | 注释换算关系，或调度器加显式 delay 参数 |

**给接手者的硬提醒**：
- 🔴 **N1+N2 组合是本模块最大的安全敞口**（恶意抢答 UDP 即可控制设备时间 → TLS/签名/过期全错）——公网部署前必须修；私有化内网风险评级降低但建议一并修（来源校验一行、LI 检查两行、阈值一段）。
- 🟡 T4 的 steady_clock 推算法（:242-243）是本模块最精巧的一处——**改动同步逻辑前理解它为什么不用第二次系统钟读数**，别「简化」掉。
- 🟡 时间写入顺序：先 settimeofday 后 hwclock——hwclock 从系统钟抄到 RTC，顺序反了 RTC 写错值。

---

## 7. 参考文档
- 模块路径：`palm_app_linux/src/facility/ntp/`（ntp_time_sync.h/.cpp）
- 消费方生态（前几篇串讲）：调度器 `requires_time_synced`（调度器 3.3）、MQTT 时间门（Mqtt 5.1）、激活时间守卫与域名校验（激活 3.2/6.2-A1）、指令过期校验（指令 3.2）、LicenseManager（Story 136057693 停 kLicenseChecking + RefreshAuthState）
- 调用方代码：activation_service.cpp:184-189（预同步）、iot ntp_sync_cmd（指令触发）、time_state_manager（时间可信判定）
- 相关串讲：《调度器功能》（三路编排的底座）、《网络库》（`link-{host}` 域名族与 TLS 时间根因）、《激活功能》（A1 时区 bug——NTP 只同步不设时区是其一）
