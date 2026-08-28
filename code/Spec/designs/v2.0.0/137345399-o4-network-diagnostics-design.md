# Technical Design — O4 网络诊断措施（网络检测 / 测速 / 弱网运维）

> **关联 Intake**: `docs/intake/v2.0.0/137345399-o4-network-diagnostics.md`
> **Story ID**: 137345399
> **Author**: chongyyang
> **Reviewers**: [TBD]
> **Created**: 2026-08-26
> **Updated**: 2026-08-28
> **Status**: draft

## 需求背景与目标

> 忠实复述 intake 的需求与动机（不扩写）。

- **背景**：O4 缺乏网络问题的现场定位手段——设备网络问题、分网（网段隔离）问题、网络正常但路由策略致客户环境不通等场景无法快速排查；无网速/弱网度量。
- **技术目标**：
  1. **网络检测模块**：物理层/本地层/连接层/服务层四层检测（复用 O2 资产），暴露问题辅助排查；与业务解耦的功能模块 + 简易 API；触发：扫码 + 上位机 Demo 按钮
  2. **网络测速模块**：轻量测速；网络切换 + 周期（30min 等）触发；弱网标准明确；定时测速、上报、通知上位机
  3. **运维文档**：面向客户的网络问题排查/解决手册
- **本方案要回答的核心问题**：O2 资产如何移植（附录 A）；模块归属与 API 形态；错误码归属（联动 137332527 规范）；测速与弱网标准选型；上位机协议通道；上报链路。

## 对网络检测的认知

**好的网络检测 = 让咱们前线在 30 秒定位问题层级 + 让用户在 3 秒知道下一步该点哪个按钮**。

1. **网络问题不能笼统判断**，必须**分层定位**。物理/本地/连通/服务/业务是五个截然不同的故障域，不能混着诊断。
2. **检测要从底向上，短路返回**，物理层都没了，去检测应用层是浪费用户的耐心和电量。
3. **结果要"用户语言"**，不能让用户看到 IP、看到 ping 值、看到 DNS record；必须翻译成"下一步该做什么"。这一步很关键啊！

## 现状分析

> 2026-08-26 完成代码侦察（O2 资产 + O4 网络层），实证带文件:行号。

### A. O2 可复用资产详析（`iotservice_linux/src/operations/strategy/`，14 文件）

> **取材分支**：O2 资产位于 iotservice_linux **master**（commit 7603f78 引入、870bb63 迭代，`src/operations/strategy/`；develop 基线无此目录）——移植以 master 为源（本地实证 + MR 评审确认，2026-08-26）。

| 组件 | 形态 | 检测手段（实证） |
|---|---|---|
| `BaseNetworkStrategy` | 策略基类：`GetName() / Execute() / ExecuteAsync(callback)`；`DetectionResult{success, level, strategy_name, error_code, error_message, details<map>, ToJsonString()}` | — |
| `PhysicalLayerStrategy` | 物理层 | `getifaddrs` 枚举接口（跳 loopback）+ `/sys/class/net/<iface>/{flags,carrier}` 读状态/载波 + `getifaddrs` 取 IP；四种失败分类（无接口/无载波/接口 DOWN/UP 无 IP）→ 10011~10015 |
| `LocalNetworkStrategy` | 本地层 | 网关/路由查找（`/proc/net/route` 系）→ 10021~10023 |
| `ConnectivityStrategy` | 连接层 | ping 网关连通测试（`TestGatewayConnectivity/PingHost`，connectivity_strategy.cpp:20-29）→ 10031~10032 |
| `ServiceStrategy` | 服务层 | 服务 host health endpoint 探测 + 基础 HTTP 连接回退（service_strategy.cpp:33-56，依赖 O2 `network/network.h` + `network/http_request.h`）→ 10041~10044 |
| `NetworkDetectorManager` | 单例：按序执行全量检测（**遇失败停止**），`PerformFullDetection()` 同步 + `PerformDetectionAsync(callback)` 异步 | — |
| `unified_network_errcode.h` | 统一诊断码：分段 1001x 物理 / 1002x 本地 / 1003x 连通 / 1004x 服务（**10xxx 段**） | — |

**依赖分析**（移植适配点）：`system/system_interface.h`（**O4 有同名模块** `facility/system/system_interface.h`，`IsNetworkInterfaceUp` 等接口同名）、`thread/threads.h`（O4 有 `facility/thread/threads.h`，`Threads::IotThread()->Post` 可用）；**风格差异**：O2 为 callback 风格（`std::function` + Post），O4 主流为 asio 协程风格（`awaitable` / `CoSpawn`，见 network_monitor_service.cpp:21-31）；ServiceStrategy 依赖 O2 `network/` HTTP 模块（需替换为 O4 等价物或移植）。

