# facility/keystore SE 安全芯片封装 · 业务串讲

> **一句话定位**：设备**可信根的软件封装层**——`SeKeyStore` 把 SE 芯片 SDK（`se_api`）包成三类能力：**签名**（四族 SM2 签名，HTTP/MQTT/识别认证之源）、**密钥交换**（激活三步曲）、**SE 自定义存储**（域名备份/出厂状态）；加上一套**SE 时钟修复机制**（双钟差检测跳变 + 签名前懒同步）。私钥永不离开芯片——「设备是谁」的全部答案都封在 SE 里。
> **场次**：45-60 分钟（4 文件约 460+460 行，难点在签名体系全景与线程模型）　**听众**：同组开发 / 评审人 / 后续接手者
> **对照模版**：`06.技术模版/04.业务串讲的模版.md` 第 08 章　**代码基线**：`palm_app_linux/src/facility/keystore/`（4 文件全量精读 + 全仓调用方核对，2026-10-08 审阅；K1 竞态结论已 grep 坐实）

---

## 1. 需求与业务背景  [8min · 先现象后价值]

### 1.1 需求来源与问题
- **来源**：支付级设备的身份与签名必须有**硬件级可信根**——软件密钥可被提取/复制，只有 SE 安全芯片里的私钥不可读出。全部对外认证（HTTP 签名、MQTT 接入、刷掌识别上报）都必须由 SE 完成。
- **问题**：① SE 是**独占资源**（多 App 共享一颗芯片，DeInit 要释放给其他 App，:84-114）；② SE 芯片自带 RTC，**SE 时钟与系统时钟漂移**会让签名时间戳错（防重放失败）；③ 签名/交换的调用方分散在多个模块，需要一个统一封装防散落。
- **目标**：一套接口覆盖签名/交换/存储；SE 时间自动跟随系统时间（含 NTP 校时后的修复）；调用方对 SDK 细节零感知。

### 1.2 业务边界
- **做**：四族签名（DeviceAuth/IlinkAuth/CommonAuth/PayAuth）、密钥交换三接口、SE 自定义存储（SvrUrl/FactoryStatus）、芯片信息/解锁/巡检、SE 时钟同步、异步签名桥（completion-token）。
- **不做**：SE 芯片内部运算（COS 固件）；`kSoftKeyStore` 软实现（**枚举存在但无实现类**，6.2-K5）；密钥生命周期管理（交换后密钥全在芯片内）。

### 1.3 关键业务规则
- **四族签名与消费通道**（全仓已核对）：
  | 签名 | SDK 函数 | 消费方 | 执行线程 |
  |---|---|---|---|
  | `SignDataHash` | ComputeDeviceAuthSignature | Network HTTP DEVICE 签名（network.cpp:409） | **请求线程**（各业务） |
  | `SignPayAuth` | ComputePaySign(±anti_replay) | Network HTTP RECOGNIZE 签名（network.cpp:445，`anti_replay=true, need_base64=false`） | 请求线程 |
  | `SignCommonAuth` | ComputeAuthSign(±anti_replay) | MQTT password（device_mqtt_client.h:33，`anti_replay=true`） | **mqtt 线程** |
  | `SignIlinkAuth` | ComputeILinkAuthSignature | iLink 通道（网络库 stub 未启用） | — |
  | `ExchangeKeyWithServer` 等 | 见 3.3 | 激活 Step 3/4（activation_service.cpp:625/:746） | **LicenseThread** |
