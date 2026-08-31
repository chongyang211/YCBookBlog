# O4 错误码体系梳理与治理 · 业务串讲

> **一句话定位**：刷掌设备端存在「SDK 内部码 / 模组码 / 服务端码」三级错误码混用、码表缺口、映射不全三类问题，本期把服务端码在协议通道与 SDK 回调中和模组码彻底隔离，补齐有通道暴露的模组码常量并修复文案体系，沉淀规范 + CI 校验防再犯。
> **场次**：60 分钟（代码量中等、难点集中，可按评审节奏浮动）　**听众**：同组开发 / 评审人 / 后续接手者
> **对照模版**：`06.技术模版/04.业务串讲的模版.md` 第 08 章　**代码基线**：仓库当前实现（已按 v2.0.0 终态落地，含少量 spec 残留待第 6 章指出）
> **本期重点**：`[trace_id:xxx]` 尾缀的拼接逻辑（第 4.2 章专深）是本期最易被误改、也最能体现代码设计取舍的一处，理由与原理见 ADR `palm_app_linux/docs/specs/2026-08-28-trace-id-start-new-trace-analysis.md`。

---

## 1. 需求与业务背景  [10min · 先现象后价值]

### 1.1 需求来源与问题
- **来源**：2026-08-24 代码审计（design 附录 A/B/C，带文件:行号实证）发现 O4 三级错误码体系有三处硬伤。
- **问题**：① **混用**——`activation_controller.cpp` 曾把 `server_code` 拼进 `error_msg`（`"server_code:" + to_string(...)`，唯一硬违规）；② **码表缺口**——SDK 缺 27 个有通道暴露的模组码常量，`getMessage()` 一直返回常量名导致文案体系整体失效，`isServerError()` 的 `>=1000` 判定与模组 IoT 码（≥1000）冲突；③ **映射不全**——服务端码→模组码映射待补齐。
- **目标**：服务端码不与 SDK/模组码混用同一字段（存量 1 违规清零）；SDK 码表覆盖全部有通道暴露的模组码且每码可取人类可读文案；服务端字典完成「映射/透传」判定并落地，规范与双端一致性校验机制沉淀。

### 1.2 业务边界
- **做**：服务端码字段隔离、27 个模组码常量补齐（含 TLS 拆分 20105）、`getMessage` 修复、`isServerError` 废弃、激活/识别/注册映射定稿、规范 + CI 校验。
- **不做**：服务端字典全量维护（仅接入识别/注册/加验三接口 + 激活链路典型场景）、codegen 单一源生成双端常量、V2 回调版本化、PalmCore 算法码治理、多语言文案（本期仅英文文案补齐）。

### 1.3 关键业务规则
- **三段数值区间**（规范核心，消除历史模糊）：
  - SDK 段 1~59；模组段 100~199（`isModuleError` 覆盖 100~199，306 为 legacy 特例排除）；模组 IoT 段 305xx~507xx（20xxx 网络 / 305xx 升级 / 404xx 激活）；
  - 服务端段 5~6 位：10003~100038、120017~120018，**仅经 `server_code` 字段透传，绝不进 `error_code` 字段**。
- **状态/口径**：`wire` 上 `error_code` 恒为模组码；`error_msg` 恒为人类可读纯文案；未知服务端码落 141（识别/注册/绑定）或 20407（激活）兜底。
- **唯一允许附加格式**：`[trace_id:xxx]` 尾缀，仅后台返回错误场景拼接。

### 1.4 改动范围
- **[新增]** `ErrorCode.java` +29 常量及 CODE_MAP 注册（27 个 + 激活设备未登记 40427 + TLS 拆分 20105）；规范文档 + CI 校验脚本。
- **[改动]** `activation_controller.cpp` `OnEnterError()` 改为分场景纯文案组包；`recognize_retcode.cpp` 映射表；`module_error_code.h`/`unified_errcode_iot.h` 常量；`palm_session_controller.cpp` 注册/绑定链路不透传 `server_code`；`upgrade_schd.cpp` 升级码出码。
- **[不动]** 推送通道 SDK 回调签名（零改动，天然向后兼容老 SDK）。

---

## 2. 整体设计与模块划分  [10min · 先图后讲]