**M3/M4 网络检测现状参照**（横向参考）：M3/M4 采用「采集 → 聚合 → 广播 → 消费」链路——HAL 层双厂商采集（M3 每 3 秒轮询 SDK / ping 网关三级探测，获取 eth0 接口存在性、IP、网关连通等；XiKe 订阅 D-Bus 事件），经 DeviceHalManager 缓存去重后以 Qt 信号广播，供 HTTP 请求快速失败、切网自愈和 UI 状态展示消费。其三级探测与 O2 四层检测思路互补；本方案复用 O2 四层（按需触发式深度检测），M3/M4 的常驻轮询广播模式仅作架构参照，不直接移植。

> **M3/M4 实测经验（2026-08-28 intake 补充，检测规则硬约束）**：七条实测问题已结构化入 intake（carrier 失效→Ping 网关唯一判据禁公网 / 无 DHCP「已连无 IP」特殊态 / 路由 metric 优先级告警 / DNS 生效值输出 / 静态 WiFi 按 SSID 隔离 / 以太网>WiFi 无感切换 / 静态配错只提醒不切换）——检测相关五条已落入 spec FR-1.1「M3/M4 实测经验吸收」表；配置策略类（#5/#6/#7）作为检测结果建议文案与运维手册处置指引的设计输入。

### B. O4 网络层现状（palm_app_linux）

| 组件 | 位置 | 现状与复用价值 |
|---|---|---|
| `NetworkWatcher` | `biz/comm/network_watcher.h` | **网络事件源已存在**：监听 4 类信号（ConnectChanged / StrengthChanged / DefaultNetworkChanged / OfflineModeStateChanged，均 boost::signals2）+ 网络状态 UI 更新先例（`UpdateNetworkStatusUi`）——**测速「网络切换触发」天然挂载点** |
| `NetworkMonitorService` | `service/network/network_monitor_service.cpp:20-32` | 网络变化响应先例：监听 `NetworkConnectChangedSig` → 延迟 1s → `QueryNetworkType()`（协程风格）→ 更新 NetworkInfo + 触发 NTP/MQTT 重连——新模块可挂同一信号链 |
| `NetworkInfo` | `facility/network/network_info.h` | 全局网络状态（NetworkType/ConnectState 单例） |
| `QRCodeOperationHandler` | `service/operations/qrcode_operation_handler.cpp:55-70` | **扫码触发挂载点**：`TryHandle(code_content)` → `ActionCodeProcessor::IsActionCode` 判断操作码 → 处理（ActionCode 体系，已有防抖/并发保护；流程含 NTP+network 等待，kProcessTimeout=35s）——「扫码触发检测」可扩展操作码或挂接处理链 |
| 上报体系 | `service/report/` | **成熟 metrics 上报先例**：`metrics_resource_reporter` + `device_resource_metrics_builder`（资源指标）+ `ReportEvent`（palm_report_item.h:126）——网络测速结果可参照新增 network metrics |
| 协议扩展点 | `pos_link`（commands.yaml + `pos_transferer.cpp` 注册） | 新增上位机指令的既有机制（137332527 已验证） |
| MQTT | `service/mqtt/mqtt_service.h` | 长连接（NetworkMonitorService 触发重连先例）——上报通道候选 |

### C. 关键约束与衔接

1. **错误码规范衔接**（137332527，v2.0.0 已评审）：O4 三段数值区间（SDK 1~59 / 模组 100~199 / 模组 IoT 305xx~507xx）+ server_code 独立通道先例；O2 诊断码 10xxx 与 PalmIotErrorCode 10100 同属 1xxxx——**若并入业务错误码体系存在段位冲突**。业务运行期网络错误（PalmIotErrorCode 20xxx，如 20102 网络错误）与诊断码（10xxx）是两个体系：前者是业务请求失败错误码，后者是主动检测的分层定位码——并存不冲突，运维文档中说明二者关系
2. 弱网对刷掌业务的影响：识别/注册需上传掌图（带宽敏感），激活走 HTTPS——弱网标准应结合业务链路特征定值


### D. 需求场景 → 检测能力映射（intake 1.3 场景为典型示例；四层检测为通用排查框架）

