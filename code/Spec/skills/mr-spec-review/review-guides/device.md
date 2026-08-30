# 设备端（固件 · 边缘）评审依据（Device / Edge Review Guide）

> 适用：跑在业务设备上的固件/边缘服务。
> - `<IoT 服务仓库>`：C++ / CMake，设备侧 Linux IoT 服务。
> - `paymax_device`：`<终端应用仓库>`（C++ 设备应用）、`<设备接入仓库>`（C++ POS 对接）、`<管理后台仓库>`（Android 设备管家）。
> - `<IoT 服务仓库>`：Android(Java/Kotlin) 设备侧 IoT 框架/SDK（IoTSdk、module-ota、module-device、module-cmd、module-activate、module-recovery…）。
> 加载时机：MR 命中上述目录，或涉及 `devicegateway`/`wecarddevicegateway`/`<设备网关>` 接口时，Step 4.1 / 4.3 / 4.5 / 4.6 前读完本文件。

---

## 1. 评审基准（设备端的特殊性）

设备端与后台/前端最大不同：**代码跑在真实硬件上，升级靠 OTA、有物理副作用、回滚代价极高、无法随时热修**。因此评审的第一性原理是：

1. **兼容性 > 功能** —— 老设备可能长期不升级，任何协议/字段/接口变更都要保证老固件不崩。
2. **稳定性 > 优雅** —— 崩溃/内存泄漏/死锁会让设备变砖，比后台重启严重得多。
3. **可回滚 / 可恢复** —— OTA 失败要能 recovery（`module-recovery`），不能出现"升级即变砖"。

---

## 2. 评审 Checklist（设备端专业维度）

### 2.1 设备接口兼容性（🔴 硬性，对应 SKILL.md 4.3）
- [ ] **新增字段必须有默认值**，老设备不传时行为不变。
- [ ] **不改已有字段语义、不改类型、不改字段号**（proto/协议），对照 [`contract.md`](./contract.md)。
- [ ] **错误码只新增不复用/不改义**。
- [ ] 接口下线/改名前有兼容期，**至少覆盖一个 OTA 周期**（老设备升级需要时间窗）。
- [ ] 破坏性变更 → 🔴，要求作者给灰度/分批 OTA 方案。

### 2.2 OTA 升级（`module-ota` / `upgradeapp`）
- [ ] 升级包完整性校验（签名/hash）、版本比对、断点续传/失败重试。
- [ ] **升级失败可回滚**（`module-recovery` / A-B 分区 / 恢复出厂前的保护），不留砖机路径。
- [ ] 灰度策略：按设备批次/机型灰度，不一次全量推。
- [ ] 升级过程对业务的影响（升级中能否业务？重启时机是否避开高峰？）。

### 2.3 C++ 稳定性与资源（`<IoT 服务仓库>` / `<终端应用仓库>` / `<设备接入仓库>`）
- [ ] **内存**：new/delete、智能指针配对，无泄漏/野指针/double free；缓冲区边界检查（防溢出）。
- [ ] **并发**：多线程共享数据有锁保护、无死锁；条件变量/信号量用法正确。
- [ ] **异常/错误**：系统调用、IO、网络返回值都检查；不因单点异常整进程崩溃。
- [ ] **资源**：文件句柄/socket/定时器有释放；长跑服务无句柄泄漏。
- [ ] 遵循 `CPPLINT.cfg`；无注释死代码、无 TODO 遗留在关键路径。

### 2.4 Android 设备框架（`<IoT 服务仓库>` / `<管理后台仓库>`）
- [ ] AIDL/进程间通信（`*.aidl`）兼容：接口方法只加不改签名；跨进程数据序列化兼容。
- [ ] 生命周期/线程：主线程不做耗时 IO；Service/广播注销防泄漏。
- [ ] 指令下发（`module-cmd`）幂等：重复指令不产生双份物理副作用（开门/重启）。
- [ ] 权限/激活（`module-activate`）：设备密钥/证书安全存储（不明文落盘），激活流程防重放。

