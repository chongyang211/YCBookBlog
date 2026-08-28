---
name: analyze-log
description: 分析智研项目 15692 (wecard-palm) 线上日志的通用技能，支持 **test-gz（默认）与 hk 两个环境**。当用户报告"某接口报错 / 某服务异常 / 某笔请求跑不通 / 想看某个 trace 的全链路 / 想查服务健康度"时使用。触发词：「查日志」「分析日志」「看看日志」「trace 链路」「排查 xxx 报错」「xxx 服务有没有异常」「zhiyan-log」「智研日志」「查香港日志」「hk 环境」。**前置依赖**：本 skill 依赖 `zhiyan-log` MCP 与太湖 token，AI 无法自动安装，首次使用前需引导用户按 https://knot.woa.com/mcp/detail/89 完成配置。skill 负责：① **执行任何查询前先探测 MCP 是否就绪**，未就绪时打印安装引导并暂停；② **环境识别**：用户没显式指定环境时默认 `test-gz`，只有用户明确说"香港/hk/hongkong"才切到 hk；③ **优先根据本会话上下文（create-zerus 环境 / spec plan / git 现状）锁定查询服务范围**，缩小到 1~5 个接入名而非全项目撒网；④ 用户提供 trace_id 时以 trace_id 为最强定位条件，查询范围仍按上下文缩小，查不到再逐级扩大（L1 上下文 → L2 上下游链路 → L3 全项目）；⑤ 把用户描述的现象映射到目标环境的接入名；⑥ 选择合适的分析模式（root cause / trace 链路 / 健康巡检）；⑦ 用 zhiyan-log MCP 组合查询与聚合；⑧ **查到日志后必须结合本仓库 `src/` 下的服务源码定位报错点、还原调用链、给出代码级根因，不能只停在日志字面**（详见 §源码关联根因分析）；⑨ 输出简明摘要到对话。
---

# 分析线上日志（Analyze Log）

`test-gz` 环境（智研项目 `wecard-palm / 15692`）线上日志的**通用分析技能**。所有查询都走 `zhiyan-log` MCP，本 skill 不落盘、不写脚本，只做**查询编排 + 结果聚合 + 源码关联根因 + 摘要输出**。

**核心工作流：日志查询 → 源码定位 → 调用链还原 → 代码级根因 → 摘要输出**。查到日志只是第一步，必须结合本仓库 `src/` 下的服务源码（Go 微服务 monorepo）才能给出真正有价值的分析。

## 何时使用

- 用户报告某个接口 / 服务 / 请求异常，需要查日志定位
- 用户给出一个 `trace_id`，想看完整链路
- 用户想对某服务/一批服务做健康度巡检（错误率、慢请求、异常码分布）
- 触发词：「查日志」「分析日志」「看看日志」「trace 链路」「排查 xxx 报错」「xxx 服务有没有异常」「智研日志」

## 何时**不**使用

- 本地开发环境的日志（本 skill 只查 `test-gz`，不查本地 stdout）
- 需要监控告警配置（用 zhiyan-log 的告警组功能，不在本 skill 覆盖范围）
- 需要生成日志报告文件（本 skill 只在对话里输出摘要，不写文件）
- 非 `wecard-palm (15692)` 项目（本 skill 项目 ID 硬编码）
- test-gz / hk 之外的其他环境（如 prod）

## 环境上下文（默认 test-gz，可切 hk）

本 skill 支持项目 `wecard-palm (15692)` 下的两个环境。**默认查 test-gz**，只有用户明确表达要查香港环境时才切到 hk。

| 字段 | test-gz（默认） | hk（香港） |
|---|---|---|
| 智研项目名 | `wecard-palm` | `wecard-palm` |
| 项目 ID | `15692` | `15692` |
| 环境标识 | `test-gz` | `hk` |
| 环境 ID | `192643` | `284883` |
| 接入名后缀 | `_log_test_gz` | `_log_hk` |
| APM trace 索引 | ⚠️ **禁用**（`apm_15692_test_gz_677a_trace` 实测无数据，别再查） | `apm_15692_hk_b2bd_trace` |
| APM log 索引 | `apm_15692_test_gz_677a_log` | `apm_15692_hk_b2bd_log` |
| 接入名总数 | 73 | 62 |
| MCP 服务 | `zhiyan-log` | `zhiyan-log` |
| 接入名清单 | `references/dataflows.md` §test-gz | `references/dataflows.md` §hk |