| 客户场景（intake 1.3 **典型示例，非穷举**） | 检测层覆盖 | 定位产出 |
|---|---|---|
| 设备网络有问题（网线脱落 / 网卡未激活 / 无 IP） | 物理层（接口枚举 + UP/载波/IP 检查） | 1001x 诊断码 + 接口明细（details：各接口 status/ip/carrier） |
| 分网有问题（网段隔离 / 网关路由异常） | 本地层（网关/路由查找）+ 连接层（网关 ping） | 1002x/1003x 诊断码，区分「没有网关路由」与「网关不可达」两种断点 |
| 网络正常但路由策略等连不通客户环境（防火墙 / ACL / 服务不可达） | 服务层（health endpoint 探测 + 基础 HTTP 回退） | 1004x 诊断码，区分「服务 API 异常」与「完全不可达」 |

> **通用性原理**：任何网络故障最终都表现为某一层的失败（接口 / 路由 / 连通 / 服务）——intake 1.3 三场景仅为典型示例，**四层按序检测是通用排查框架，覆盖各种网络问题**。未列举场景同样落于对应层：DNS 异常 / 代理配置错误多体现为服务层（域名解析失败 → 探测不通）、IP 冲突 / 子网掩码错误体现为本地层或连接层失败、运营商侧问题体现为连接层/服务层不可达。
>
> 四层按序检测、遇失败停止（NetworkDetectorManager 机制）——**第一失败层即网络断点**，排查遵循「由近及远」路径，一次检测即定位断点层级；配合测速模块弱网分级，另覆盖「能连通但质量差」场景。

### 方案速览（三块需求对应结论）

| # | 需求块 | 方案结论 | 动作落点 |
|---|---|---|---|
| 1 | 网络检测 | 移植 O2 四层检测到 `facility/network/diagnosis/`（协程适配），诊断码 10xxx 独立体系 JSON 透传；扫码 + Demo 按钮（新增指令）双触发，扫码结果走设备 UI | FR-1 + FR-3 |
| 2 | 网络测速 | HTTP 小文件下载（3~5s / <100KB）+ 弱网四级分级（**终端内置阈值，终端闭环**）；网络切换 + 30min 周期触发；report 体系上报 + push 通知上位机 | FR-2 + FR-4 |
| 3 | 运维文档 | 面向客户的现场排查手册（检测入口 / 诊断码对照 / 弱网标准与处置 / 常见问题对照） | FR-5 |

## 总体方案

**复用 O2 四层检测架构移植到 O4 + 新增轻量测速模块，两者封装为独立网络诊断功能模块（facility 层），业务侧通过三条通道消费（扫码触发 / Demo 按钮指令 / 周期+事件自动），结果经检测详情透传上位机与 metrics 上报，弱网标准贯穿测速与运维文档。**

```
┌─ facility/network/diagnosis/（新，功能模块，与业务解耦）
│   ├─ detection/   ← 移植 O2 strategy 四层 + Manager（协程风格适配）
│   └─ speedtest/   ← 轻量测速 + 弱网分级判定
│        ↑ 简易 API（同步/协程双形态）
├─ 触发通道
│   ├─ 扫码：QRCodeOperationHandler 扩展操作码 → 检测 → 设备 UI 展示
│   ├─ 上位机：pos_link 新增指令（Demo 按钮 → req/resp 检测结果）
│   └─ 自动：NetworkWatcher 网络切换事件 + 30min 周期（测速）
├─ 结果消费
│   ├─ 上位机通知：测速/弱网结果推送（新指令 or 复用推送通道）
│   └─ 云端上报：参照 metrics_resource_reporter 新增 network metrics
└─ 交付物：《O4 网络问题现场排查手册》（运维文档，面向客户）
```

**设计原则**：
1. 检测/测速为**纯功能模块**（facility 层），不依赖 biz/service 业务逻辑；业务通过简易 API 调用
2. 诊断结果（DetectionResult/SpeedTestResult）**自成体系**（结构化 JSON），不与业务错误码混用（遵循 137332527「不混用」规范精神）
3. O2 检测逻辑移植保真（算法/分类/诊断码原值），仅做工程适配（协程风格 / 依赖替换）

## 方案对比

### 决策点 1：O2 资产复用方式

| 方案 | 优点 | 缺点 | 结论 |
|------|------|------|------|
| A. 移植检测逻辑 + O4 协程风格适配 | 保留 O2 已验证的检测算法与错误分类；与 O4 工程风格（awaitable/CoSpawn）一致；检测实现以同步系统调用为主，适配成本低 | 需重写 ExecuteAsync 为协程；ServiceStrategy 的 HTTP 依赖需替换 | ✅ 推荐 |
| B. 直接拷贝 strategy 目录原样移植 | 最快 | callback 风格与 O4 主流协程风格割裂，双风格并存增加维护成本 | ❌ |
| C. 仅参考架构、按 O4 规范重写 | 风格最纯 | 重复造轮子，O2 四层检测算法（接口枚举/载波/路由/ping/health 探测）需重新验证 | ❌ |

