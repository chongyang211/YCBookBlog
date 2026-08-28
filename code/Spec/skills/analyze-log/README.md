# analyze-log skill

分析 `test-gz` 环境（智研项目 `wecard-palm / 15692`）线上日志的通用 skill。基于 `zhiyan-log` MCP，覆盖三种典型场景：

| 模式 | 触发场景 |
|---|---|
| **Root Cause 定位** | "xxx 接口报错了帮我看看" / "支付服务刚才有 panic" |
| **Trace 全链路** | "这个 trace_id 的调用链是什么样" / "d2f3e730... 这笔请求跑到哪挂了" |
| **健康巡检** | "掌纹识别链路今天有没有异常" / "过去 1h 慢请求 TOP" |

> ⚠️ **依赖外部 MCP（AI 无法自动安装）**：本 skill 依赖 [`zhiyan-log` MCP](https://knot.woa.com/mcp/detail/89) 与太湖 token，必须**用户手动**在 IDE 里配置好才能使用。详见 [§前置条件](#前置条件重要ai-无法自动装)。

## 特点

- **AI 侧零安装**：无需装 Python 包、不落盘、不写脚本；纯 AI + MCP 编排（MCP 本身需用户手动装，见 [§前置条件](#前置条件重要ai-无法自动装)）
- **多环境支持**：默认查 `test-gz`，用户明说"香港/hk"时切到 `hk`（环境 ID 284883），两个环境有各自的接入名清单与 APM 索引，单次查询不跨环境
- **上下文感知**：优先根据本会话已有的 `create-zerus` 环境 / spec plan / git 现状**自动锁定查询服务范围**，1~5 个接入名精准查而非全项目撒网；查不到再逐级扩大（L1 上下文 → L2 上下游链路 → L3 当前环境全量接入名，test-gz 73 / hk 62）
- **trace_id 优先**：用户给 trace_id 时以其为最强定位条件，仍先在上下文范围内查
- **🔬 源码关联根因分析**：查到日志后**自动下沉到 `src/` 下对应 Go 微服务源码**，按 `file:line:func` 精确定位报错点，还原调用链，给出三层根因（表面 / 触发 / 根因），并主动指出代码缺陷 —— 不只是复述日志字面
- **接入名内置**：73 个 test-gz 接入名按业务域预分类（见 `references/dataflows.md`），另附**仓库名/zerus service_name → 接入名映射表**（与 `create-zerus` 联动的关键）
- **DSL Cookbook**：三种模式的 ES DSL 模板 + 常见组合技（见 `references/dsl-cookbook.md`）
- **对话摘要输出**：Markdown 格式，关键发现 + 详情 + **源码根因** + 建议四段式

## 与 create-zerus skill 的联动

当本会话已经用 `create-zerus` 拉起过环境（Step 4 返回过 `data.serviceInstanceList`）：

- `analyze-log` 会**自动**把该环境覆盖的服务作为主查询范围
- 服务名 → 接入名规律：`service_name + "_log_test_gz"`（例外见 `references/dataflows.md` §仓库名→接入名映射）
- 每次查询前会打印**上下文识别块**，让你确认查的是哪几个接入名

典型联动流程：
```
1. /spec-implement + /spec-push（改代码 + 推分支）
2. create-zerus（拉起联调环境，覆盖 palmdiagnosis + palm_local）
3. 你联调时发现问题："刚才那笔请求出错了" / "trace_id=xxx 挂了"
4. analyze-log 自动锁定到 palmdiagnosis_log_test_gz + palm_local_log_test_gz 上查
```

## 前置条件（重要，AI 无法自动装）

本 skill 依赖两个外部资源，必须**由用户手动配置**：

| 依赖 | 说明 | 安装方式 |
|---|---|---|
| `zhiyan-log` MCP server | 提供日志查询工具（`SearchLog` / `QueryClickhouse` / `GetCurrentTimestamp` 等） | 参考 [Knot MCP 89 号](https://knot.woa.com/mcp/detail/89) 在 IDE 里添加 MCP 配置 |
| 太湖 token | zhiyan-log 的鉴权凭据 | 到太湖平台申请后填入 MCP 环境变量 |
| 项目查询权限 | 用户账号需要 `wecard-palm (15692)` 项目的日志查看权限 | 找项目管理员开权限 |

### 首次使用检查

Skill 会在**执行任何查询前**自动探测 MCP 是否可用：
- **未装 MCP** → 打印安装引导 + Knot 文档链接，暂停执行
- **装了但 token 无效** → 打印 token 配置引导，暂停执行
- **一切就绪** → 静默进入正常流程

> 探测结果在同一会话内会被缓存，只在首次触发 skill 时打印引导。装好后重新描述诉求即可继续。

## 使用方式

直接说人话，本 skill 会自动识别意图：

- "帮我看看 wecardpayapp 刚才有没有报错"
- "trace_id=d2f3e730d86ec276963978ada66b01e8 拉一下全链路"
- "掌纹识别链路过去 1 小时健康状况"

Skill 会：
1. 把口语化服务名映射到接入名（模糊时向你确认）
2. 用 `GetCurrentTimestamp` 定基准，构造 RFC3339 时间窗
3. 先做量级探测（`size:0 + aggs`），量大时聚合、量小时拉原文
4. 输出结构化摘要（不贴大段原始日志）

## 环境范围

**支持 test-gz（默认）和 hk 两个环境**。项目固定为 `wecard-palm (15692)`。

| 字段 | test-gz（默认） | hk（香港） |
|---|---|---|
| 环境标识 | `test-gz` | `hk` |
| 环境 ID | `192643` | `284883` |
| 接入名后缀 | `_log_test_gz` | `_log_hk` |
| APM trace 索引 | ⚠️ 禁用（`apm_15692_test_gz_677a_trace` 无数据） | `apm_15692_hk_b2bd_trace` |
| 接入名数量 | 73 | 62 |

**切换规则**：
- 用户口语里出现"香港"、"hk"、"hongkong" → 切到 hk
- 未提及 → 默认 test-gz
- 单次查询不跨环境（扩围也只在当前环境内扩）
- 服务在两个环境不一定都有（hk 少一些），拼接后先复核

其他环境（prod / 其他项目）不支持，会在识别阶段直接告知并让用户改用 zhiyan-log MCP 手动查。

## 相关文件

- `SKILL.md`：详细 SOP（3 种模式流程、时间窗规范、输出格式）
- `references/dataflows.md`：73 个接入名分组 catalog + 常见链路组合 + 口语映射
- `references/dsl-cookbook.md`：ES DSL 模板（含反模式）
- `references/fields.md`：字段字典（keyword vs text、数值单位、常用组合）