### 环境识别 SOP（Step 1，先于任何服务范围识别）

**默认值**：`env = "test-gz"`。

按下列顺序**明确切换**到 hk：

1. **用户显式关键词**：本轮消息里出现 "香港"、"hk"、"hongkong"、"HK 环境"、`_log_hk`、`apm_15692_hk_*` → `env = "hk"`
2. **create-zerus 联动带环境**：如果 `create-zerus` Step 2 的 payload 里 `environment` / `env` 字段是 `hk` → `env = "hk"`（当前 create-zerus 主要针对 test-gz，但保留此路径以便未来扩展）
3. **无任何 hk 信号** → 保持默认 `test-gz`

**关键约束**：
- **单次查询不跨环境**：同一次分析要么 test-gz 要么 hk，不同时查两个环境（数据模型独立，混着看反而误导）
- **服务在两个环境不一定都有**：hk 环境接入名少一些（62 vs 73），例如 `wecardh5gateway` / `wecardadminusermanage` / `wecardactivationapp` 在 hk 没有独立接入 —— 拼接后先跑一次 `GetDataflowListByProjectId(projectId=15692, environment=hk)` 复核；不在的显式告知用户
- **`环境:` 字段必须打印在上下文识别块的第一行**（见下文 §打印上下文来源），让用户一眼看出你查的是哪个环境

### 环境切换的显式对话样例

用户："帮我看下香港环境 wecardpayapp 有没有报错"
→ 识别到"香港" → `env = "hk"` → 接入名 = `wecardpayapp_log_hk`

用户："wecardpayapp 有没有报错"
→ 无 hk 信号 → 默认 `env = "test-gz"` → 接入名 = `wecardpayapp_log_test_gz`

用户："这个 trace_id 在 hk 上跑到哪挂了：abc123"
→ 识别到"hk" + trace_id → `env = "hk"`，模式 B，L1 起点 = 上下文服务的 `_log_hk` + `apm_15692_hk_b2bd_trace`

> 用户如果想查其他环境（prod / 其他项目），请直接告知本 skill 不支持，让用户改用 zhiyan-log MCP 手动查询。

## 前置依赖检查（Step 0，任何查询前必跑一次）

**本 skill 依赖两样东西，AI 无法自动装，缺任何一个都跑不通**：

| 依赖 | 说明 | 谁装 |
|---|---|---|
| `zhiyan-log` MCP server | 提供 `SearchLog` / `QueryClickhouse` / `GetCurrentTimestamp` 等工具 | **用户** 按 https://knot.woa.com/mcp/detail/89 手动装 |
| 太湖 token | zhiyan-log MCP 的鉴权凭据 | **用户** 到太湖平台申请后配到 MCP 配置里 |

### 探测方式（每次 skill 首次调用时跑一次即可，同一会话内可缓存结果）

用**最轻量的健康检查调用**探测：

```
调用 mcp_get_tool_description(toolRequests=[["zhiyan-log", "GetCurrentTimestamp"]])
```

- **返回成功** → MCP 已装、token 已配，进入正常流程
- **返回"tool not found" / "server not connected" / "unknown server"** → MCP 未装 → 打印 §安装引导，**暂停当前查询**
- **返回鉴权错误 / 401 / token invalid** → MCP 装了但 token 没配 / 过期 → 打印 §Token 配置引导，**暂停当前查询**

### 安装引导（MCP 未装时对用户说）

```
[前置依赖缺失] 本 skill 需要 zhiyan-log MCP，但你当前 IDE 里没检测到。

请按以下步骤配置（AI 无法代劳）：
1. 打开 https://knot.woa.com/mcp/detail/89 查看官方安装文档
2. 按文档在 IDE 的 MCP 配置里添加 zhiyan-log server
3. 到太湖平台申请 token，填到 MCP 环境变量里
4. 重启 IDE / 重新加载 MCP 配置

装好后重新说一遍你的诉求，本 skill 会自动继续。
```

### Token 配置引导（MCP 装了但鉴权失败时对用户说）

```
[前置依赖鉴权失败] zhiyan-log MCP 已连接，但太湖 token 缺失 / 过期。

请：
1. 到太湖平台申请或续期 token
2. 更新 MCP 配置里的 token 环境变量
3. 重载 MCP 后重试

参考文档：https://knot.woa.com/mcp/detail/89
```