**倾向**：A — 理由：检测核心是同步系统调用（getifaddrs/读 sysfs/ping），协程适配仅在外壳（异步触发与回调）；O2 错误分类逻辑（GenerateDetailedErrorMessage 四情况分类）是排查价值核心，原样保留。

### 决策点 2：模块归属与 API 形态

| 方案 | 优点 | 缺点 | 结论 |
|------|------|------|------|
| A. `facility/network/diagnosis/`（检测+测速同模块） | 与 NetworkInfo 同属 facility/network，网络域内聚；纯功能无服务状态，符合 facility 定位 | — | ✅ 推荐 |
| B. `service/network/` 扩展（与 NetworkMonitorService 同处） | 网络相关聚合 | service 层定位是有生命周期的服务，纯功能放 service 层次不符 | ❌ |
| C. 独立顶层模块 | 解耦最彻底 | O4 无此层级先例，破坏现有分层 | ❌ |

**倾向**：A — API 形态：`NetworkDiagnosis::DetectNetwork() -> DetectionReport`（协程）+ `DetectNetworkAsync(callback)`（兼容回调）+ `SpeedTest() -> SpeedTestReport`；两 Report 自带 `ToJsonString()`（O2 先例）。

### 决策点 3：检测结果错误码归属（联动 137332527 规范）

| 方案 | 优点 | 缺点 | 结论 |
|------|------|------|------|
| A. 诊断码独立体系（沿用 O2 10xxx 原值，随 DetectionResult JSON 透传/上报，**不并入** PalmIotErrorCode） | 检测算法零改动；诊断码与业务错误码彻底解耦（JSON 独立字段承载，wire 上不混入 error_code 字段——与 137332527「server_code 独立通道」同构） | 10xxx 与 PalmIotErrorCode 10100 数值段重叠，需在规范中显式登记「诊断码不入错误码表」防混淆 | ✅ 推荐 |
| B. 映射并入 O4 错误码段（如 PalmIotErrorCode 新增网络诊断段） | 纳入统一码表管理 | O2 检测逻辑需改码值；诊断码语义（分层层级）与业务错误码语义不匹配；新增 20+ 码 | ❌ |
| C. 重新设计 O4 专属诊断码段 | 段位干净 | 改动大，O2 验证逻辑需重映射 | ❌ |

**倾向**：A — 理由：诊断码是「检测结果」不是「业务错误」；JSON 结构化透传天然隔离；归属说明随本需求运维手册（手册第 6 章双体系关系），**不关联《O4 错误码规范》**（已确认 2026-08-26）。**定义文件（2026-08-28 负责人决策）**：不新建独立 `unified_network_errcode.h`（不随移植保留 O2 文件）——10xxx 诊断码枚举**并入现有码表文件 `unified_errcode_iot.h`**（同文件独立 enum + 段注释，物理同文件、逻辑分离，防误占且免新增文件）。

### 决策点 4：测速实现选型（轻量）

| 方案 | 优点 | 缺点 | 结论 |
|------|------|------|------|
| A. HTTP 小文件下载测速（终端内置 URL，3~5s 采样窗口） | 直接度量带宽（刷掌上传掌图正是带宽敏感）；实现简单（复用 HTTPS 客户端）；无配置依赖（终端闭环） | 占用少量带宽（轻量窗口 <100KB 级）；依赖探测源可用性（URL 变更需发版，见决策 8） | ✅ 推荐 |
| B. 延迟探测（ping/TCP connect RTT） | 带宽占用几乎为零 | 只测延迟不测带宽，弱网判定信息不足（掌图上传是带宽瓶颈） | ❌ 不足以判定弱网 |
| C. 复用业务请求 RTT 统计（心跳/识别请求采样） | 零额外流量 | 依赖业务流量存在（空闲设备无样本）；样本不控（受业务负载影响） | ❌ 可作补充不作主手段 |

**倾向**：A 为主 + C 为辅（业务 RTT 异常可触发即时测速）— 采样窗口短、周期 30min，单次流量开销可忽略。

**探测源补充评估（2026-08-28）**：复用激活 license 文件作测速载荷的可行性取决于实测大小——≥50KB 且 URL 稳定可作降级候选；过小则 3~5s 采样窗口带宽失真，且业务资产复用有 URL 稳定性与语义耦合顾虑。**倾向维持 ~100KB 专用测速文件**；Plan 阶段实测 license 文件大小后确认（见风险表）。

### 决策点 5：弱网标准（需给出标准，具体值评审定）

**分级模型**（建议值，结合刷掌业务链路特征：掌图上传带宽敏感 + 激活 HTTPS 延迟敏感）：

