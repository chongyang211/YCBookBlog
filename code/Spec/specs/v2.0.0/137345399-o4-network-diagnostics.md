# O4 网络诊断措施（网络检测 / 测速 / 弱网运维）

> **Story ID**: 137345399
> **Status**: draft
> **Author**: chongyyang
> **Created**: 2026-08-26
> **Updated**: 2026-08-28
> **Sibling Specs**: 无（单 spec 覆盖 palm_app_linux / palm_manager / pos_link 三仓改动 + iotservice_linux 只读取材仓，不按仓拆分——参照 137332527 单 spec 多仓先例）
> **Branch**: `feature/137345399-o4-network-diagnostics`
> **关联 Design**: `designs/v2.0.0/137345399-o4-network-diagnostics-design.md`

## 背景

O4 缺乏网络问题的现场定位手段：设备网络问题（网线脱落/网卡异常）、分网问题（网段隔离/网关路由异常）、网络正常但路由策略致客户环境不通等场景无法快速排查；无网速/弱网度量与上报。

2026-08-26 代码侦察结论（见 design 现状分析，带文件:行号实证）：

1. **O2 可复用**：四层检测（物理/本地/连接/服务，策略模式 + Manager + 统一诊断码 10xxx）；O2/O4 共享 facility 设计（`system_interface`/`Threads` 同名），移植适配点仅协程风格与 ServiceStrategy HTTP 依赖
2. **O4 挂载点齐备**：网络事件源（`NetworkWatcher` 4 类信号）、扫码触发点（`QRCodeOperationHandler` ActionCode 体系）、上报先例（`metrics_resource_reporter`）、协议扩展机制（commands.yaml，137332527 已验证）
3. **衔接约束**：诊断码 10xxx 与 PalmIotErrorCode 10100 同属 1xxxx 段——决策为独立体系不并入（JSON 透传隔离）

## 目标

- 网络检测：四层检测快速定位网络断点层级（第一失败层即断点），**通用排查框架覆盖各种网络问题**（任何故障最终表现为某一层失败；intake 1.3 三类场景为典型示例）
- 网络测速：轻量测速 + 弱网四级分级（终端内置阈值），网络切换 + 周期触发，上报云端 + 通知上位机
- 运维文档：面向客户的现场排查手册

## 非目标

- 弱网自动降级动作（降画质/断流等业务策略）——本期仅分级标记 + 上报 + 通知
- 阈值与测速 URL 云端可配（**已决策终端闭环**：编译期常量；后续如需云端可配为非破坏性增量）
- 诊断码并入 PalmIotErrorCode 错误码表（独立体系；归属说明随本需求运维手册，不关联《O4 错误码规范》）
- 服务端改造（仅协调放置 ~100KB 静态测速文件）
- O2 侧改动（只读复用）

## 用户故事

- 作为现场运维人员，我希望扫一下码（或在 SDK Debug 页点一下网络诊断）就能看到四层检测全流程结果（没插网线 / 没网关 / 网关不通 / 服务不通逐层呈现），以便快速处置
- 作为现场运维人员，我希望设备能自动测速并告诉我弱网等级，以便判断"能连但慢"的问题
- 作为后台研发，我希望测速与弱网数据自动上报，以便远程定位与阈值校准
- 作为收银端开发者，我希望通过 SDK 简易接口触发网络检测并拿到结构化结果，以便集成排查能力
- 作为客户运维，我希望有一份排查手册，对照诊断码与弱网标准就能处置常见网络问题

## 功能需求

### FR-1 组：网络检测模块（决策 1/2/3）

#### FR-1.1 移植 O2 四层检测

移植 O2 `strategy/` 四层检测到 `facility/network/diagnosis/detection/`：物理层（接口枚举/UP/载波/IP）、本地层（网关/路由）、连接层（网关 ping）、服务层（health endpoint 探测 + HTTP 回退）+ Manager（按序执行、遇失败停止）。**检测算法与错误分类逻辑保真**（O2 `GenerateDetailedErrorMessage` 四情况分类），诊断码沿用 O2 10xxx 原值（10011~10044）。诊断码定义**不新建独立文件**（2026-08-28 负责人决策，不随移植保留 O2 的 `unified_network_errcode.h`）——10xxx 诊断码枚举**并入现有码表文件 `palm_app_linux/src/comm/constant/unified_errcode_iot.h`**：同文件内新增独立 enum（如 `PalmNetworkDiagnosisCode` 10xxx 段），与 `PalmIotErrorCode` 业务码表**物理同文件、逻辑分离**（独立 enum + 段注释「10xxx 为网络诊断码保留，不用于业务错误码」防误占），免新增文件。