### 严禁行为

- ❌ **不要**跳过探测直接调 `SearchLog` —— MCP 未装时会以晦涩报错抛出，用户体验极差
- ❌ **不要**假装"我先按经验猜一下"给出结论 —— 没日志就是没日志，直接引导安装
- ❌ **不要**尝试通过 `execute_command` 帮用户装 MCP —— MCP 安装涉及 IDE 配置文件与凭据，必须用户手动完成
- ✅ 探测通过后，本会话剩余轮次可以复用结论，不用每次都探测

## 会话上下文感知（必读，先于任何模式判断）

**核心原则：先用上下文缩小范围，查不到再扩大。** 用户在本会话里可能已经做了很多事，日志查询范围要**优先跟这些事绑定**，而不是每次都全项目 73 个接入名一顿撒。

### 上下文优先级（查询范围锚点）

按下列顺序取"当前会话服务集合"作为查询范围（**首选**），从上到下越靠上优先级越高：

1. **create-zerus 联动**：如果本会话已经用 `create-zerus` skill **成功创建 / 修改过 zerus 环境**（Step 4 返回了 `data.serviceInstanceList`，或用户明示"刚才那套 zerus 环境"），把**该环境覆盖的服务列表**（`serviceInstanceList[].name`）当作**主查询范围**。
   - 通常这就是本次 spec 改动涉及的服务，日志查询天然应聚焦在这几个上。
   - 服务名 → 接入名规律：`serviceInstanceList[i].name + {env 后缀}`（`test-gz` → `_log_test_gz`，`hk` → `_log_hk`；详见 `references/dataflows.md` §仓库名→接入名映射）。
2. **spec / plan 联动**：会话上下文里有 `specs/<VERSION>/<STORYID>-*.md` 或 `plans/<STORYID>-*-plan.md`，从"涉及仓库"表读 `service_name`。
3. **git 现状**：`src/<repo>/` 下有非默认分支 / 未推送提交的仓库，视为"用户当前正在改的"。
4. **用户显式提到的服务**：本轮或近几轮消息里出现的服务名（口语或接入名）。
5. **兜底**：什么上下文都没有 → 才让用户明确说要查哪个服务，或问是不是要做全项目健康巡检。

### trace_id 优先级最高

无论上面哪一条命中，只要用户提供了 `trace_id`：
- **一律先按 trace_id 查**，跳过 level/msg 过滤——trace_id 具备全局唯一性，是最强定位条件
- **查询范围仍然按上下文缩小**：优先只查"当前上下文服务集合的业务日志接入名"
  - `test-gz` → 只查业务接入名（`*_log_test_gz`），**不要再查 `apm_15692_test_gz_677a_trace`**，该索引实测无数据，加进去纯浪费一次 MCP 调用
  - `hk` → 服务的 `*_log_hk` + `apm_15692_hk_b2bd_trace`（HK 环境 APM 有数据，正常查）
- 缩小范围**没查到**再扩大到当前环境**全量接入名**（test-gz 73 个 / hk 62 个，分批并行，见 §模式 B）

### 范围扩大的三级 fallback

任何模式发现"缩小范围查不到"时，按下列顺序逐级放宽（**每级独立打印一次 MCP 查询 + 结果**，让用户看到扩围逻辑）：

| 级别 | 范围 | 触发条件 |
|---|---|---|
| L1 | 上下文服务集合（通常 1~5 个接入名，**限定当前环境**） | 默认起点 |
| L2 | 上下文服务 + 其**上游/下游链路**（见 `references/dataflows.md` §常见链路组合） | L1 total=0 |
| L3 | 当前环境**全量接入名**分批并行（test-gz 73 / hk 62） | L2 仍 total=0 且是 trace 模式，或用户明确要求"扩到全部" |

**L3 属于慢查询**（网关会限流、耗时可能 10s+），扩到 L3 前**必须**向用户确认一次："当前上下文范围没查到，需要扩大到 {env} 环境全部接入名吗？（可能较慢）"

**扩围绝不跨环境**：test-gz 查不到不会自动去 hk 试（反之亦然），环境是用户意图的一部分。除非用户显式说"那试试另一个环境"，才切换 env 重跑。

### 打印上下文来源

每次执行查询前，**在同一条回复里显式列出**当前认定的上下文，让用户一眼看出你在哪个范围上查：