### 2.5 设备鉴权与安全
- [ ] 设备侧密钥（SE 芯片/证书/SM 密钥）安全存储与使用；不硬编码、不进日志。
- [ ] 与云侧鉴权对齐：SN 签名 + 过期校验 + 限频（devicegateway 侧）；防重放（时间戳/nonce）。
- [ ] 上报数据脱敏；生物特征/掌图端侧处理与留存合规。

### 2.6 网络与弱网
- [ ] MQTT/长连接：断线重连、心跳、离线缓存与补传。
- [ ] 弱网/断网降级：核心业务能力（如离线本地识别）不完全依赖云端在线。
- [ ] 超时/重试不放大写副作用；服务端限流时端侧退避。

---

## 3. 设备端高频坑

| 坑 | 症状 | 后果 | 对策 |
|----|------|------|------|
| 破坏性协议变更 | 改字段号/语义/删字段 | 老设备解析崩溃 | 只加不改 + 默认值 + 兼容期（4.1） |
| 升级不可回滚 | OTA 失败无 recovery | 设备变砖 | A-B 分区/恢复保护 + 灰度 |
| C++ 内存问题 | 泄漏/越界/野指针 | 长跑崩溃、可被攻击 | 智能指针 + 边界检查 + 静态扫描 |
| 指令不幂等 | 重复指令重复开门 | 物理安全事故 | RequestId + 单指令锁 |
| 密钥明文 | 证书/密钥落盘或进日志 | 密钥泄露 | 安全存储 + 脱敏 |
| 强依赖在线 | 断网就不能业务 | 可用性差 | 离线识别兜底 |

---

## 4. 本项目真实代码约定与专项检查（带代码依据）

> 以下是从设备端真实代码（`<IoT 服务仓库>` C++、`<IoT 服务仓库>` Android）提炼的专业检查项，**含已在代码里发现的高危写法**。评审时逐条对照；括号内为可 grep 的真实文件/符号。

### 4.1 通信链路：iLink 长连 + HTTPS 短链（认清走哪条）
- [ ] C++ 侧 `network/network.h` 的 `DualLinkTdiRequest`（iLink+HTTPS 双链路）**当前是未实现的空壳**，实际 `static use_shortlink_=true`（`network.cpp`）走 **HTTPS 短链**；Android 侧长连接走 `lib-ilink/…/IoTIlinkApiManager.sendCommonRequest`。评审跨端消息/新指令时先确认它实际走哪条链路，别假设长连可用。
- [ ] 🟠 `IoTApiManager` 有 "hardcode ip" 拉取 iplist 的逻辑——涉及此处改动要盯**硬编码 IP + 是否绕过证书校验**（`<IoT 服务仓库>/lib-ilink`）。
- [ ] 心跳 `service/heart_service.cpp DoHeartAction()`：周期由服务端下发 `heartbeat_interval_sec` 动态调整、带 `RandomOffset()` 抖动防惊群。⚠️ 注意 `need_config_sync` 与 `server_time` 校准分支仍是 **TODO 未落地**——若 spec 依赖这两个能力要先确认是否已实现。

### 4.2 OTA 升级（🔴 本端最高风险，已发现真实缺陷）
C++ 侧 `service/upgrade/upgrade_schd.cpp` + `service/download_service.cpp`：
- [ ] 🔴 **包完整性校验可被跳过 + 仅 MD5**：`download_service.cpp` 的 `CheckMac()` 首句 `if (mac.empty()) return true;`——**服务端不下发 mac 就直接放行**；且校验用 `CalMd5`（MD5 可碰撞），**无数字签名验签**。任何 OTA/下发包相关方案，评审要求：mac 强制非空 + 引入签名验签（对照本文件 §2.2）。
- [ ] 🔴 **无 A/B 分区自动回滚**：C++ 侧靠 `CheckPendingUpgradeSession()` + session 落盘做"断电续做"，装坏无自动回退。新 OTA 方案**必须明确失败恢复路径**（回滚/recovery/最坏可远程重推）。
- [ ] 版本比对 `CompareVersion()`（点分整数逐段，`>=0` 跳过）；升级模式 `UpgradeMode{ADVICE/FORCE/ATONCE}`；整体超时 `kOverallUpgradeTimeout=1h` + `busy_/installing_` 原子锁防并发——评审改升级流程别破坏这几处保护。
- [ ] 后置动作 `UpgradePostAction{kRestartApp/kRestartDevice/kRestartDeviceImmediately}`；⚠️ `DoInvokeUpgrade` 的 `post_action` 标注 "还没用上"，实际走 `ExecuteUpgradePostLogic` 的 `max_finish_action` 另一套——改重启逻辑要保证两处一致。
- [ ] Android 侧 `module-ota`（`OTAManager.kt` + Flow 架构）与 C++ 侧是**两套实现**，评审时别张冠李戴；`module-recovery` 是恢复兜底，涉及升级失败要联动看。