### 2.1 架构与分层
```
          模组 (palm_app_linux, C++)                上位机 SDK (palm_manager, Java/C++)
  ┌──────────────────────────────┐          ┌──────────────────────────────────┐
  │ 0xA5 激活  activation_controller│  wire    │  ErrorCode.java (CODE_MAP)        │
  │   OnEnterError→BuildErrorNotif │ ─ret_code─▶ onResult(errorCode, errorMsg)   │
  │ 0xA3 识别/注册/绑定 session_ctrl │ (模组码)  │  getMessage/getErrorName/isKnown │
  │   MapRecServerCodeToModule      │          │                                  │
  │ 0xA6 升级 upgrade_schd          │          │  Map*ServerCodeToModule(各链路)   │
  │ qrcode_operation_handler (字符串)│          │                                  │
  └──────────────────────────────┘          └──────────────────────────────────┘
         ↑ 服务端码经 server_code 字段透传，绝不进 ret_code
```
分层关键：**模组侧把「服务端码 → 模组码」映射收敛在唯一维护点，再上 wire；SDK 侧只认模组码，服务端码只在 `server_code` 字段透传**。双端通过 CI 校验常量名+值一致性。

### 2.2 模块划分与职责

| 模块 | 职责 | 切分理由 |
|---|---|---|
| `ErrorCode.java` | SDK 常量表 + 文案查找（CODE_MAP） | 唯一对外常量源，UI 全部经此取文案 |
| `MapActivationServerCodeToModule` | 激活链路服务端码→模组码（唯一维护点） | 映射单点，防散落 |
| `MapRecServerCodeToModule` | 识别/注册/绑定链路映射（含 141 兜底） | 与激活链路正交，各自可单测 |
| `BuildErrorNotification` | 通用分场景组包（纯文案 + traceid 尾缀 + 截断） | 模板函数，各链路注入自己的 mapper |
| `check_error_code_alignment.py` | 双端常量一致性校验 | 防漂移的最后兜底，CI 卡口 |

### 2.3 核心数据流（以激活失败为例）
```
激活请求 → 服务端返回(server_code=268566642, message="Device not exists")
  → activation_service.cpp 透传 trace_id 到 ActivationResult
  → ActivationController::OnEnterError(error_code, error_msg, server_code)
  → BuildErrorNotification(error_code, error_msg, server_code, trace_id, MapActivationServerCodeToModule)
       server_code>0: error_code = mapper(268566642)=40427
                    error_msg = "Device not exists" + " [trace_id:xxx]"
  → NotifyActivationPhase(kError, 40427, "Device not exists [trace_id:xxx]")
  → SDK onResult(40427, "Device not registered on platform...")  ← getMessage(40427) 取文案
```

---

## 3. 逐模块实现讲解  [25min · 业务意图先行]

### 3.1 SDK 侧 `ErrorCode.java`（常量表 + 文案体系）　`palm_manager/.../constant/ErrorCode.java`

- **业务意图**：让上位机拿到错误码后能取到人类可读文案，而非一串英文常量名。
- **代码落点**：`CODE_MAP`（364-525）、`getMessage()`（547-550）、`isServerError()`（616-632）。
- **关键片段**：
```java
// 367: // [0] = constant name, [1] = message
// 524: CODE_MAP.put(code, new String[]{name, msg});
public static String getMessage(int code) {        // 547
    String[] entry = CODE_MAP.get(code);
    return entry != null ? entry[1] : "Unknown error(" + code + ")";  // 读文案位[1]
}
@Deprecated
public static boolean isServerError(int code) { return false; }  // 628-631 废弃,恒 false
```
- **要点**：`getMessage` 现已读 `entry[1]`（文案），注释 542-544 明示「prior to 2.0.0 返回常量名，已在 137332527 修复」。`isServerError` 已 `@Deprecated` 且恒返回 `false`——判断服务端失败的唯一可靠信号是专用 `serverCode` 字段。