```
[上下文识别]
- 环境: test-gz (默认)  |  或  hk (用户显式指定)
- 来源: create-zerus 环境 (env_id=6597, hashName=abc123)
- 服务范围: palmdiagnosis, palm_local
- 对应接入名: palmdiagnosis_log_test_gz, palm_local_log_test_gz
- 时间窗: 2026-07-15 08:30 ~ 08:45 (+08:00)
- trace_id: (未提供)  |  或  d2f3e730d86ec276963978ada66b01e8
```

`环境:` 一行**永远打印**（不能省略），并显式标注"默认"或"用户指定"来源，方便用户核对是不是查错环境了。

如果上下文缺失（比如全新会话），也要打印：
```
[上下文识别]
- 环境: test-gz (默认)
- 未从会话检出改动服务，请指定要查的服务或范围。
```

## 三种分析模式

调用前先判断用户意图属于哪种模式，如无法判断，先向用户澄清一次。

**所有模式都要先跑上面的"上下文识别"**：拿到服务范围后再决定 DSL 用哪种。

### 模式 A：Root Cause 定位（最常见）

**触发**：用户说"xxx 接口报错"「xxx 服务挂了」「刚才有笔支付失败」「排查一下 activation 服务」

**输入**：
- 服务名（可模糊，如 "支付网关" → 映射到 `wecardpayapp_log_test_gz`）
- 时间窗（用户没说就默认最近 15 分钟；跨小时的用户会主动说）
- 现象描述（可选，用于关键词过滤，如 "签名校验失败"、"panic"、"timeout"）

**SOP**：
1. **服务名映射**：查 `references/dataflows.md`，把用户口语化的服务名映射到接入名；模糊/多选时向用户确认
2. **时间窗解析**：
   - 无明确说 → 默认 `now - 15min ~ now`（先 `GetCurrentTimestamp` 拿基准）
   - 相对时间（"刚才"/"半小时前"/"下午 3 点"）→ 换算成 RFC3339 `+08:00`
3. **量级探测**（关键一步！）：
   ```json
   {"size":0,"query":{"bool":{"filter":[{"term":{"level":"error"}}]}},"aggs":{"total":{"value_count":{"field":"@timestamp"}}}}
   ```
   先看总量，量太大（>1000）先聚合分桶，不要直接拉原文
4. **分层查询**：
   - 优先看 `level:"error"` 的日志
   - 没 error 就看 `level:"warn"` + `msg` 关键词
   - 都没有再看 `status_code:>=500` 或 `cost:>3000`
5. **聚合归类**：按 `msg` / `error` / `file:line` 聚合，取 Top 5 错误
6. **摘要输出**：见 §输出格式

### 模式 B：Trace 链路查询

**触发**：用户给出 `trace_id="xxx"` 或说"这笔请求的调用链"

**输入**：
- `trace_id`（必需）
- 时间窗（可选，默认最近 1 小时；trace_id 具有全局唯一性，窗口可以放宽但别放太宽避免超时）
- 服务范围（可选，**默认按上下文优先级取，见 §会话上下文感知**）

**SOP**：
1. **确定查询范围**（严格按上下文优先级，逐级扩大；下述接入名都要按当前 `env` 拼后缀）：
   - **L1（首选）**：会话上下文里的服务集合（create-zerus / spec / git 里能识别的服务）
     - `test-gz` → 服务 `+ _log_test_gz`（⚠️ **不要**把 `apm_15692_test_gz_677a_trace` 加进 `index_name_list`，实测无数据）
     - `hk` → 服务 `+ _log_hk` + `apm_15692_hk_b2bd_trace`（HK APM 索引有数据）
   - **L2**：L1 total=0 → 扩到上下文服务的**上下游链路**（`references/dataflows.md` §常见链路组合）+ 该环境的 `apigateway_log_{env}`
   - **L3**：L2 仍 total=0 → 询问用户后，才扩到**当前环境全量接入名**分批并行（每批 10 个，避免 index_name_list 太长）
   - 用户显式说"查全部" → 直接跳到 L3（仍限定 env）
2. **DSL**：
   ```json
   {"size":100,"query":{"term":{"trace_id":"xxx"}},"sort":[{"@timestamp":{"order":"asc"}}]}
   ```
3. **按 `@timestamp` 排序合并**，展示：`时间 | 服务 | file:line | level | msg`
4. **识别关键节点**：入口 API、跨服务 RPC、DB 慢查询、报错点
5. **摘要输出**：把链路呈现成时序表格 + root cause 判断