### 4.3 指令下发（command 模式，`<IoT 服务仓库>/src/cmds/`）
指令是 command 模式：`base_cmd.h` + 每指令一个类。
- [ ] 🔴 **高危不可撤销指令**已有：`reset_factory.h`（恢复出厂）、`reboot_device.h`（重启）、`restart_app.h`——评审这类指令：是否**独立权限/二次确认**（对照 backend code-patterns#10）？是否**幂等**（重复下发不重复执行物理副作用）、有无 `RequestId`/去重？
- [ ] 新增指令继承 `base_cmd`、按现有模式注册；执行结果要有回执与失败上报，不静默吞错。
- [ ] MQTT/长连 + 轮询双通道下发时，端侧必须去重（防同一指令两条通道各执行一次）。

### 4.4 设备密钥与安全
- [ ] 🔴 设备密钥走 **SE 芯片 keystore**（`<IoT 服务仓库>/src/keystore/sekeystore.h`）——密钥/证书**不落普通文件、不进日志**。评审密钥相关改动确认经 `sekeystore`，别引入明文存储；`certs/` 证书、`lib-tid` 设备身份同理。
- [ ] 设备鉴权：SN 签名 + 过期校验 + nonce 防重放（与后台 devicegateway 对齐）；`module-activate` 激活流程防重放。

### 4.5 C++ 工程规范（`<IoT 服务仓库>`）
- [ ] 保持现有惯例：广泛使用 `shared_ptr/unique_ptr` + `std::mutex/lock_guard`（见 `thread/`、`scheduler/base_frequency_scheduler`）——新增 C++ 代码**别裸 `new/delete`、别无锁访问共享态**。
- [ ] 长跑逻辑注意崩溃恢复（`system/crash_manager`）；跨进程走 `dbus/`（`iot_dbus_server`）要保证接口兼容。
- [ ] 遵循 `CPPLINT.cfg`；系统调用/IO/网络返回值必检，单点异常不拖垮整进程。

### 4.6 Android 设备框架（`<IoT 服务仓库>`）
- [ ] AIDL 跨进程接口（`*.aidl`，如 `module-ota`/`IoTSdk`）**只加方法不改已有签名**；跨进程数据序列化保持兼容。
- [ ] 设备侧模块（`module-device`/`module-shadow`/`module-config`）改属性上报/影子逻辑，期望态 vs 上报态收敛要与后台一致。
- [ ] OkHttp 超时当前硬编码 10s（`IoTApiManager`）——评审新网络调用别各处再散落硬编码超时。

---

## 5. 跨端联调契约（设备端视角）

- 与**后台**：设备网关（`devicegateway`/`wecarddevicegateway`/`<设备网关>`）的接口/字段/错误码/proto，**是老设备兼容性的高发区**，任何变更强制对照 [`contract.md`](./contract.md)，并让后端与设备端 spec 作者双向确认（对应 code-patterns#4 兄弟 spec 字段号一致）。
- 与**识别端**：离线本地识别的特征格式/版本要与云端下发对齐（[`algorithm.md`](./algorithm.md)）。
- 与**前端**：设备状态/OTA 进度/属性上报的枚举语义与前端展示一致。

---

## 🖊 人工追加评审注意点（Human-Curated Review Points）

> 团队设备端专家在此追加专业注意点，skill 评审设备端时**必须逐条加载检查**。格式见 [`README.md`](./README.md) 第 3 节。

<!-- HUMAN-CURATED:START -->
<!-- 在本行下方按格式追加条目，勿删除首尾两个标记 -->

<!-- HUMAN-CURATED:END -->