### 3.2 模组侧映射单点　`recognize_retcode.cpp` / `activation_controller.h`
- **业务意图**：服务端码语义千差万别，只有「用户可理解可行动」的才映射成模组码上 wire，其余落兜底 + `server_code` 透传。
- **代码落点**：`MapRecServerCodeToModule`（recognize_retcode.cpp:7-27）、`MapActivationServerCodeToModule`（activation_controller.h:27-34）。
```cpp
// activation_controller.h:27 激活链路唯一维护点
inline int MapActivationServerCodeToModule(int server_code) {
  switch (server_code) {
    case kActSvrCodeDeviceNotRegistered:
      return ERR_PALM_ACTIVATION_DEVICE_NOT_REGISTERED;  // 40427
    default:
      return ERR_PALM_IOT_HTTP_SERVER_ERROR;  // 20407 兜底
  }
}
```
- **要点**：激活映射仅 1 码（设备未登记→40427），其余 20407 兜底靠 `error_msg` 文案区分；识别/注册共用 `MapRecServerCodeToModule`，`default→141`（SERVER_ERROR）。

### 3.3 0xA5 激活链路组包（本次核心修复）　`activation_controller.cpp` + `network.h`
- **业务意图**：消除 `server_code` 拼进 `error_msg` 的硬违规，并给客服提供可检索的定位凭据（即 `[trace_id:xxx]` 尾缀）。
- **代码落点**：`OnEnterError`（activation_controller.cpp:467-494）→ `BuildErrorNotification`（network.h:51-66）。
- **要点**：`OnEnterError` 现在把组包完全委托给 `BuildErrorNotification`，自身不再拼接 `server_code`（AC-1 红线清零）；消费完 `last_error_trace_id_` 后 `.clear()`（493）防残留。
- **重点深挖**：`[trace_id:xxx]` 尾缀为何这样拼、为何只在后台返回错误的场景拼、为什么 trace 必须取自 `IoTResponse` 而非公共槽位——见第 4.2 章专深。

### 3.4 0xA3 注册/绑定链路（不透传 server_code）　`palm_session_controller.cpp`
- **业务意图**：注册/绑定失败，上位机按模组码提示即可，原始服务端码不进 wire（避免双字段混用）。
- **代码落点**：palm_session_controller.cpp:241-248。
```cpp
if (code != 0) {
  const int module_code = MapRecServerCodeToModule(
      static_cast<int>(code), /*is_register_flow=*/true);
  send_result(BindPalmResult{module_code, message, session_id, "", "", 0, 0}); // server_code 置 0 不透传
}
```

### 3.5 升级 305xx / ActionCode 404xx 出码
- **升级** `upgrade_schd.cpp`：实发 5 码（30502/30503/30508/30511/30512）经 `NotifyUpgradeStatus` 上送（628-632 下载、738-743 安装）；其余 305xx 仅 HTTP 上报后台，无 wire 出码点 → 不进 SDK 常量表。
- **ActionCode** `qrcode_operation_handler.cpp:141/170-173`：失败仅以 `(event_type, error_msg)` 字符串上报，**数字码不上 wire** → 404xx 动作码 10 个（40405/40407/40408/40420-40426）刻意不注册。

### 3.6 规范 + CI 双端一致性校验　`palm_manager/docs/o4-error-code-spec.md` + `scripts/check_error_code_alignment.py`
- **业务意图**：把「码表只收录会出现的码」「双端对齐」写成机制，防漂移再犯。
- **代码落点**：CI 脚本比对 `ErrorCode.java` ↔ `module_error_code.h` ↔ `unified_errcode_iot.h` 常量名+值；`SDK_ALLOWED` 基线 27+2 码、`WHITELIST` 18 个刻意不注册码（与 3.5 的 305xx/404xx 白名单呼应）；规则 4 校验 `kActSvrCode*`/`kRecCode*` 常量集 vs 映射 case 闭环。

---

## 4. 核心代码实现细节与原理（重点）  [10min+ · 讲透原理]

### 4.1 挑选的核心代码
本期最值得讲透的 4 处：**① 拼接 `[trace_id:xxx]` 尾缀的逻辑与原理（最易被误改、最体现代价取舍）**；② `BuildErrorNotification` 分场景纯文案 + 截断；③ 映射单点收敛；④ 双端 CI 一致性校验。

---

### 4.2 专深重点：拼接 `[trace_id:xxx]` 尾缀——代码逻辑、理由与原理

> 这一段是本期最该讲清楚的地方。它看起来只是一行字符串拼接，但背后是一份 ADR（设计决策记录）级别的取舍：**为什么这个号一定查得到、为什么不能从公共槽位取、为什么只在特定分支拼**。讲不清，接手者极易「顺手优化」改坏它。