### 模式 C：健康巡检

**触发**：用户说"xxx 服务今天有没有异常"「支付链路健康度」「最近一小时慢请求 TOP」

**输入**：
- 服务范围（一个/多个/一整条链路，如"palm 识别链路" = palmrecognition + palmliveness + palmfeature + palmalignment）
- 时间窗（默认 1 小时）
- 关注维度（错误率 / 慢请求 / status_code 分布，用户没说全都跑一遍）

**SOP**：
1. **错误率**：
   ```json
   {"size":0,"query":{"match_all":{}},"aggs":{"by_level":{"terms":{"field":"level"}}}}
   ```
2. **慢请求 TOP**：
   ```json
   {"size":10,"query":{"range":{"cost":{"gte":3000}}},"sort":[{"cost":{"order":"desc"}}]}
   ```
3. **异常码分布**：
   ```json
   {"size":0,"query":{"range":{"status_code":{"gte":400}}},"aggs":{"by_code":{"terms":{"field":"status_code"}}}}
   ```
4. **接口 TOP**：
   ```json
   {"size":0,"aggs":{"by_url":{"terms":{"field":"url","size":10}}}}
   ```
5. 每个维度独立查完，合并成一份"服务健康报告"

## 通用规范

### 时间窗

- 一律用 **RFC3339 + `+08:00`**，如 `2026-07-15T08:30:00+08:00`
- **⚠️ 时间基准优先级（血泪教训，务必遵守）**：
  1. **用户明确给出的时间**（如"当前时间是 15:30"、"这条日志是最近 30 分钟"）→ **绝对以此为准**
  2. **对话里已出现过的用户本地时间线索**（用户 IDE / additional_data 的 `current_time`）→ 次优先
  3. **`GetCurrentTimestamp` 服务端时间戳** → **仅作最后兜底**，且**不可与用户时间冲突**
     - 曾出现过服务端时钟与用户本地时钟相差 **40 分钟** 的情况，直接导致"最近 30 分钟"窗口整体错开，查不到日志
     - **如果服务端时间戳与用户提供的时间差距 > 5 分钟，必须以用户时间为准，并显式在上下文块里注明"时间基准：用户本地时钟"**
- 默认窗口大小：Root Cause 15min / Trace 1h / 健康巡检 1h
- 超过 6h 的查询要先向用户确认（可能触发慢查询）
- **命中 0 且用户说时间在"最近 X 分钟"** → 除了扩服务范围外，**第一件事是核对时间基准**，把用户时间再确认一次

### DSL 编写要点

- **`query` 用 `bool.filter`**（不评分、更快）：`{"bool":{"filter":[{"term":{"level":"error"}}]}}`
- **精确匹配用 `term`**（level、trace_id、status_code 这类 keyword/long 字段）
- **模糊匹配用 `match_phrase` / `wildcard`**（msg、@message、error 这类文本字段）
- **`size:0` + `aggs`** 做聚合，比拉原文快 10 倍
- **`sort` 用 `@timestamp`**，一般 `desc`（最新在前）；trace 场景用 `asc`（按时序）
- 单次 `size` 不要超过 200，聚合 Top 不要超过 20

### 结果裁剪

- 默认 `raw_json:false`（走 MCP 的文本裁剪，节省 token）
- 需要精确字段值（如复制 request body）时才 `raw_json:true`
- `remove_backslash:true` 用于打印 SQL / JSON 时避免视觉噪音

### 源码关联根因分析（Step Final，所有模式都要做）

**核心原则：日志只是现象，源码才是真相。** 找到日志之后，绝不能停在"日志说 xxx 报错"这一层——必须**下沉到本仓库 `src/` 里对应的服务源码**，把日志字段（`file`/`line`/`func`/`msg`）跟代码对上，才能给出**代码级根因**。

#### 前提：本地已有全量微服务源码