| 等级 | 建议阈值 | 业务含义 |
|---|---|---|
| 良好 | 带宽 ≥ 2 Mbps 且延迟 ≤ 200ms | 掌图上传与交互流畅 |
| 一般 | 1 ~ 2 Mbps 或延迟 200~500ms | 可用，识别上传略慢 |
| **弱网** | **带宽 < 1 Mbps 或延迟 > 500ms 或连续 2 次测速失败** | 识别上传明显变慢/超时风险 |
| 断网 | 检测失败（连通层/服务层不过） | 无法联网 |

- 判定动作（本期）：**分级标记 + 上报 + 通知上位机**（提示性），不做自动降级动作（如降画质/断流）——降级策略留待后续需求，运维文档指导现场处置
- 阈值**终端内置**（编译期常量，终端闭环，无 IoT 配置依赖）；上线后经校准确认需调整时走版本迭代（校准结论见决策点 9）
- **海外适用性（2026-08-28）**：默认阈值以国内主流网络为基准，海外网络基础差地区弱网占比预期偏高——弱网分级为提示性不阻断业务；推送按等级变化触发（决策 6，避免持续打扰）；海外阈值纳入决策 9 按区域分别复盘、可单独调整走版本迭代

### 决策点 6：上位机协议通道（Demo 按钮触发 + 测速通知）

| 方案 | 优点 | 缺点 | 结论 |
|------|------|------|------|
| A. pos_link 新增 2 条指令：`NetworkDiagnosis`（req/resp：上位机触发检测，resp 带全量结果 JSON）+ `SpeedTestNotify`（module_push：测速/弱网结果推送上位机） | 通道语义独立清晰；req/resp 与推送各归其位（commands.yaml 既有两种模式先例）；结果走独立字段不混 error_code | 双端协议代码生成 + Demo 按钮 UI 改动；**检测耗时（<10s，断网各层超时叠加可能更长）与 req/resp 协议超时的匹配需 Plan 解决**（分层超时预算 / 指令独立超时 / 受理+推送异步化，异步化会改变指令形态） | ✅ 推荐 |
| B. 复用 BiComm（0xAD）扩展子指令 | 不加新指令号 | BiComm 语义是双向业务指令通道，混入诊断功能语义不符 | ❌ |
| C. 仅 Demo 本地触发（不经协议） | 无协议改动 | 违背需求（检测跑在模组侧，上位机必须经协议触发） | ❌ 不可行 |

**倾向**：A — 扫码触发的检测结果展示在**设备侧 UI**（NetworkWatcher::UpdateNetworkStatusUi 先例 + wordings 文案体系，全流程逐层呈现、轻量形态）；上位机触发经协议 req/resp 返回；测速结果推送含弱网等级。**建议 cmd_id**：`NetworkDiagnosis = 0x15`（req/resp 低号段顺延，0x14 HostResultAck 之后）、`SpeedTestNotify = 0xA7`（module_push 段空缺号，0xA6 与 0xA8 之间，避开已下线的 0xB4）——建议值，评审/Plan 确认后写入 commands.yaml。**SpeedTestNotify 老 SDK 兼容已收敛（MR 评审）**：沿用 0xB3 范式（老 SDK 未识别 → pos_link 层返回 reply_msg_code=131，模组降级不再推送）。

**推送策略与触发载体（2026-08-28 定稿/变更）**：

- **推送策略**：非每次测速都推送，**按弱网等级变化触发**——等级恶化（进入弱网/断网）即推、恢复（回升）推一次、平稳等级不推（数据仅走上报，避免 30min 周期持续打扰）；**异常推送友好化**：payload 附用户可读提示与建议动作（「网络质量较差，识别可能变慢」/「网络已恢复」），上位机可直接展示
- **触发载体变更**：网络诊断入口**不做 Demo 独立按钮**，集成到 **SDK 的 Debug 页面**（SDK 调试能力组成部分，供外部 Demo/客户应用直接调用；SDK 无 Debug 页机制则随本需求一并提供）；Debug 页全流程可视化——四层逐层状态（检测中/✓/✗/跳过）+ 失败原因与诊断码 + 中间数据（IP/网关/DNS/路由优先级/Ping 时延）+ 测速结果（对齐 intake 2026-08-28 可视化需求）

### 决策点 7：测速结果上报通道