#### 4.2.1 完整链路闭环（数据从哪来到哪去）
```
IotDeviceRequestImpl 生成 traceparent → resp.trace_id（network.cpp:561）
  → ActivationResult.trace_id（activation_service.cpp 9 处失败分支透传）
  → last_error_trace_id_（activation_controller.cpp 存成员）
  → BuildErrorNotification 拼 " [trace_id:xxx]" 上屏（activation_controller.cpp:478）
```
这是 O4 规范 §4 的核心闭环：激活失败且**取得服务端响应**时，error_msg 尾缀携带失败请求的 trace_id，客服按其在服务端检索日志。整条链路上有 13 处 `r.trace_id = resp.trace_id;`（activation_service.cpp 9 处 + domain_validator.cpp 4 处），任何一处漏写都是静默故障——失败恰恰是客服最需要单号的场景。

#### 4.2.2 拼接代码逐行（落在哪里、怎么拼）
拼接发生在 `BuildErrorNotification`（network.h:64-66，由 `OnEnterError` 在 478 行调用，mapper 注入 `MapActivationServerCodeToModule`）：
```cpp
// activation_controller.cpp:478  OnEnterError 内调用（组包全委托给通用函数）
const auto notification = BuildErrorNotification(error_code, error_msg, server_code,
                                                 last_error_trace_id_,
                                                 MapActivationServerCodeToModule);
// network.h:64-66  唯一的拼接点
if (!trace_id.empty()) {
  suffix = " [trace_id:" + trace_id + "]";   // 唯一允许的附加格式
}
```
而在它之前，`last_error_trace_id_` 的来源是：`GenerateTraceparent` 在 network 层生成 `trace_id`（`network.cpp:240-247`，锁内生成并写入）→ 随 `IoTResponse` 带回（`network.cpp:561` 写 `resp.trace_id`）→ `activation_service.cpp` 9 处失败分支透传到 `ActivationResult.trace_id` → `activation_controller.cpp` 存进成员 `last_error_trace_id_` → `OnEnterError` 消费后 `.clear()`（493）。**生成、透传、消费、清理四步闭环**。

#### 4.2.3 为何只在 `server_code > 0` 分支拼（理由 + 原理）
`BuildErrorNotification` 里 `suffix` 的拼接被包在 `if (server_code > 0)` 内，`server_code == 0`（连不到后台）的分支**不拼尾缀**。理由：
- **服务端只在收到请求时，才会有该请求的日志**。trace_id 是「本次请求」的凭据，后台拿到请求才落日志、才能按 trace 检索。
- **连不到后台（server_code==0）时，服务端根本没收到请求，没有任何日志可查**。此时若还拼上 trace_id，客服拿去检索只会落空，反而误导定位——所以宁可不拼。
- 对应的 `error_msg` 在 `server_code==0` 时是网络库底层错误串（如 `connect to <host>:443 failed`、TLS 证书错误串），本来就不需要 trace 也能定位网络层。

#### 4.2.4 为何 trace_id 必须取自 `IoTResponse`、不能取公共槽位（竞态论证）
这是最易被「优化」坏的地方，讲清它就能挡住大部分误改。

`trace_id` 不是随便哪都能拿的。它在 `GenerateTraceparent` 里生成，生成在 network 层，**上层激活服务压根拿不到生成时的那串值**——除非 network 把这次请求的 trace 随响应带回。于是设计让 `IoTResponse::trace_id` 随响应带回（network.cpp:561）。

**为什么不能用 `current_trace_id_` 公共槽位代替？** 那个槽位是全 Network 共享的：
- 激活三接口（start / confirm-key-change / confirm）靠它 + `start_new_trace` 参数串联；
- 但**心跳、指令、影子等并发请求随时会改写这个槽位**。
- 失败后若去回读公共槽位，大概率读到**别的请求**的 trace——这才是真正的「查错单」：客服按号查到的是心跳的日志，而不是激活失败的日志。
- `IoTResponse::trace_id` 不一样：号是**发请求时当场记下来、跟着本次响应一起带回来的**（不是事后去公共便签上现看）。所以不管有没有并发、号有没有被复用，用户报给客服的号一定就是失败那次请求真正发出去的号。覆盖激活三步与域名校验等所有走 `IotDeviceRequestImpl` 的路径。**唯一不带号的情况：压根没连上后台**（服务端没收到请求，查也查不到）。