**四层检测能力矩阵**（检测维度与实现手段，对齐 O2 实现与 O4 网络形态 eth / wifi / cell）：

| 检测类型 | 检测内容 | 底层原理 | O2/O4 实现手段（实证） |
|---|---|---|---|
| **物理层** | 网络接口存在 / 已激活（UP，含 eth / wlan / cell 接口形态）/ 载波信号 / IP 已获取 | 系统接口与链路层状态查询 | `getifaddrs` 枚举（跳 loopback）+ `/sys/class/net/<iface>/{flags,carrier}` + 接口 IP 读取 |
| **本地层** | 网关已配置 / 路由存在 | 内核网络配置读取 | 网关/路由查找（`/proc/net/route` 系） |
| **连通层** | 网关可达（本地网段出口通；公网连通由服务层覆盖） | ICMP Echo 探测 | ping 网关（`ConnectivityStrategy::TestGatewayConnectivity`） |
| **服务层** | 服务域名解析 → 建连 → 服务健康（覆盖 DNS 异常 / 防火墙拦截 / TLS 失败等服务侧全链路） | DNS 解析 + TCP/TLS 建连 + HTTP 探测（逐段计时，衔接 FR-2.1 RTT 口径） | health endpoint 探测 + 基础 HTTP 连接回退（`ServiceStrategy`） |

**M3/M4 实测经验吸收**（intake 2026-08-28 补充，检测规则硬约束——七条实测问题中检测相关的五条落点）：

| # | 实测经验（问题与根因） | 检测规则落点 |
|---|---|---|
| 1 | M3 内核 `carrier` 因给模组分网**恒为 1**，无法表征网线插拔 | 网络可用性判据**以 Ping 通网关为准**；**禁止 Ping 公网**（百度等）做判据——设备可能部署在私有化环境，Ping 不通公网 ≠ 不可用；carrier 仅作物理层参考信号，不作唯一依据 |
| 2 | 无 DHCP 路由器：static 模式下「无 IP ≠ 未连路由器」（连上但永远无 IP） | 物理层「UP 无 IP」分类需区分输出「**已连路由器但无 IP**（疑似路由器无 DHCP 或需静态配置）」语义，不笼统报「未连接」 |
| 3 | 双网口出口由路由表 metric 决定：实测「WiFi 可用但以太网网关优先级更高 → 走以太网 → 断网」 | 本地层/连接层 details 输出**路由表与各网关 metric 优先级**（`ip route show` 口径）；检测到「活动路由指向不可用网口」作为**独立告警项**输出 |
| 4 | DNS 配置错误（默认 DNS=网关，用户可指定）→ 后台域名解析失败 → 连不上后台 | 服务层 details 输出**当前生效 DNS**；「DNS 可达但域名解析失败」为独立失败项 |
| 5~7 | 静态配置语义（静态 WiFi 参数按 SSID 隔离 / 以太网>WiFi 优先级+无感切换 / **静态以太网配错不切换只提醒改参数**） | 检测结论建议文案区分「**静态配置错误**（提示修改参数，不建议切换网络——静态语义即明确要求使用该配置）」与「链路故障（可切换备用网络）」；本地层输出当前配置模式（DHCP/静态） |



#### FR-1.2 协程风格适配

`ExecuteAsync` 由 O2 callback 风格（`std::function` + `Threads::IotThread()->Post`）适配为 O4 asio 协程风格（`awaitable` / `CoSpawn`，参照 `network_monitor_service.cpp:21-31` 先例）；检测实现主体（同步系统调用）零改动。适配另含**超时常量参数化**：O2 `ServiceStrategy` 硬编码 `SetTimeout(5000ms)`（service_strategy.cpp:94，MR 评审核实）——参数化后供 FR-3.2 超时预算统一编排，「适配」不止换协程外壳（MR 行内意见）。

#### FR-1.3 简易 API（与业务解耦）