| 方案 | 优点 | 缺点 | 结论 |
|------|------|------|------|
| A. 参照 `metrics_resource_reporter` 模式新增 network metrics 上报（service/report 体系） | 复用成熟上报框架（report_item_base/opentelemetry_entity）；资源指标上报先例（device_resource_metrics_builder）直接可参照 | 需新增 reporter + builder | ✅ 推荐 |
| B. IoT 属性上报（property） | 属性可云端查询 | 属性适合状态不适合时序指标；测速是周期采样事件 | ❌ |
| C. MQTT 事件 | 实时 | MQTT 通道当前定位业务消息，指标上报走 report 体系是既有惯例 | ❌ |

**倾向**：A — 上报内容：时间戳 / 带宽 / 延迟 / 弱网等级 / 网络类型（wifi/ethernet/cell）/ 触发原因（周期/网络切换/手动）。

### 决策点 8：测速探测源

| 方案 | 优点 | 缺点 | 结论 |
|------|------|------|------|
| A. 服务端在现有 API 域名下放置静态测速文件（~100KB，如 /speedtest.bin），URL **终端内置**（探测文件为本需求新放置，路径随需求约定固定）；**文件就绪前降级为 health endpoint 延迟探测**（仅 RTT 维度） | 复用现有域名与证书链路；服务端配合成本极小（放一个文件）；降级路径不阻塞开发；无配置依赖，终端闭环 | 需服务端排期放置文件；URL 变更需发版（探测文件位置约定固定，变更概率低；后续如需云端可配为非破坏性增量） | ✅ 已定稿 |
| B. 复用现有业务接口/静态资源直接测 | 零协调 | 资源大小/响应行为不受控（可能重定向/动态内容），测速失真 | ❌ |
| C. 自建测速域名/CDN | 可控性最强 | 成本与协调最重，超出「轻量」定位 | ❌ |

### 决策点 9：弱网阈值校准策略

| 方案 | 优点 | 缺点 | 结论 |
|------|------|------|------|
| A. 默认值终端内置上线 + **上线后以 NetworkMetricsReporter 上报数据驱动首次校准**（满 1 个月复盘） | 不阻塞上线；用本需求自产生的全量网络质量数据校准（代表性最强）；实现最简（无配置依赖） | 首月阈值可能偏松/偏严；校准后若需调整走版本迭代（校准价值仍在：确认下一版是否调整阈值） | ✅ 已定稿 |
| B. 实施阶段先离线分析 palmapp_*.xlog 校准 | 上线即"准" | 业务日志无系统性带宽采样，校准价值有限 | ❌ |
| C. 保守高阈值上线（宁严勿松） | 漏报少 | 误报多，打扰现场 | ❌ |

## 关键技术决策

> 2026-08-26 负责人授权评估定稿（依据见各决策点方案对比），随 design 评审最终确认；供 `/spec-plan` 直接复用。

- **决策 1**（O2 复用方式）：✅ **移植检测逻辑 + O4 协程风格适配** — O2/O4 共享 facility 设计（system_interface / Threads 同名），检测核心为同步系统调用，适配仅在外壳；O2 四情况错误分类逻辑保真（重写有回归风险）
- **决策 2**（模块归属）：✅ **`facility/network/diagnosis/`** — 纯功能无生命周期符合 facility 定位，与 NetworkInfo 同域内聚；API 协程 + 回调双形态
- **决策 3**（诊断码归属）：✅ **独立体系 + JSON 透传，不并入 PalmIotErrorCode** — 沿用 O2 10xxx 原值；诊断码语义 ≠ 业务错误码语义；与 137332527「server_code 独立通道」同构；归属说明随本需求运维手册（不关联《O4 错误码规范》，已确认）。**定义文件并入现有 `unified_errcode_iot.h`（同文件独立 enum，不新建 unified_network_errcode.h——2026-08-28 负责人决策）**
- **决策 4**（测速选型）：✅ **HTTP 小文件下载为主（3~5s 窗口，<100KB/次）+ 业务 RTT 异常辅助触发** — 掌图上传带宽敏感；流量开销可忽略；license 文件作探测源的可行性待实测大小确认（倾向维持专用文件，2026-08-28）
- **决策 5**（弱网标准）：✅ **四级模型**（良好 ≥2Mbps 且 ≤200ms / 一般 1~2Mbps 或 200~500ms / 弱网 <1Mbps 或 >500ms 或连续 2 次失败 / 断网=检测失败），**阈值终端内置（终端闭环，无 IoT 配置依赖）**；本期动作=分级标记+上报+通知，不做自动降级；海外区域按上报数据分别复盘、可单独调整阈值（2026-08-28）
- **决策 6**（上位机通道）：✅ **新增 2 条 pos_link 指令**（NetworkDiagnosis req/resp + SpeedTestNotify push）；扫码触发结果走设备 UI 轻量提示；**推送按弱网等级变化触发（恶化即推/恢复一推/平稳不推）+ 异常推送友好文案**；**诊断入口集成 SDK Debug 页（供外部 Demo 调用，不做 Demo 独立按钮）+ 全流程可视化**（2026-08-28）
- **决策 7**（上报通道）：✅ **report 体系新增 NetworkMetricsReporter**（参照 metrics_resource_reporter 先例）
- **决策 8**（测速探测源）：✅ **服务端 API 域名静态文件（~100KB）+ URL 终端内置**（探测文件路径随需求约定固定）；文件就绪前降级 health endpoint 延迟探测（仅 RTT/弱网延迟维度），带宽维度待就绪启用——不阻塞开发
- **决策 9**（阈值校准）：✅ **默认值终端内置上线**；上线后以 NetworkMetricsReporter 上报数据驱动首次校准（满 1 个月复盘），校准后需调整走版本迭代；替代离线日志校准