#### 4.2.5 结构性保证（一句话总结原理）
> 号随请求发出时记下、随响应带回 → 必然对应失败那次通信。这条属性不是靠加锁/比对保证的，而是靠「所有权随请求流动」保证的：谁发的请求，谁的响应带回自己的号。

#### 4.2.6 设计取舍 ADR（字段保留 / 参数可删但不直接删）
ADR `2026-08-28-trace-id-start-new-trace-analysis.md` 给了决策：

| 议题 | 结论 | 关键理由 |
|---|---|---|
| `IoTResponse::trace_id` 字段 | **保留** | 删了激活失败就没有上屏单号，无法拼接；它是整条链路上唯一的无竞态回传通道 |
| `current_trace_id_` 槽位 / `trace_mutex_` | **保留不动** | 不改 `GenerateTraceparent`，回归成本最低 |
| `start_new_trace` 参数 | **可以去掉（按做法 B）但不可直接删** | `override_trace_id` 够用，但须先把 trace 所有权从网络层槽位**上移**到 `ActivationService`（start 时生成、三接口 override 透传），串联由「可能被心跳打断」变**强保证**；只删参数不挪职责 = 丢跨步骤链路；改外部清空槽位 = 有竞态（心跳占位，激活链粘到别人身上） |

做法 B 的比方：现在激活把「流水号」记在 Network 的公共便签上（`current_trace_id_`），谁都能往上写——心跳一写就把激活的号盖掉，前后对不上号；改成激活自己把号揣兜里（`ActivationService` 成员），发请求时直接递给 Network，谁也抢不走，跟刷掌「揣着号去请求」同一套路。好处：号不会被别人盖掉、代码一眼看出这条请求串到哪条链、Network 底层零改动。

> **接手提醒**：`start_new_trace` 即便按做法 B 删掉，也**必须先上移所有权**；只删参数会让激活三条接口各用各的 trace，客服只能拿到失败那一步的日志，拿不到前序步骤（如 start 下发的密钥协商）链路——与服务端「串成同一条激活链」的既有约定冲突（做法 D 已否决）。

#### 4.2.7 回归验证清单（改动后必跑）
1. 激活成功：start / confirm-key-change / confirm 三段 trace-id 一致；
2. 并发压测：激活中触发心跳/指令，三请求 trace-id 仍一致（验证「链可断」已修复）；
3. 失败上屏：失败请求日志 traceparent 与 error_msg 尾缀 `[trace_id:xxx]` 的 trace 段一致；
4. 重试不串单：连续两次扫码激活，两次尝试 trace-id 不同；
5. 开机补发 confirm：trace 为空时走默认路径，traceparent 正常；
6. 刷掌 `recognize`：traceparent 与改造前一致（override 零改动验证）；
7. 网络失败（20xxx）：error_msg **不拼** trace 尾缀（对应 4.2.3 的分支）。

---

### 4.3 `BuildErrorNotification` 分场景纯文案与截断（其余原理）
```cpp
template <typename Mapper>
std::pair<int, std::string> BuildErrorNotification(
    int error_code, const std::string& error_msg, int server_code,
    const std::string& trace_id, Mapper&& mapper) {
  int notify_code = error_code;
  std::string msg = error_msg;
  std::string suffix;
  if (server_code > 0) {                       // ① 后台返回错误(有服务端响应)
    notify_code = mapper(server_code);          //   映射后模组码(40427/20407)
    if (msg.empty()) msg = "Server error: code=" + std::to_string(server_code);
    if (!trace_id.empty()) suffix = " [trace_id:" + trace_id + "]";  // 见 4.2
  } else {                                      // ② 连不到后台(无服务端响应)
    if (msg.empty()) msg = "Network error";
  }
  // 防御截断: 总长 > 512 截断 message 保前缀 + "...", 尾缀完整保留
  ...
  return {notify_code, msg + suffix};
}
```
**为什么这么写（原理）**：
- **分场景而非 JSON 化**：终态方案推翻了早期「error_msg JSON 化 + 回调加参」路径——JSON 化要老 SDK 改解析、破坏向后兼容；纯文案「error_msg 原本即字符串通道」天然兼容新老组合。
- **尾缀为唯一允许格式**：规范登记 `[trace_id:xxx]` 为唯一附加格式，防止未来又有人把 `server_code`、JSON 夹带进 `error_msg`（治理 AC-1 的长效约束）。
- **防御截断保尾缀**：0xA5 `error_msg` 是变长 string（uint16 长度前缀），超 512 截断 `message` 保前缀 + `...`，`[trace_id:xxx]` 尾缀完整保留（定位凭据不可丢）。