```
NetworkDiagnosis::DetectNetwork()            -> awaitable<DetectionReport>   // 协程形态（O4 主流风格）
NetworkDiagnosis::DetectNetworkAsync(cb)     // 回调形态（非协程上下文）
// DetectionReport{ level, error_code(10xxx), error_message, details<map>, ToJsonString() }（O2 结构保真）
```

**调用示例**（高层示意，最终签名 Plan 阶段定）：

```cpp
// 场景 1：协程调用（服务层/后台协程中，O4 主流风格）
auto report = co_await NetworkDiagnosis::Instance()->DetectNetwork();
if (!report.success) {
  // 第一失败层即断点：level = 失败层级、error_code = 10xxx 诊断码、error_message = 可读错误
  LOG_E("network broken: level={}, code={}, msg={}", report.level, report.error_code, report.error_message);
  // details 含定位明细：物理层各接口 status/ip/carrier、本地层网关路由等
}

// 场景 2：回调调用（非协程上下文，如扫码触发处）
NetworkDiagnosis::Instance()->DetectNetworkAsync(
    [](const DetectionReport& r) { ShowDiagnosisToast(r); });  // 后台线程执行，不阻塞调用方
```

模块为纯功能（facility 层），不依赖 biz/service 业务逻辑；检测耗时 2~10s，异步执行不阻塞刷掌业务。

#### FR-1.4 ServiceStrategy HTTP 依赖替换

O2 `network/network.h` + `network/http_request.h` 依赖替换为 O4 等价 HTTP 客户端（具体替换物 Plan 阶段细化），health 探测语义不变。

### FR-2 组：网络测速模块（决策 4/5/8）

#### FR-2.1 HTTP 小文件下载测速

从终端内置 URL（服务端 API 域名下 ~100KB 静态文件，如 `/speedtest.bin`）下载测速：采样窗口 3~5s，计算带宽（kbps）与 RTT（ms）；单次流量 <100KB。RTT 测量口径（连接建立耗时 vs 首字节 TTFB）Plan 阶段定并在运维手册中说明（弱网延迟维度判定的可解释性）。

**探测源补充评估（2026-08-28）**：是否可复用激活流程下载的 license 文件作为测速载荷（省去服务端放置新文件）——取决于 license 文件**实际大小（大概900多kb）**：文件过小（如 <50KB）在 3~5s 采样窗口内数据量不足，带宽采样失真；且业务资产复用存在 URL 稳定性与语义耦合顾虑。**结论倾向维持 ~100KB 专用测速文件**（大小可控、可无限重复拉取、语义独立）；license 文件实测 ≥50KB 且 URL 稳定可复用时，可作为降级探测源候选——Plan 阶段确认（见风险表）。

#### FR-2.2 弱网四级分级（终端内置阈值）

| 等级 | 阈值（编译期常量） | 业务含义 |
|---|---|---|
| 良好 | 带宽 ≥ 2 Mbps 且延迟 ≤ 200ms | 掌图上传与交互流畅 |
| 一般 | 1~2 Mbps 或延迟 200~500ms | 可用，识别上传略慢 |
| **弱网** | **<1 Mbps 或 >500ms 或连续 2 次测速失败** | 识别上传明显变慢/超时风险 |
| 断网 | 检测失败（连通层/服务层不过） | 无法联网 |

判定动作：分级标记 + 上报（FR-4）+ 通知上位机（FR-3.3），不做自动降级。

**海外适用性（2026-08-28）**：默认阈值以国内主流商户网络为基准；海外网络基础较差地区（很多国家网络非常差）预期弱网占比偏高。缓解设计：①弱网分级为**提示性标记，不阻断业务**（误报不影响刷掌可用性）；②推送按**等级变化触发**（FR-3.3——平稳弱网不重复推送，避免持续打扰）；③海外阈值校准纳入决策 9 机制：按区域上报数据**分别复盘**，海外区域可单独调整阈值走版本迭代。

#### FR-2.3 触发机制

- **网络切换事件**：挂 `NetworkWatcher` 网络信号（与 `NetworkMonitorService` 同一信号链）
- **周期触发**：默认 30min
- **辅助触发**：业务 RTT 异常（如识别请求持续超时）触发即时测速