## 跨系统 / 跨仓库影响

| 仓库 / 系统 | 是否改动 | 影响说明 |
|------------|---------|---------|
| `palm_app_linux`（模组） | ✅ | 新增 diagnosis 功能模块、扫码挂载、协议 handler、上报 reporter、UI 文案 |
| `palm_manager`（PDM SDK） | ✅ | 新增 2 条指令的 SDK 接口 + Demo 按钮 UI |
| `pos_link`（协议层） | ✅ | commands.yaml 新增指令定义 + 双端生成 |
| 服务端 | ❌（仅感知） | metrics 上报落既有上报通道；**测速探测源已决策**（决策 8）：API 域名下放置 ~100KB 静态测速文件（唯一外部协调项，降级路径不阻塞） |
| 云端配置 | ❌ | **已简化为终端闭环**（弱网阈值与测速 URL 均终端内置，无 IoT 配置依赖）；后续如需云端可配为非破坏性增量 |

## 数据结构 / 接口影响（高层）

```
// 高层示意，非最终定义
- facility/network/diagnosis/：
  - DetectionReport{ level, error_code(10xxx), error_message, details<map>, ToJsonString() }（O2 结构保真）
  - SpeedTestReport{ bandwidth_kbps（降级模式为 -1 未测）, rtt_ms, weak_level, network_type, trigger_reason, timestamp, mode(full/degraded) }
  - API：DetectNetwork() / DetectNetworkAsync(cb) / SpeedTest() / StartAutoSpeedTest() / StopAutoSpeedTest()
- pos_link 新指令：NetworkDiagnosis req/resp（resp 带 DetectionReport JSON）+ SpeedTestNotify push（SpeedTestReport JSON）
- service/report：NetworkMetricsReporter（参照 metrics_resource_reporter）
- 弱网阈值与测速 URL：终端内置常量（终端闭环，无 IoT 配置依赖）
- 交付物：《O4 网络问题现场排查手册》（面向客户运维）
```

## 建议的 spec 拆分（核心产出）

> **建议单 spec**：`specs/v2.0.0/137345399-o4-network-diagnostics.md`
>
> **理由**：改动集中在 palm_app_linux 单仓库为主（SDK 侧仅协议接口+Demo 按钮）；三块功能（检测/测速/文档）共享 diagnosis 模块底座，单 spec 单分支可控。
>
> **FR 分组预划**：
>
> | 分组 | 内容 | 输入 |
> |------|------|------|
> | FR-1 | 网络检测模块（O2 移植 + 协程适配 + 简易 API + 诊断码独立体系） | 决策 1/2/3 |
> | FR-2 | 网络测速模块（轻量实现 + 弱网分级 + 周期/事件触发 + 探测源降级策略） | 决策 4/5/8 |
> | FR-3 | 触发与通知通道（扫码挂载 + 设备 UI + 上位机 2 指令 + Demo 按钮） | 决策 6 |
> | FR-4 | 测速上报（NetworkMetricsReporter；阈值与 URL 终端内置） | 决策 5/7/8 |
> | FR-5 | 《O4 网络问题现场排查手册》运维文档：检测入口（扫码/Demo 按钮）→ 诊断码对照表 → 弱网标准与处置指引 → 常见问题对照表 → 日志/上报数据获取方式 | 全部决策 |

## 风险与未决问题