### 4.4 映射单点收敛原理
- **为什么单点**：激活与识别/注册是两条正交链路，各自一个 `Map*ServerCodeToModule` 维护点 → 各自可单测、可复用，新增映射只改一处（spec FR 可维护性）。
- **为什么激活只映射 1 码**：设备未登记（268566642）是「用户可理解可行动」的典型场景（找运营加设备），其余服务端拒绝由 `error_msg` 服务端文案直接给原因，无需每码一映射——映射收敛到 1 码避免无谓膨胀。

### 4.5 双端 CI 一致性校验原理
- **机制**：脚本静态解析三处常量定义，比对「常量名 + 值」集合；`SDK_ALLOWED` 终态基线（27 实发码 + 20105 + 40427），`WHITELIST` 18 个刻意不注册码（死常量告警）；规则 4 校验服务端码常量集与映射 case 集闭环（FR-3.4 补录防漏）。
- **复杂度**：纯静态解析 O(N)，CI 卡口 `--strict` 不一致即 exit 1；`--selftest` 内存自测。

### 4.6 性能与复杂度
- `BuildErrorNotification`：O(1) 模板内联，无锁；`mapper` 为 switch，O(1)。
- `getMessage`：HashMap 查找 O(1)。
- CI 校验：O(N) 文件解析，仅 PR 触发，无运行时开销。

---

## 5. 边界、异常与测试  [5min · 别一笔带过]

### 5.1 异常与降级
- `server_code=0`（无服务端响应）：走 ② 分支，error_msg 为网络库底层错误串（如 `connect to <host>:443 failed`、TLS 证书错误串），**不拼尾缀**（见 4.2.3）。
- `getMessage` 未知码：返回 `Unknown error(code)`（550），不崩。
- `isServerError` 废弃：恒 `false`，仅保留签名兼容，新代码不得用于服务端码判断。

### 5.2 边界条件
| 场景 | 行为 |
|---|---|
| 未知服务端码（激活） | 20407 兜底 + error_msg 服务端文案 |
| 后台报错但 trace_id 缺失 | 仅 message 纯文案，不拼尾缀 |
| error_msg 超长 | 截断 message 保前缀，`[trace_id:xxx]` 尾缀完整 |
| 老 SDK + 新模组 | 老 SDK 直接展示服务端 message，不 crash、无解析变化 |
| 306 USER_NOT_ENROLLED | legacy 特例，`isModuleError` 排除（ErrorCode.java:575-577） |
| 新服务端码 + 老模组 | default 落 141 + server_code 透传原值 |

### 5.3 测试覆盖
- **已覆盖**：各通道透传（识别 141+server_code / 激活 40427+分场景文案 / 升级 305xx）；27 新常量 `isKnown/getErrorName/getMessage`；`MapRecServerCodeToModule` 10023→118 与 default→141；`MapActivationServerCodeToModule` 40427 命中与 20407 兜底；`OnEnterError` 组包（后台报错+尾缀 / 连不到后台无尾缀 / trace 缺失无尾缀）；**trace 链路 13 处透传 + 尾缀拼接**（见 4.2.7 回归清单）。
- **未覆盖（open 风险）**：`getMessage` 修复后的 **Demo 全量错误提示 UI 回归**（spec 风险表标注 open，需设备/人工，T5.5）——这是本期唯一未关闭项，接手者需重点跟进。

---

## 6. 踩坑与接手注意事项（含审阅发现）  [审阅 + 雷区]