#### FR-2.4 探测源降级策略

测速文件未就绪（下载失败/404）时降级为 health endpoint 延迟探测：仅测 RTT（弱网判定的延迟维度），`bandwidth_kbps = -1`（未测）、`mode = degraded`；文件就绪后自动恢复完整模式。降级与恢复对调用方透明（SpeedTestReport 结构不变）。

#### FR-2.5 简易 API

```
NetworkDiagnosis::SpeedTest()                -> awaitable<SpeedTestReport>   // 单次主动测速
NetworkDiagnosis::StartAutoSpeedTest()       // 启用自动测速（网络切换 + 30min 周期 + RTT 异常触发）
NetworkDiagnosis::StopAutoSpeedTest()
NetworkDiagnosis::SetSpeedTestCallback(cb)   // 订阅自动测速结果（本地消费，可选）
// SpeedTestReport{ bandwidth_kbps(-1=降级未测), rtt_ms, weak_level, network_type, trigger_reason, timestamp, mode(full/degraded) }
```

**调用示例**（高层示意）：

```cpp
// 设备启动时启用自动测速（上报 FR-4 与推送 FR-3.3 由框架自动完成，业务无需轮询）
NetworkDiagnosis::Instance()->StartAutoSpeedTest();

// 需要本地消费测速结果时订阅（如设备 UI 弱网提示）
NetworkDiagnosis::Instance()->SetSpeedTestCallback([](const SpeedTestReport& r) {
  if (r.weak_level == WeakLevel::kWeak) { ShowWeakNetworkToast(); }
});

// 业务侧按需单次测速（如排查场景）
auto speed = co_await NetworkDiagnosis::Instance()->SpeedTest();
```

### FR-3 组：触发与通知通道（决策 6）

#### FR-3.1 扫码触发 + 设备 UI

`QRCodeOperationHandler` 挂载网络检测（扩展 ActionCode 操作码或挂接处理链，复用既有防抖/并发保护）；检测启动即提示「网络检测中」（检测最长 ~10s，需过渡态），完成后设备侧展示**全流程检测结果**（2026-08-28 对齐 intake 可视化需求）：按执行顺序逐层呈现「通过 ✓ / 失败 ✗」状态 + 失败层诊断码与错误信息——设备侧为轻量形态（Toast/临时面板呈现摘要 = 第一失败层 + 错误信息，参照 `NetworkWatcher::UpdateNetworkStatusUi` + wordings 文案先例；逐层详情与中间数据完整形态见 FR-3.4 SDK Debug 页，设备侧具体展示深度 Plan 定），详情进日志。

#### FR-3.2 上位机指令：NetworkDiagnosis（req/resp）

pos_link 新增指令（commands.yaml + 双端生成，**建议 cmd_id = 0x15**，req/resp 低号段顺延——建议值，评审/Plan 确认）：上位机（Demo 按钮）发 req → 模组执行四层检测 → resp 返回 `DetectionReport` JSON（诊断码独立字段承载，不混入业务 error_code）。**注意**：检测耗时（目标 <10s，弱网/断网各层超时叠加可能更长）与协议 req/resp 超时的匹配需 Plan 阶段解决（分层超时预算 / 该指令独立超时配置 / 受理+推送异步化，三选一）——**评审倾向优先评估异步化**（req 受理 + push 结果，避免断网场景 resp 丢失；会改变指令形态并影响 SDK Listener 设计）——见风险表。

#### FR-3.3 上位机指令：SpeedTestNotify（push）

pos_link 新增推送指令（**建议 cmd_id = 0xA7**，module_push 段空缺号——建议值，评审/Plan 确认，避开已下线的 0xB4）：模组测速完成后推送 `SpeedTestReport` JSON（含 weak_level）给上位机。

**推送策略（2026-08-28 定稿：非每次测速都推送，按等级变化触发 + 异常友好）**：

- **等级恶化即推**：进入弱网 / 断网 → 推送
- **恢复即推一次**：从弱网/断网回升（→ 一般/良好）→ 推送恢复通知
- **平稳不推**：连续同等级（持续良好/一般，或持续弱网无变化）不推送——数据仅走 FR-4 上报，避免 30min 周期测速的持续打扰
- **异常推送友好化**：弱网/断网推送 payload 除 SpeedTestReport JSON 外附**用户可读提示与建议动作**（如「网络质量较差，识别可能变慢，请检查网络/路由器」），上位机可直接展示；恢复通知同理（如「网络已恢复」）
- 主动查询最近结果走 FR-3.2 req/resp（SDK 侧可缓存最近一次推送结果供查询）