- 本仓库 `src/` 是 **wecard-palm 微服务 monorepo**（Go），大部分线上服务都能找到对应源码
- 目录约定：
  - `src/palm_global/gateway/<service>/` — palm_global 域的网关（如 `palmdevicegateway`、`palmwebgateway`、`palmactivationgateway`）
  - `src/palm_global/<service>/` — palm_global 域的业务服务
  - `src/palm_local/gateway/<service>/` — palm_local 域的网关（如 `wecarddevicegateway`）
  - `src/palm_local/<service>/` — palm_local 域的业务服务
  - `src/device_manage/gateway/<service>/` — device_manage 域网关（如 `devicegateway`）
  - `src/device_manage/<service>/` — device_manage 域业务
  - `src/infrastructure/` — 通用中间件、错误处理、日志、注册中心等**共享库**（`middleware`、`errorhandler`、`errcodes`、`logger`…）
  - `src/palm_global/common/` / `src/palm_local/common/` / `src/device_manage/common/` — 各域共享代码（如 `wechaterrorhandler`）
  - `src/palm_proto/` / `src/proto/` — 各服务 proto 定义与 grpc-gateway 生成代码
- **接入名 → 源码目录映射**：一般 `<service>_log_{env}` 的 `<service>` 就是目录名。找不到时用 `search_file` 按目录名递归查，或用 `search_content` 查服务的 `service_name` / `AppName` 常量。

#### SOP（找到日志之后必做）

**只要日志里带 `file` / `line` / `func` 字段（这是本项目 logrus 的标配），就必须做以下步骤，不做等于没查**：

1. **定位源文件**：
   - 日志字段 `file: xxx/yyy/zzz.go`、`func: funcName`、`line: N`
   - **优先用 `search_file` 或 `search_content` 直接找到 `src/` 下的对应文件**：
     - 文件名唯一 → `search_file(pattern=<basename>, target_directory=/abs/src)` 一步到位
     - 文件名重名（如 `errorhandler.go` 在 `infrastructure/` 和各 `common/` 下都有多份）→ **必须结合 `server_name` / 接入名判定属于哪个域**
       - `server_name: palmdevicegateway` → `palm_global` 域 → 读 `src/palm_global/common/wechaterrorhandler/errorhandler.go`
       - `server_name: wecarddevicegateway` → `palm_local` 域 → 读 `src/palm_local/common/wechaterrorhandler/errorhandler.go`
       - `server_name: devicegateway` → `device_manage` 域 → 读 `src/device_manage/common/wechaterrorhandler/errorhandler.go`
   - **读文件时以 `line` 字段为中心**，`offset=max(1, line-30)`, `limit=60`，拿到上下文

2. **对齐日志与代码**：
   - 把日志 `msg` 跟代码里 `logger.Errorf("...")` / `.Error("...")` / `logger.WithFields(...).Error("...")` **字符串精确匹配**
   - 逐字对齐后**确认这个日志确实是这行代码打的**（不同分支可能有相同 msg，需要 file:line 一起对）

3. **向上还原调用链**：
   - 从命中的日志代码行开始，看**这段代码在什么条件下被触发**（if/else 分支、error return、panic recover）
   - 顺着函数调用栈往上找：这个函数是谁调用的？是 HTTP handler、gRPC method、cron job 还是消息消费者？
   - **优先用 `codebase_search`**（语义搜索，效率高）问："哪里调用了 `<funcName>`？"、"`<msg 关键词>` 相关的处理流程是什么？"
   - 结合 `middleware` / `errorhandler` 一起看，很多"表面错误"其实是中间件在兜底打的（比如 `http_log_middleware.go` 会对所有 4xx/5xx 打 `http handle failed`）

4. **判定根因层次**（务必区分三层，别混淆）：
   - **表面层（Symptom）**：日志字面说的错，如 `msg="http handle failed"`
   - **触发层（Trigger）**：直接导致这条日志的代码分支，如 `errorhandler.go:44` 因为 `runtime.RPCMethod` 返回 `ok=false`
   - **根因层（Root Cause）**：真正的业务/架构原因，如"请求路径 `/solr/` 未在 grpc-gateway 注册 → 大概率是 mesh 误路由 / 漏洞扫描"
   - **摘要里三层都要有**，只写表面层等于没分析

5. **主动指出代码缺陷**（可选但强烈建议）：
   - 顺着代码看到有明显问题（如"这里该返 404 却返了 500"、"error 没 wrap 导致堆栈丢失"、"重试次数写死"）→ **直接在建议里列出改法**并给出文件:行号
   - 用户没问你也要提——他现在是在排查问题，正好可以顺手修

#### 何时可以**跳过**源码关联