### 6.1 开发中坑
- **终态方案反复**：决策 3 曾定「error_msg JSON 化 + 回调加参」，后被推翻为「纯文案 + 零加参 + 映射收敛 1 码」——**任何在这两次决策间写的代码都已被回退**，接手别被历史分支误导。
- **码表「死常量」陷阱**：18 个码（404xx ActionCode / 部分 305xx）代码库无 wire 出码点，注册进 SDK 反而误导接入方以为能收到 → 必须留在 WHITELIST 不注册。
- **`[trace_id:xxx]` 尾缀的隐形坑**：它依赖 13 处透传 + `IoTResponse::trace_id` + `current_trace_id_` 槽位三件套，任一处「顺手精简」都会静默丢号（编译通过、正常路径测不出，只在失败时暴露）。改动 trace 链路前先通读 4.2。

### 6.2 接手者雷区地图（⚠️ spec 与代码实测差异，已审阅）
本次代码审计式审阅发现 **spec 文档与当前代码存在 5 处不一致**，接手/评审务必以**代码为准**：

| # | spec 说法 | 代码实测 | 影响 |
|---|---|---|---|
| R1 | 激活映射「268566642→**142**」，且 `+142` SDK 常量 | 实际→**40427**（`IOT_ACTIVATION_DEVICE_NOT_REGISTERED`，ErrorCode.java:481）；**ErrorCode.java 无 142 常量** | spec 的「142」「+142 常量」均不成立；更坑的是 `activation_controller.cpp:472` 注释仍写「268566642→142」——**代码自身注释也已 stale** |
| R2 | FR-3.2「**10020**→118 新增映射」 | 代码 `recognize_retcode.cpp:22-23` 是 `kRecCodePalmAlreadyEnrolled→PALM_ALREADY_ENROLLED(118)`（即 10023），**无 10020 显式 case** | spec 的「10020」命名漂移，以代码常量名为准 |
| R3 | 「`IsNetworkError()` 排除 20105」 | 仓库**无 `IsNetworkError()`**；实为 `IsActivationErrorRetryable`（unified_errcode_iot.h:171-185）白名单**不含 20105** | 函数名引用错误，串讲/评审别照抄 spec 函数名 |
| R4 | `isServerError` 区间 `>=1000` 误判模组 IoT 码 | 已改为恒 `false` + 注释说明「模组码跨 100~508xx，分区已不存在」（ErrorCode.java:627-631） | 旧 `>=1000` 断言用例（ErrorCodeTest.java:114-117）已随废弃改写 |
| R5 | 1000+ 分区注释「Server business errors」 | 注释保留（352-355/518）但**段内已无常量** | 仅历史兼容，勿据此认为存在 1000+ SDK 常量 |

**给接手者的硬提醒**：
- 🔴 **改激活映射前先改 `activation_controller.cpp:472` 的 stale 注释**，否则下个人又按 142 理解。
- 🔴 新增服务端码映射时**只改 `Map*ServerCodeToModule` 单点**，并同步三处：模组 case + SDK 常量 + 文案 + CHANGE，再跑 CI 脚本。
- 🟡 `server_code` 字符串**永远禁止拼进 error_msg**，唯一例外是 `[trace_id:xxx]` 尾缀（network.h:28 约束）。
- 🟡 改 `BuildErrorNotification` / trace 链路前，先回看第 4.2 章与 ADR，确认没动「生成→透传→消费→清理」闭环与「只在 server_code>0 拼」的分支。
- 🟡 error_msg 超 512 截断逻辑在 `BuildErrorNotification`，改长度上限记得保尾缀。

---

## 7. 参考文档
- 需求 spec：`CoSpec/specs/v2.0.0/137332527-o4-error-code-governance.md`
- 关联 design：`designs/v2.0.0/137332527-o4-error-code-governance-design.md`（附录 A/B/C 审计实证）
- 规范文档：`palm_manager/docs/o4-error-code-spec.md`（双端互链 `palm_app_linux/docs/O4-ERROR-CODE-SPEC.md`）
- CI 脚本：`palm_manager/scripts/check_error_code_alignment.py`
- **trace 链路 ADR（本期重点）**：`palm_app_linux/docs/specs/2026-08-28-trace-id-start-new-trace-analysis.md`
- 关键代码：`ErrorCode.java`、`activation_controller.cpp/.h`、`recognize_retcode.cpp`、`module_error_code.h`、`unified_errcode_iot.h`、`palm_session_controller.cpp`、`upgrade_schd.cpp`、`qrcode_operation_handler.cpp`、`network.h/.cpp`