**老 SDK 兼容已决策（MR 评审收敛）**：沿用 0xB3 既有范式——老 SDK 未识别推送时 pos_link 层返回 `reply_msg_code=131`，模组据此降级（不再推送），AC-8 验收。

#### FR-3.4 SDK Debug 页集成（2026-08-28 变更：不做 Demo 独立按钮）

palm_manager（PDM SDK）新增检测/测速结果监听接口（Listener 形态，具体签名 Plan 阶段定）+ **网络诊断入口集成到 SDK 的 Debug 页面**（不做成 Demo 独立按钮——SDK Debug 页作为 SDK 调试能力的一部分，**供外部 Demo/客户应用直接调用集成**；若 SDK 尚无通用 Debug 页机制，随本需求一并提供）。

Debug 页展示**检测全流程可视化**（对齐 intake 2026-08-28 可视化需求）：

- **检测流程**：四层按执行顺序逐层呈现「检测中 / 通过 ✓ / 失败 ✗ / 跳过（上游失败）」+ 失败层诊断码与错误信息 + 关键中间数据（当前 IP / 网关 / DNS / 路由优先级 / Ping 网关时延）
- **测速结果**：最近测速时间 / 带宽（上下行）/ RTT / 弱网等级与判定依据（阈值对照）
- **展示分层**：解析后的分层结论 + 用户可读提示为主（运维可读），原始 JSON 折叠供调试（研发定位）

Demo（官方示例）通过集成 SDK Debug 页获得该能力，自身不再单独实现诊断按钮与展示。

### FR-4 组：测速上报（决策 7）

#### FR-4.1 NetworkMetricsReporter

参照 `metrics_resource_reporter` 模式新增 network metrics 上报（service/report 体系）：上报字段 = 时间戳 / 带宽 / 延迟 / 弱网等级 / 网络类型（wifi/ethernet/cell）/ 触发原因（周期/网络切换/手动/RTT 异常）/ mode；走既有上报通道。

**协议对齐与未来演进（2026-08-28）**：

- **上报协议对齐**：上报沿用既有 report 体系格式（`metrics_resource_reporter` / opentelemetry_entity 先例），终端侧无需新协议；**后台是否需为新 metrics 类型（network）做接收/入库/展示适配需服务端确认**——新增协调项（见风险表），不阻塞终端开发（上报链路先落，后台消费能力跟进）
- **未来演进兼容（设备诊断增加网速项）**：若产品后续在设备诊断（云端诊断视图）增加网速项，当前时序上报数据天然可支撑（按设备取最新样本/趋势曲线）；上报字段设计保持语义清晰、结构可扩展（新增字段为非破坏增量）；如需「实时状态快照」可后续补属性上报通道，不动本期结构——**当前实现可无缝兼容该演进**

#### FR-4.2 终端内置常量

弱网阈值与测速 URL 为编译期常量（终端闭环，无 IoT 配置依赖）。

### FR-5 组：运维文档（全部决策）

#### FR-5.1 《O4 网络问题现场排查手册》

面向客户的现场排查手册，内容框架：

1. **检测入口**：扫码操作说明 + Demo 按钮说明（何时用哪个）
2. **诊断码对照表**：10xxx 四层诊断码 → 中文含义 → 现场处置建议（如 10012 网络接口未激活 → 检查网线/网卡）
3. **弱网标准与处置指引**：四级标准 + 弱网时的现场检查项（路由器负载/距离/带宽占用）
4. **常见问题对照表**：典型场景（含 intake 1.3 三类示例，扩展 DNS 异常 / 代理配置错误 / IP 冲突等未列举场景）→ 典型诊断码 → 处置路径
5. **日志与上报数据获取方式**：如何导出日志、在哪查上报数据
6. 说明诊断码（10xxx）与业务错误码（20xxx 等）双体系关系

落盘 **`paymax_device/docs/`**（已确认 2026-08-26）；文档同时作为 FR-1~4 验收的现场可用性证明。