- 日志已经**足够自解释**（如 `err="mysql: connection refused"` + `dsn=xxx`，根因就在网络/DB 本身，看代码也没用）
- 日志字段里**没有 file/line/func**（少数老服务没接入统一 logger），此时只能靠 msg 关键词在 `src/` 里全局搜
- 用户只要**服务健康巡检数据**（模式 C 的错误率/慢请求 TOP），只想看聚合数字时——但**只要出现具体错误 msg，就仍要下沉源码**

#### 摘要里怎么呈现

在 §输出格式的 `### 详情` 后追加一小节：

```
### 源码根因（结合 src/）

- 报错点：`src/palm_global/common/wechaterrorhandler/errorhandler.go:44`
- 触发条件：`runtime.RPCMethod(ctx)` 返回 ok=false（grpc-gateway 未匹配到路由）
- 调用链：grpc-gateway NotFoundHandler → ErrorHandler → 写 500 → http_log_middleware 打 `http handle failed`
- 根因层次：
  - 表面：http handle failed (500)
  - 触发：ctx 里没有 rpc method
  - 根因：请求路径 `/solr/` 不在 palmdevicegateway 注册路由中（大概率 mesh 误路由或漏洞扫描）
- 代码缺陷建议：该路径应返 404 而非 500，避免污染错误率大盘（`errorhandler.go:44-46` 可改）
```

### 无结果处理

先自查：
1. **⏰ 时间基准是不是搞错了**（最高频，务必第一位核对！）：
   - 用户说"最近 30 分钟" / "刚才" → 我用的窗口对不对？服务端 `GetCurrentTimestamp` 是否与用户本地时间对齐？
   - 若上下文能拿到用户本地时间（additional_data `current_time` 或用户显式说时间），**必须以用户时间为准**，服务端时间戳仅兜底
   - 曾出现服务端时钟与用户时钟差 40 分钟，直接错过整个时间窗的教训
2. **环境是不是搞错了**：用户口语里有没有 hk / 香港 关键词被漏识别？（复核 `env`）
3. 接入名拼错（一律查 `references/dataflows.md` 里对应环境的清单，别把 test-gz 的名字带到 hk 查）
4. 过滤条件太严（先去掉 filter 再看有没有数据）
5. **量级探测反证**：对目标接入名跑一次 `match_all` count，确认索引本身在该时间窗有数据；若 count>0 但目标 trace_id/关键词 = 0，才是真的"目标不在"

四次自查后仍无结果，如实告知用户："该时间窗内 `xxx_log_{env}` 索引里没有匹配 `xxx` 的日志。" 环境值和时间窗都要显式带出，让用户核对。

## 输出格式（对话摘要）

三种模式统一遵循如下结构（Markdown），保证信息密度 + 可读性：

```
### 分析摘要

- **模式**：Root Cause / Trace 链路 / 健康巡检
- **环境**：test-gz  |  或  hk
- **时间窗**：2026-07-15 08:30 ~ 08:45 (+08:00)
- **范围**：wecardpayapp、apigateway

### 关键发现
1. ... (最多 5 条，每条一行，带证据)

### 详情
（表格 / 时序 / 分布图，视模式而定）

### 源码根因（结合 src/）
- 报错点：`src/<domain>/<path>/<file>.go:<line>`
- 触发条件：<直接触发这行代码的分支/条件>
- 调用链：<从入口到报错点的关键跳转>
- 根因层次：
  - 表面：<日志字面 msg>
  - 触发：<代码级直接原因>
  - 根因：<业务/架构级原因>
- 代码缺陷建议（可选）：<如有明显问题，给出文件:行号 + 改法>

### 建议
- ... (可选，1~3 条排查方向)
```

**硬约束**：`### 源码根因` 一节**必须存在**，除非命中 §源码关联根因分析 里列出的"可跳过场景"（日志自解释 / 无 file:line 字段 / 纯聚合数字）；跳过时也要显式一行说明"本次跳过源码关联，原因：..."。

**硬约束**：
- 不要贴超过 20 行的原始日志（用 file:line + msg 首句代替）
- 涉及敏感字段（`request` / `response` / `sql` 里的完整 body）默认打印前 200 字符 + "..."
- 摘要里的 pod 名 / IP 保留完整（便于用户在 zhiyan 前端复现）

## 常见错误码（自查用）