- **签名产物**：`DeviceAuthCode`（wxpayface_se.proto:19-27：se_sn/sign_version/**timestamp/counter/random**（防重放材料）/sign/**state 防拆状态**）——SE 内部自建 signSrc 再 SM2 签，**调用方只见 opaque bytes**（linkcomm.proto:9-11 注释）。
- **SE 时钟规则**：签名前 `PreSetupSign` 检查——系统时间未同步（`SysTimeSynced`）则跳过修复；已同步且（待修复 或 系统时间跳变）则 `SetSeTimestamp` 重写 SE 时钟（:442-458）。

### 1.4 改动范围（模块组成）
- **[抽象]** `keystore.h`：KeyStore 接口（SignResult{sign, sign_src, timestamp}）。
- **[实现]** `sekeystore.h/.cpp`：SeKeyStore 单例 + 专用 `"se"` 线程 + 三接口 Async 桥。
- **[协议]** `wxpayface_se.proto`：ILinkAuthCode / DeviceAuthCode / PaasSignPackage（网络库与 MQTT 签名包三件套之源）。

---

## 2. 整体设计与模块划分  [8min · 先图后讲]

### 2.1 架构与分层
```
 调用方（前几篇串讲的现场）
   Network HTTP 签名（请求线程） ──┐
   MQTT password（mqtt 线程）    ──┤
   激活密钥交换（LicenseThread） ──┼→ SeKeyStore（单例）
   License/固件检查              ──┘      │
                                    ┌────┴─────────────────────┐
                                    │ PreSetupSign（时钟修复懒执行）│
                                    │ 4 族签名 / 交换 / 存储       │
                                    │ SE_CHECK_RET_OR_RETURN 宏   │
                                    └────┬─────────────────────┘
                                     se_api（腾讯 SE SDK）
                                    ┌────┴─────────────────────┐
                                    │ 专用 "se" 线程（仅 3 个      │
                                    │ Async 接口用——零调用方，     │
                                    │ 见 6.2-K1）                 │
                                    └──────────────────────────┘
```
分层关键：**「opaque 签名」哲学**——SE 内部自建 signSrc（magic/sn/版本/时间戳/counter/random + payload）再 SM2 签，**上层只搬运 bytes**（PaasSignPackage 打包 base64），谁也不知道也不需要知道签名原文——这是防伪造的最小暴露面设计。

### 2.2 模块职责
| 组件 | 职责 |
|---|---|
| `KeyStore` 抽象 | 签名接口 + SignResult（软实现预留未兑现） |
| `SeKeyStore` 同步接口 | 4 族签名 / 交换三接口 / 存储 / 芯片管理 |
| `PreSetupSign/SyncSeTime` | SE 时钟跟随系统时间（跳变检测） |
| Async 桥（3 接口） | completion-token 跨线程执行（**零调用方**，6.2-K1） |

---

## 3. 逐模块实现讲解  [15min · 业务意图先行]

### 3.1 初始化与全局资源　:51-114/:290-303
- **两级 Init**：`GlobalInit`（SDK `SeInit` + 日志回调桥接 :294-298，静态 `global_inited_succ_`）+ 实例 Init（读 SN/COS/SDK 版本 + 运行模式，:60-81）。
- **时间处理分叉**（:71-81）：系统时间未同步 → 只置 `need_time_repair_`（**不写错时间进 SE**）；已同步 → `SyncSeTime` 失败也置 repair——懒修复闭环（见 4.2）。
- **DeInit 释放语义**（:84-114）：停 se 线程 + `SeUnInit` 归还芯片给其他 App——「SE 是独占资源」的兑现；**但 DeInit 后 thr_ 已 Stop，线程串讲 T1：此后 Async 接口静默失败、再 Init 不重建线程**（6.2-K6）。

### 3.2 四族签名（同步接口）　:116-187
- 统一模式：`PreSetupSign()` → 定长 buffer（1024）→ SDK 调用 → `resize(ret.data_len)` → base64（need_base64 开关）。
- **SignIlinkAuth 的 timestamp 解析**（:144-150）：SDK 返回 `ILinkAuthCode` pb，`std::stol(pack.timestamp())`——**timestamp 是 bytes 字段**（proto:15），非数字内容会抛 `invalid_argument` 且无 try 包裹（6.2-K10；当前 iLink 未启用，风险潜伏）。
- `SignDeviceAuth`（:128-133）只是 SignDataHash 的薄壳——`sign_src` 为空（SE 不暴露原文）。

### 3.3 密钥交换三接口（激活 Step 3/4 的 SE 侧）　:220-254
```
ExchangeKeyWithServer(index, svr_pub_key, auth, verify)
  ① ImportCustomServerPubKey（服务端公钥入 SE）
  ② ExchangeKeyAuthVerifyInfo → 返回 (dev_auth_code, dev_verify_code)
ConfirmExchangeKeyWithServer(confirm_code)
  ③ SetSeCustomFactoryStatus(confirm_code)   ← 激活确认码写入
HasExchangedKeyWithServer()
  ④ GetSeCustomFactoryStatus == 11(kSeActivated)
```
- **注意 ③④ 的复用**：`SetSeCustomFactoryStatus` 同一 SDK 函数承担**两个语义**（:244 传 confirm_code vs :360 传 factory status 字符串）——6.2-K4。
- 与激活串讲 A2（半交换风险）的 SE 侧视角：② 先改写 SE 密钥状态、③ 才由服务端 confirm——服务端失败时 SE 已半交换，**可否重入②取决于 COS 行为**（模块内无法回答，需芯片文档）。

### 3.4 SE 自定义存储与芯片管理　:307-398
- 存储族：`Get/SetSeCustomSvrUrl`（激活域名 SE 备份，激活串讲 3.2 双存储之源）、`Get/SetSeCustomFactoryStatus`（出厂状态）。
- 芯片族：SN/COS/SDK 版本、Unlock、巡检参数、`WriteSeConfig`、`DeviceAuthDecrypt`（协程解密）。
- **GetAllSePubKeys 的 0x04 修正**（:211-216）：SDK 返回 `[index(1B)][X||Y(64B)]`，软件把首字节替换为非压缩 ECC 标志 `0x04` → `04||X(32)||Y(32)` 标准未压缩 P-256 公钥——**对 SDK 返回格式的静默修正**，讲清楚可防后人误删。

### 3.5 SE 时钟修复机制　:417-458（见 4.2 深挖）

---

## 4. 核心代码实现细节与原理（重点）  [10min · 讲透原理]

### 4.1 深挖①：签名体系全景（为什么这样分四族）
- **每族对应 SE 内一条密钥 + 一套 signSrc 构造**：DeviceAuth（设备认证密钥 kSeKeyIndexDeviceAuthKey=3）、PayAuth（刷掌识别）、CommonAuth（通用）、IlinkAuth（iLink 通道）——**SE 内密钥按用途隔离**，跨用途签名互不通用（一把钥匙开一扇门）。
- `anti_replay` 开关（:157/:174）：带防重放的签名走 SE 内 counter/random/timestamp 路径——**DeviceAuthCode 的 counter 字段就是它**（proto:23）；`SignDataHash` 明确不走（network.cpp:444 注释「与 DEVICE 签名保持一致」）。
- `need_base64=false` 的场景（network.cpp:445、device_mqtt_client.h:34）：**调用方要取原始字节再自装 PaasSignPackage**——base64 是给 HTTP 头用的，protobuf 序列化前要原始字节。

### 4.2 深挖②：SE 时钟同步机制（本模块最精巧的一段）
```cpp
// CheckSysTimeChanged（:417-425）：双钟差检测
delta = (steady_now - last_steady_) - (sys_now - last_system_);
return |delta| > 1000ms;    // 双钟本应同步走，差值突变 = 系统时间被改
// PreSetupSign（:442-458）：每次签名前懒执行
if (need_time_repair_ || CheckSysTimeChanged()) {
  if (!SysTimeSynced()) return;            // 系统时间还没对——宁可 SE 时间旧，不写错值
  SetSeTimestamp(sys_ts);                  // NTP 校时后，把新时间写进 SE
  need_time_repair_ = false;
}
```
- **为什么双钟差**：`steady_clock` 单调不跳，`system_clock` 可被 NTP 校时大改——两者各自走过的时长本应相等，**差值 >1s 即系统时间发生了跳变**。与 NTP 模块 T4 的 `t4 = t1 + steady 差值` 同一哲学：用可信差值检测不可信时钟。
- **懒修复的顺序保证**：NTP 校时（系统时间变）→ 下一次任意签名 → PreSetupSign 检出跳变 → 重写 SE 时钟。**SE 时间戳是签名防重放材料的一部分，SE 时钟错 = 所有带 anti_replay 的签名时间戳错**——这是本机制存在的全部理由。
- `SysTimeSynced` 守卫（:443-447）：时间未同步时**不写**（宁旧勿错）——与 NTP「永不放弃」、激活时间守卫同一安全哲学。

### 4.3 深挖③：密钥交换的信任链（与激活串讲 4.1 呼应）
- ①服务端公钥入 SE → ②SE 产设备侧 auth/verify code（含 SE 公钥签名）→ 服务端 confirm_code → ③**confirm_code 写入 SE 才真正生效**——密钥的「生效」以 SE 内状态为准，服务端与设备端各持一半凭证，**任何单方都完成不了交换**（防中间人）。
- `HasExchangedKeyWithServer` 用**出厂状态位**表达「已交换」（status==11 kSeActivated，:253 注释）——激活串讲 Step 1 的 `HasExchangedKeyWithServer` 判出厂即源于此。

### 4.4 深挖④：线程模型——**本模块最大的问题**（引 6.2-K1）
- 现状：同步接口散落 4 类线程（请求线程/LicenseThread/mqtt 线程/…），**专用 se 线程只给 3 个 Async 接口——而 Async 接口零调用方**（grep 坐实）。
- 后果：**SE 并发调用是否安全完全取决于 se_api SDK 内部**（模块层无串行化保证）。若 SDK 非线程安全，HTTP 签名（请求线程）与 MQTT 接入（mqtt 线程）并发即竞态——**接口给了护栏，工程没用上**。

---

## 5. 边界、异常与测试  [5min · 别一笔带过]

### 5.1 异常与降级
- SDK 错误统一 `SE_CHECK_RET_OR_RETURN` 宏；**例外**：`SE_ERR_PARSE_RESPONSE_APDU_FAILED (268447737)` 被宏**吞掉**（LOG_W 后按 status/data 继续，:23-38）——TODO 未确认含义（6.2-K2）。
- 系统时间未同步：PreSetupSign 跳过（宁旧勿错）；SE 签名照发（旧时间戳）。
- DeInit 后：SE 资源归还其他 App；本模块再 Init 可恢复，**但 se 线程已死**（6.2-K6）。

### 5.2 边界条件
| 场景 | 行为 |
|---|---|
| 系统时间跳变（NTP 校时） | 下次签名前重写 SE 时钟（4.2） |
| DeInit 后调 Async 接口 | 静默失败（线程已 Stop，6.2-K6） |
| ILink timestamp 非数字 | `std::stol` 抛异常无包裹（6.2-K10） |
| SDK 返回 APDU 解析错 | 吞错继续（6.2-K2） |
| 防拆 state≠0 | 设备端不校验照发（服务端权威校验，6.2-K3） |

### 5.3 测试覆盖
- **零单测**。签名产物是 opaque bytes（依赖真芯片），但 **PreSetupSign/CheckSysTimeChanged 的双钟差逻辑、0x04 修正、base64 编解码、proto 打包全是可单测的纯逻辑**——都没写。双钟差检测（>1s 判跳变）尤其值得单测：构造模拟时钟序列即可，无需芯片。

---

## 6. 踩坑与接手注意事项（含审阅发现）  [审阅 + 雷区]

### 6.1 开发中坑
- **改签名族别**：四族对应 SE 内不同密钥——「统一签名」会破坏服务端验签预期（与激活串讲 A4 签名三级同理）。
- **SE 时间**：调 NTP/改系统时间的同学要知道——**SE 时钟在下一次签名时才修复**，中间窗口内 SE 签名时间戳是旧值。

### 6.2 接手者雷区地图（⚠️ 本次审阅发现，K1 已 grep 坐实）
| # | 现象 | 代码位置 | 风险 / 建议 |
|---|---|---|---|
| **K1** 🔴 | **SE 并发调用无线程保证**：同步接口散落 4 类线程（HTTP 签名=请求线程 network.cpp:409/:445、MQTT=mqtt 线程 device_mqtt_client.h:33、交换=LicenseThread activation:625）；**专用 se 线程只服务 3 个 Async 接口且零调用方**（grep 坐实）——SE 串行化完全寄望 se_api SDK 内部，模块层无护栏 | sekeystore.cpp:40/:268-288 vs 全部同步调用 | **先核实 SDK 线程安全声明**；若非线程安全，最低成本修法：同步接口内部 `post(thr_)` 化（一行变串行）；或文档声明「调用方须自持 LicenseThread」 |
| **K2** | `SE_ERR_PARSE_RESPONSE_APDU_FAILED` 被宏吞掉（TODO 未确认含义，:23-24 注释）——静默降级按 status/data 继续 | :25-38 | 向芯片团队确认 0x10002FF9；确认前至少升级为 LOG_E + 计数告警 |
| **K3** | `DeviceAuthCode.state`（防拆状态，proto:26）设备端不校验——SE 报告被拆时签名照发，依赖服务端拦截 | network.cpp:409 打包处 | 服务端是权威校验点；设备端可加一次性告警（state≠0 LOG_E） |
| **K4** | `SetSeCustomFactoryStatus` 双语义复用：:244 传 confirm_code（激活确认）、:360 传 factory status 字符串——同一 SDK 函数两个入口 | :242-247 vs :359-363 | 语义拆分（两个包装函数各自注释）；确认 SDK 对两种入参的区分依据 |
| **K5** | `kSoftKeyStore` 枚举存在但无 SoftKeyStore 实现类——抽象层只有一个实现 | keystore.h:20 | 软实现（测试用 mock）若要做，正好补 K1 的单测缺口 |
| **K6** | DeInit 停 se 线程后不可恢复：thr_ Stop 后（线程串讲 T1）Async 静默失败；再 Init 不重建线程 | :84-96 | DeInit 里 delete+置空 thr_，Init 里重建；或 Async 入口检查线程状态 |
| **K7** | `SignIlinkAuth` 的 `std::stol(pack.timestamp())` 无 try——bytes 字段非数字即抛 | :149 | iLink 未启用故潜伏；启用前必修（try 或 from_chars） |
| **K8** | buffer 定长（1024/256/80/65）+ `resize(ret.data_len)`——SDK 超长截断行为未文档化 | 多处 | 与 SDK 确认「data_len > 缓冲」时的返回约定 |
| **K9** | PreSetupSign 只在签名入口调用；**交换/公钥读取接口不带时间修复**——交换协议内若 SE 用时间戳，漂移期交换存在隐患 | :220-254 无 PreSetup | 与芯片文档确认交换协议是否依赖时间戳 |

**给接手者的硬提醒**：
- 🔴 **K1 是本模块头号工程风险**——不是 bug 而是缺失的护栏；核实 SDK 线程安全优先于一切改动。
- 🟡 改 `GetAllSePubKeys` 时记得 `0x04` 修正（:214）——删掉它服务端验签全挂且无报错可查。
- 🟡 双钟差阈值 1s（:424）是经验值——改小会频繁重写 SE 时间（APDU 开销），改大漏检短跳变。

---

## 7. 参考文档
- 模块路径：`palm_app_linux/src/facility/keystore/`（keystore.h / sekeystore.h/.cpp / wxpayface_se.proto）
- 依赖：`se_api`（腾讯 SE SDK：advance/base/crypto 三层）、facility/thread（专用 se 线程）、Device::SysTimeSynced
- 消费方现场（前几篇串讲）：网络库 4.4（HTTP 三级签名）、Mqtt 4.5（CommonAuth→password）、激活 3.3/4.1（交换三接口 + A2 半交换）、NTP（PreSetupSign 跳变检测的触发源）
- 调用方代码：network.cpp:409/:445、device_mqtt_client.h:33、activation_service.cpp:625/:746
- 相关串讲：《NTP功能》（双钟差哲学的同源）、《线程功能》（T1/T2——K6 的根）、《网络库》《Mqtt功能》《激活功能》（三大消费通道）