## 非功能需求

- **兼容性**：老 SDK + 新模组——SpeedTestNotify 推送对老 SDK 的行为需安全（忽略或协议层拒绝，Plan 阶段验证）；老模组 + 新 SDK——新指令不被老模组响应，Demo 按钮需处理超时
- **性能**：检测/测速在后台线程执行，不阻塞刷掌业务主流程；单次测速流量 <100KB；检测全程耗时目标 <10s（四层含 ping 与 HTTP 探测）
- **可维护性**：诊断模块零业务依赖（可独立单测）；阈值/URL 常量集中定义

## 数据结构 / API / 接口影响

- 新增 `facility/network/diagnosis/`（detection/ + speedtest/）：DetectionReport、SpeedTestReport、NetworkDiagnosis API（FR-1.3/2.5）
- pos_link 新增 2 条指令（NetworkDiagnosis req/resp + SpeedTestNotify push）+ 双端生成代码
- palm_manager SDK：新增结果监听 Listener + **SDK Debug 页（网络诊断入口 + 全流程可视化展示，供外部 Demo/客户应用集成）**
- service/report：新增 NetworkMetricsReporter（+ builder）
- 设备 UI：网络诊断提示文案（wordings）
- 交付物：《O4 网络问题现场排查手册》

## 边界情况

| 场景 | 预期行为 |
|---|---|
| 测速文件未就绪（404/下载失败） | 降级模式：RTT-only，bandwidth=-1，mode=degraded；文件就绪自动恢复 |
| 检测过程中网络切换 | 当前检测完成返回快照结果；网络切换信号触发新一轮检测/测速 |
| 检测与刷掌业务并发 | 后台线程执行，互不阻塞；检测不独占网络（ping/HTTP 探测低流量） |
| 多网络接口（eth+wifi 并存） | 物理层 details 输出全部接口明细；检测以默认路由接口为准 |
| 老 SDK + 新模组（SpeedTestNotify） | 老 SDK 安全忽略或协议拒绝（不 crash），Plan 阶段验证 |
| 老 模组 + 新 SDK（NetworkDiagnosis req） | 老模组无响应，SDK 侧超时返回（Demo 提示不支持） |
| 离线模式（OfflineMode） | 检测照常执行（正是排查场景）；测速失败计入弱网判定样本 |
| 连续测速失败 | 连续 2 次失败 → 判定弱网（阈值表）；失败计数在**网络切换后重置**（新网络环境重新计样本） |

## 验收标准

- [ ] 正常网络下四层检测全通过（DetectionReport.success=true）；人为构造各层故障（拔网线/删路由/断服务）返回对应层诊断码且 Manager 在该层停止（AC-1 / FR-1.1）
- [ ] 模块无 biz/service 头文件依赖（include 检查），API 双形态（协程+回调）可用（AC-2 / FR-1.2/1.3）
- [ ] 三类触发各自闭环：扫码→设备 UI 全流程提示；SDK Debug 页触发→req/resp 返回 DetectionReport JSON 且 Debug 页逐层可视化展示（含中间数据）；自动→按等级变化策略推送到达 SDK（恶化/恢复推送、平稳不推）（AC-3 / FR-3）
- [ ] 限速环境（模拟 <1Mbps）测速判定弱网；正常网络判定良好/一般；分级符合阈值表（AC-4 / FR-2.2）
- [ ] 测速文件不可用时降级模式正确（bandwidth=-1 / mode=degraded / RTT 有值），文件恢复后自动回 full（AC-5 / FR-2.4）
- [ ] 网络切换后 10s 内触发一次测速；30min 周期测速按计划执行（AC-6 / FR-2.3）
- [ ] NetworkMetricsReporter 上报字段完整并落既有通道（云端可查）（AC-7 / FR-4.1）
- [ ] 检测/测速执行期间刷掌业务正常（并发无阻塞、无 crash）；老 SDK 收到 SpeedTestNotify 不 crash（AC-8）
- [ ] 《手册》覆盖**全部 10xxx 诊断码**（任意故障可对照定位）+ 弱网四级标准 + 典型场景排查路径（含 intake 1.3 三类及扩展场景），新人按手册可完成模拟故障定位（AC-9 / FR-5）
- [ ] SDK CHANGE 记录新指令与新 Listener（AC-10）