| MCP 返回 | 原因 | 处理 |
|---|---|---|
| `code != 0` 且 msg 里有 "index not found" | 接入名拼错或索引未创建 | 核对 `references/dataflows.md` |
| `total.value: 0` | 时间窗内无数据 | 放宽时间窗；确认服务近期是否有流量 |
| 查询超时 / MCP 返回 timeout | 时间窗太大 or 接入名列表太长 | 缩窗口、分批查 |
| 字段不存在（`No mapping found`） | 该接入名 ES 字段与预期不符 | `GetEsFields` 确认可用字段 |

## 快速上手示例

### 示例 1：无上下文，用户显式指定服务

**用户输入**：「刚才 wecardpayapp 有报错，帮我看看」

**Skill 动作**：
1. 上下文识别：会话里没有 create-zerus / spec 上下文 → 用用户显式提到的 `wecardpayapp`
2. 识别模式 = Root Cause
3. 服务名映射：`wecardpayapp` → `wecardpayapp_log_test_gz`
4. 时间窗：`GetCurrentTimestamp` → `now - 15min ~ now`
5. 量级探测：`level:error` count
6. 若 count > 0：拉 Top 10 error 日志按 `msg` 聚合
7. 输出摘要（见 §输出格式）

### 示例 2：**create-zerus 联动**（推荐场景）

**会话前情**：用户前面用 `create-zerus` skill 拉起了一套环境，覆盖服务 `palmdiagnosis` + `palm_local`。

**用户输入**：「联调时刚才那笔请求好像出错了，帮我看看日志」

**Skill 动作**：
1. **上下文识别**：从会话历史检出 create-zerus 已成功创建环境，`serviceInstanceList = [palmdiagnosis, palm_local]`
2. **服务范围锁定**：`palmdiagnosis_log_test_gz` + `palm_local_log_test_gz`（**不去查其他 71 个接入名**）
3. 打印上下文块（见 §打印上下文来源）
4. 识别模式：Root Cause（用户说"出错"、未提供 trace_id）
5. 时间窗：`now - 15min ~ now`
6. **只在 L1 范围**跑 `level:error` count 聚合
7. 若 total=0 → 扩到 L2（+ `apigateway_log_test_gz`）
8. 若 L2 仍 0 → 询问是否扩到 L3

### 示例 3：**create-zerus 联动 + trace_id**

**会话前情**：同上，环境已建。

**用户输入**：「trace_id=abc123def456 这笔请求跑到哪挂了」

**Skill 动作**：
1. 上下文识别：环境 = test-gz（无 hk 关键词），服务 = `palmdiagnosis` + `palm_local`
2. **trace_id 提供 → 直接进模式 B**
3. **查询范围（L1）**：`palmdiagnosis_log_test_gz` + `palm_local_log_test_gz`（⚠️ test-gz 环境**不带** APM trace 索引，`apm_15692_test_gz_677a_trace` 无数据）
4. DSL：`{"query":{"term":{"trace_id":"abc123def456"}}, ...}`
5. L1 有结果 → 直接按 @timestamp 排序输出链路；无结果 → L2 加 `apigateway_log_test_gz`；仍无 → 问用户是否 L3

### 示例 4：**HK 环境查询**

**用户输入**：「香港环境 wecardpayapp 刚才是不是有报错？」

**Skill 动作**：
1. **环境识别**：检出"香港" → `env = "hk"`（覆盖默认 test-gz）
2. 上下文识别：会话无 zerus / spec 上下文 → 用用户显式提到的 `wecardpayapp`
3. 服务名映射：`wecardpayapp` → `wecardpayapp_log_hk`（**用 `_log_hk` 后缀，不是 `_log_test_gz`**）
4. 打印上下文块：
   ```
   [上下文识别]
   - 环境: hk (用户指定 "香港")
   - 服务范围: wecardpayapp
   - 对应接入名: wecardpayapp_log_hk
   - 时间窗: ...
   ```
5. 识别模式 = Root Cause
6. 量级探测：`level:error` count（DSL 不变，只是 `index_name_list = ["wecardpayapp_log_hk"]`）
7. 若 total=0 → L2 加 `apigateway_log_hk`；仍 0 → 询问用户是否扩到 hk 全 62 个接入名
8. 输出摘要，`环境:` 一行写 `hk`

**注意**：不会自动去 test-gz 试 —— 环境切换必须由用户显式发起。

---

参考 `references/dataflows.md`（接入名 catalog + **仓库名→接入名映射**）、`references/dsl-cookbook.md`（DSL 模板）、`references/fields.md`（字段字典）辅助完成上述过程。