| 项目 | 描述 | 状态 |
|------|------|------|
| 测速探测源 | **已决策**：服务端在现有 API 域名下提供静态测速文件（~100KB），URL **终端内置**；**文件就绪前降级为 health endpoint 延迟探测**（仅 RTT 维度），带宽维度待就绪启用——不阻塞开发；服务端协调事项仅「放置一个静态文件」 | 已决策 — 待服务端执行 |
| O2 ServiceStrategy HTTP 依赖 | O2 network/http_request 模块需替换为 O4 等价物（或一并移植）——移植工作量评估 | open — Plan 阶段细化 |
| 带宽占用顾虑 | 30min 周期测速的流量开销（已定：采样窗口 3~5s / <100KB）对商户网络的影响 | 已决策 — 随决策 4/5 定稿 |
| 扫码触发展示闭环 | 扫码触发检测后结果在设备 UI 展示的交互形态 | 已决策 — 轻量提示（Toast/临时面板）显示摘要，详情进日志，spec 阶段可调 |
| 弱网阈值合理性 | **已决策**：默认值（1Mbps/500ms）终端内置上线；上线后以 NetworkMetricsReporter 上报数据驱动首次校准（上线满 1 个月复盘），需调整走版本迭代——替代原「实施阶段日志校准」方案（业务日志无系统性带宽采样，本需求自产生数据才是正确校准源） | 已决策 — 上线后数据驱动校准 |
| 双端协议生成 | 新增 2 指令的 commands.yaml 定义与生成链路（137332527 已验证机制，风险低） | 低 |
| **检测耗时 vs 协议超时** | NetworkDiagnosis 同步 req/resp 下，检测耗时（<10s，断网各层超时叠加可能更长）可能超过协议请求超时致 resp 丢失 | open — Plan 三选一：分层超时预算收紧 / 指令独立超时 / 受理+推送异步化（改变指令形态） |
| 业务 RTT 异常触发挂接点 | 决策 4 辅助触发需业务侧调用诊断 API（如识别失败处理处），挂接位置未定 | open — Plan 阶段定（业务→诊断为正向依赖，不破坏解耦） |
| 测速探测源：license 文件复用 | 激活 license 文件作测速载荷取决于实测大小（≥50KB 且 URL 稳定可作降级候选；过小采样失真）——倾向维持 ~100KB 专用文件 | open — Plan 实测确认（2026-08-28） |
| 上报协议后台对齐 | network metrics 沿用既有 report 格式，终端无新协议；后台接收/入库/展示是否需适配新 metrics 类型需服务端确认（不阻塞终端开发） | open — 服务端协调（继测速文件后第二项，2026-08-28） |

## 评审记录

| 日期 | 评审人 | 评审结论 | 关键意见 / 调整 |
|------|--------|---------|----------------|
| —    | —      | 待评审   | —              |

## 修订记录

| 日期 | 修订点 | 修订人 |
|------|--------|--------|
| 2026-08-26 | 初始版本（基于 intake + O2/O4 双侧代码侦察起草） | chongyyang |
| 2026-08-26 | 7 个决策点经负责人授权评估定稿（关键技术决策段落落定，依据见方案对比）；风险表 2 项同步已决策 | chongyyang |
| 2026-08-26 | 结合 intake 完善：新增「需求场景→检测能力映射」（intake 1.3 三场景与四层检测对应）与「方案速览」；决策点 8/9 补方案对比与关键技术决策；SpeedTestReport 补降级模式字段；FR-2/4/5 细化（降级策略/云端可配/运维文档内容框架）；C.1 补业务网络错误码（20xxx）与诊断码（10xxx）双体系关系说明 | chongyyang |
| 2026-08-26 | 「终端闭环」简化：弱网阈值与测速 URL 改终端内置（去除 IoT 配置依赖）；spec 澄清定稿回填：运维手册落盘 `paymax_device/docs/`、诊断码归属说明随运维手册（不关联《O4 错误码规范》） | chongyyang |
| 2026-08-26 | 第 2 轮评审修正：清除终端闭环残留（风险表探测源行与决策点 4 方案 A 的「可配 URL」表述）；补方案级风险「检测耗时 vs 协议超时」与「RTT 异常触发挂接点」（决策点 6 缺点列 + 风险表，与 spec 对齐）；负责人补充 M3/M4 网络检测现状参照（结构化并入现状分析）；场景映射表定位为「通用排查框架」（典型示例非穷举，见 D 节通用性原理） | chongyyang |
| 2026-08-28 | **intake 增量吸收 + 负责人 8 点补充**：决策 3 补定义文件并入现有码表（不新建 unified_network_errcode.h）；决策 5 补海外阈值适用性；决策 6 补推送策略（等级变化触发 + 异常友好文案）与触发载体变更（SDK Debug 页集成、全流程可视化）；决策 4/8 补 license 文件作探测源评估（待实测）；现状分析补 M3/M4 实测经验引用；风险表 +2（license 评估 / 上报协议后台对齐）；关键技术决策沉淀同步 | chongyyang |