## 测试点

- **正常路径**：四层检测各层通过/失败矩阵；测速带宽值合理性（对照已知带宽）；三类触发链路端到端
- **边界条件**：探测源降级与恢复；检测中网络切换；多接口；离线模式；连续失败判定
- **并发**：检测/测速与刷掌并发（压测）；自动测速定时器与业务线程竞争
- **兼容性**：老 SDK 收推送；老模组无响应超时
- **单测**：四层 Strategy 各自可独立测试（检测算法保真性对照 O2）；弱网分级阈值边界值（0.99/1.0/1.99/2.0 Mbps，199/200/499/500ms）；降级模式字段

## 风险与未决问题

| 项目 | 描述 | 状态 |
|------|------|------|
| 测速探测源排期 | 服务端放置 ~100KB 静态文件（唯一外部协调项） | open — 降级策略不阻塞开发，文件就绪时间待协调 |
| O2 HTTP 依赖替换 | ServiceStrategy 的 network/http_request 替换物选型与工作量 | open — Plan 阶段细化 |
| SpeedTestNotify 对老 SDK 兼容 | **已决策（MR 评审收敛）**：沿用 0xB3 既有范式——老 SDK 未识别推送时 pos_link 层返回 `reply_msg_code=131`，模组据此降级（不再推送） | 已决策 — AC-8 验收 |
| 扫码 ActionCode 扩展方式 | 新操作码 vs 挂接处理链 | open — Plan 阶段定（复用防抖/并发保护） |
| SDK Listener 形态 | 新增监听器签名设计（检测结果 + 测速推送合一 or 分开） | open — Plan 阶段定 |
| 弱网阈值合理性 | 默认值需上线后数据校准（满 1 个月复盘，调整走版本迭代）。**采集口径（MR 评审补）**：每次测速为一个样本（含 mode=degraded 样本，单独统计），按弱网等级分布 + 误报/漏报评估复盘。**海外维度（2026-08-28）**：海外网络基础差地区弱网占比预期偏高，按区域分别复盘、海外可单独调整阈值（FR-2.2 海外适用性） | open — 上线后按区域校准 |
| **测速探测源：license 文件复用评估** | 激活 license 文件能否作测速载荷取决于实测大小：≥50KB 且 URL 稳定可作降级候选，过小则 3~5s 窗口采样失真——倾向维持 ~100KB 专用文件（FR-2.1 补充评估） | open — Plan 阶段实测确认后定 |
| **上报协议后台对齐** | network metrics 沿用既有 report 格式，终端无需新协议；**后台接收/入库/展示是否需适配新 metrics 类型需服务端确认**（不阻塞终端开发，上报链路先落） | open — 服务端协调（继测速文件之后第二项外部协调） |
| **检测耗时 vs 协议超时** | NetworkDiagnosis 同步 req/resp 模式下，检测耗时（<10s，断网各层超时叠加可能更长）可能超过协议请求超时导致 resp 丢失。**量化依据（MR 评审）**：O2 ServiceStrategy 硬编码 5000ms 探测超时，最坏路径 ≈ 连接层 ping 超时 + 服务层 5s ≈ 10s+（物理/本地层为毫秒级本地调用） | open — Plan 阶段三选一：分层超时预算收紧（含 O2 5000ms 常量参数化）/ 该指令独立超时配置 / req 受理 + 结果推送异步化，并给出量化预算表；**评审倾向优先评估异步化** |
| 业务 RTT 异常触发挂接点 | FR-2.3 辅助触发需业务侧调用诊断 API（识别失败处理处），具体挂接位置未定 | open — Plan 阶段定（业务→诊断为正向依赖，不破坏模块解耦） |

## 实施备注

- **执行顺序**：FR-1（检测模块，纯移植可先行）→ FR-2（测速，依赖 FR-1 的服务层探测）→ FR-3（通道，依赖 FR-1/2 的 API）→ FR-4（上报，可与 FR-3 并行）→ FR-5（文档，收口）
- **关键代码参考**：O2 `strategy/`（14 文件，**取材分支 iotservice_linux master**——commit 7603f78 引入、870bb63 迭代；develop 基线无此目录，本地实证 + MR 评审确认）；O4 `network_watcher.h`、`network_monitor_service.cpp`、`qrcode_operation_handler.cpp`、`metrics_resource_reporter.*`、`device_resource_metrics_builder.*`、`palm_report_item.h:126`、`commands.yaml` + `pos_transferer.cpp`
- **双端合并顺序**：模组（palm_app_linux）先行（检测/测速/指令 handler）→ SDK（palm_manager）对齐（指令接口 + Listener + Demo）→ pos_link 生成代码随模组
- **决策依赖**：无阻塞——9 项决策已定稿（含终端闭环简化），open 项均为 Plan 阶段细化或外部协调

## 修订记录

| 日期 | 修订点 | 关联 tasks 偏离记录 | 修订人 |
|------|--------|---------------------|--------|
| 2026-08-26 | 初始版本（基于 intake + design 定稿结论起草，9 决策全落定无 [待评审] 项） | — | chongyyang |
| 2026-08-26 | 澄清定稿：运维手册落盘 `paymax_device/docs/`；诊断码归属说明随运维手册（不关联《O4 错误码规范》）——FR-5.1 与非目标同步 | — | chongyyang |
| 2026-08-26 | 第 1 轮评审修正：新增风险「检测耗时 vs 协议超时」（FR-3.2 标注三选一解法）与「RTT 异常触发挂接点」；FR-2.1 补 RTT 测量口径说明；FR-3.1 补「检测中」过渡提示；FR-3.4 补 Demo 展示形态；连续失败补网络切换重置条件 | — | chongyyang |
| 2026-08-26 | 检测定位于**通用排查框架**（三场景为典型示例非穷举）；FR-1.1 补「四层检测能力矩阵」（整合负责人检测维度素材：工具列对齐 O2/O4 实现语境，连通层对齐 O2 的 ping 网关实现，服务层融入域名→建连→健康全链路） | — | chongyyang |
| 2026-08-26 | 初评总评（撤回前原始版）核对处置：4 条中 3 条已在前序处置（cmd_id/超时量化/采集口径），「协程措辞不符」批评已被订正版撤回；Sibling 说明补 iotservice_linux 只读取材仓 | — | chongyyang |
| 2026-08-26 | 行内评论收敛处置：① SpeedTestNotify 老 SDK 兼容 open → **已决策**（沿用 0xB3 范式：reply_msg_code=131 + 模组降级）；② FR-3.2/风险表补**评审倾向优先异步化**；③ FR-1.2 补超时常量参数化工作量（O2 SetTimeout(5000ms) @ service_strategy.cpp:94，MR 行内核实）；④ cmd_id 建议值补「避开已下线的 0xB4」备注 | — | chongyyang |
| 2026-08-26 | 第 2 轮复评处置：诊断码占位声明改写**全路径消歧**（`palm_app_linux/src/comm/constant/unified_errcode_iot.h` 为 O4 业务码表·占位登记目标，实证存在；与 O2 `unified_network_errcode.h` 诊断码文件是两个文件——复评「文件不存在」指控经 grep 核实不成立，属两仓同名/近名文件混淆） | — | chongyyang |
| 2026-08-28 | **intake 增量吸收 + 负责人 8 点补充**：① FR-1.1 诊断码**并入现有码表文件**（不新建 unified_network_errcode.h，同文件独立 enum）+ 新增「M3/M4 实测经验吸收」表（Ping 网关唯一判据禁公网/已连无 IP 语义/路由 metric 告警/DNS 输出/静态配错文案）；② FR-2.1 补 license 文件作测速载荷评估（待实测大小，倾向维持专用文件）；③ FR-2.2 补海外阈值适用性（按区域校准）；④ FR-3.1 设备侧展示全流程检测结果（对齐 intake 可视化）；⑤ FR-3.3 推送策略定稿（**等级变化触发**：恶化即推/恢复一推/平稳不推 + 异常推送友好文案）；⑥ FR-3.4 变更为 **SDK Debug 页集成**（不做 Demo 独立按钮，供外部调用）+ 全流程可视化展示；⑦ FR-4.1 补上报协议后台对齐（新 open 项）与未来设备诊断增加网速项的无缝兼容说明；风险表 +2 项、弱网阈值行补海外维度；用户故事/AC-3/数据结构同步 | — | chongyyang |