# 接入名 Catalog（test-gz + hk）

项目固定：`wecard-palm (15692)`。本文档收录**两个环境**的接入名清单：

| 环境 | 环境 ID | 接入名后缀 | APM trace 索引 | 数量 |
|---|---|---|---|---|
| `test-gz`（默认） | 192643 | `_log_test_gz` | ⚠️ **禁用**（`apm_15692_test_gz_677a_trace` 实测无数据） | 73 |
| `hk`（香港） | 284883 | `_log_hk` | `apm_15692_hk_b2bd_trace` | 62 |

> 本清单基于 `GetDataflowListByProjectId(projectId=15692, environment=<env>)` 实测结果。若发现遗漏（服务上线新增），先用 MCP 重跑一次拉最新列表，不要凭空猜名字。

**关键约束**：查询前必须先确定 `env`，再按对应后缀拼接接入名。**服务在两个环境不一定都有**（见 §HK 环境章节的差异表）。

---

# test-gz 环境接入名 Catalog

命名规律：`{服务名}_log_test_gz`，共 73 个接入名。

## 按业务域分组

### 掌纹 SDK 服务链路（palm-*）

| 服务 | 接入名 | 说明 |
|---|---|---|
| 网关 | `palmactivationgateway_log_test_gz` | 掌纹激活网关 |
| 激活 | `palmactivation_log_test_gz` | 掌纹激活服务 |
| Web 网关 | `palmwebgateway_log_test_gz` | 掌纹 Web 侧网关 |
| Device 网关 | `palmdevicegateway_log_test_gz` | 掌纹设备网关 |
| Activation 网关 | `palmactivationgateway_log_test_gz` | 掌纹激活网关（有分离版本） |
| 识别 | `palmrecognition_log_test_gz` | 掌纹识别核心 |
| 活体检测 | `palmliveness_log_test_gz` | 静态活体 |
| 在线活体 | `palmonlineliveness_log_test_gz` | 在线活体 |
| 动作活体 | `palmactionlive_log_test_gz` | 动作活体 |
| 特征 | `palmfeature_log_test_gz` | 掌纹特征提取 |
| 浅层特征 | `palmshallowfeature_log_test_gz` | 浅特征 |
| 对齐 | `palmalignment_log_test_gz` | 掌纹对齐 |
| 质量 | `palmquality_log_test_gz` | 图像质量评估 |
| 评估 | `palmevaluation_log_test_gz` | 综合评估 |
| Zeus Lite | `palmzeuslite_log_test_gz` | zeus 轻量版 |
| 用户管理 | `palmusermanage_log_test_gz` | palm 用户管理 |
| 设备管理 | `palmdevicemanage_log_test_gz` | palm 设备管理 |
| 设备认证 | `palmdeviceauth_log_test_gz` | 设备鉴权 |
| 设备运维 | `palmdeviceops_log_test_gz` | 设备运维 |
| 订阅 | `palmsubscriber_log_test_gz` | 订阅推送 |
| 指令 | `palminstruction_log_test_gz` | 指令下发 |
| 升级 | `palmupgrade_log_test_gz` | 设备升级 |
| 许可证 | `palmlicense_log_test_gz` | 许可证服务 |
| 控制台 | `palmconsole_log_test_gz` | 内部控制台 |
| gRPC UI Proxy | `palmgrpcuiproxy_log_test_gz` | gRPC UI 代理 |
| 诊断 | `palmdiagnosis_log_test_gz` | 诊断服务 |
| Dato 代理 | `palmdatofproxy_log_test_gz` | Dato 代理 |
| 信封加密工具 | `palmenvelopeencrypttool_log_test_gz` | 信封加密 |
| 私有识别 | `privaterecognition_log_test_gz` | 私有识别 |

### WeCard 业务链路（wecard-*）

| 服务 | 接入名 | 说明 |
|---|---|---|
| 支付 | `wecardpayapp_log_test_gz` | 支付应用 |
| 订单 | `wecardorderapp_log_test_gz` | 订单应用 |
| 激活 | `wecardactivationapp_log_test_gz` | wecard 激活 |
| 激活网关 | `wecardactivationgateway_log_test_gz` | wecard 激活网关 |
| Palm 应用 | `wecardpalmapp_log_test_gz` | wecard 侧掌纹应用 |
| 设备网关 | `wecarddevicegateway_log_test_gz` | wecard 设备网关 |
| 设备管理 | `wecarddevicemanageapp_log_test_gz` | 设备管理应用 |
| 设备认证 | `wecarddeviceauth_log_test_gz` | 设备鉴权 |
| 应用网关 | `wecardappgateway_log_test_gz` | 应用侧网关 |
| 应用鉴权 | `wecardappauth_log_test_gz` | 应用鉴权 |
| 开放网关 | `wecardopengateway_log_test_gz` | 对外开放网关 |
| H5 网关 | `wecardh5gateway_log_test_gz` | H5 侧网关 |
| Web 网关 | `wecardwebgateway_log_test_gz` | Web 侧网关 |
| 控制台网关 | `wecardconsolegateway_log_test_gz` | 控制台网关 |
| 用户管理 | `wecardusermanage_log_test_gz` | wecard 用户管理 |
| 管理员用户管理 | `wecardadminusermanage_log_test_gz` | admin 用户管理 |
| 数据分析（App） | `wecarddataanalysisapp_log_test_gz` | 数据分析应用 |
| 数据分析（Task） | `wecarddataanalysistask_log_test_gz` | 数据分析任务 |
| 前端 | `palm_pay_fe_log_test_gz` | palm-pay 前端 |
| 前端 H5 | `palm_local_h5_log_test_gz` | 本地 H5 前端 |

### 通用基础设施（gateway/device/upgrade/...）

| 服务 | 接入名 | 说明 |
|---|---|---|
| API 网关 | `apigateway_log_test_gz` | 统一 API 入口（重点关注） |
| 设备网关 | `devicegateway_log_test_gz` | 通用设备网关 |
| 设备接入 | `deviceaccessapp_log_test_gz` | 设备接入应用 |
| 设备管理 | `devicemanageapp_log_test_gz` | 通用设备管理 |
| 设备 OTel 采集 | `deviceotelcolapp_log_test_gz` | OpenTelemetry Collector |
| 设备上报（默认） | `devicereportapp_log_test_gz` | 设备上报应用 |
| 指令应用 | `instructionapp_log_test_gz` | 指令下发应用 |
| 升级应用 | `upgradeapp_log_test_gz` | 升级应用 |
| 升级检查脚本 | `checkandupdateupgradetaskscript_log_test_gz` | 升级检查脚本 |
| Demo | `wecardpalmdemo_log_test_gz` | wecard palm 演示 |
| API 测试 | `palmtestapi_log_test_gz` | palm API 测试 |
| Helper 脚本 | `helperscript_log_test_gz` | 辅助脚本 |

### 设备上报（按 device_id / 场景切分的多接入）

`devicereportapp_*_test_gz` 系列按不同的设备/场景切分，一般不需要单独查，除非用户明确指出某个 device_id：

`devicereportapp_16160`、`_18437`、`_20021`、`_21960`、`_22057`、`_22058`、`_25261`、`_27919`、`_28425`、`_32048`、`_32168`

### APM 索引（trace / 结构化日志）

| 接入名 | 说明 |
|---|---|
| `apm_15692_test_gz_677a_trace` | ⚠️ **禁用**：实测无实际数据，skill 不要再把它加入 `index_name_list`。trace_id 查询直接按业务接入名查 `trace_id` 字段即可 |
| `apm_15692_test_gz_677a_log` | APM 结构化日志 |

---

# HK 环境接入名 Catalog

命名规律：`{服务名}_log_hk`，共 62 个接入名（比 test-gz 少 11 个）。

## 完整清单（按业务域分组）

### 掌纹 SDK 服务链路（palm-*，hk）

| 服务 | 接入名 | 说明 |
|---|---|---|
| 激活 | `palmactivation_log_hk` | 掌纹激活服务 |
| 激活网关 | `palmactivationgateway_log_hk` | 掌纹激活网关 |
| Device 网关 | `palmdevicegateway_log_hk` | 掌纹设备网关 |
| Web 网关 | `palmwebgateway_log_hk` | 掌纹 Web 侧网关 |
| 识别 | `palmrecognition_log_hk` | 掌纹识别核心 |
| 活体 | `palmliveness_log_hk` | 静态活体 |
| 在线活体 | `palmonlineliveness_log_hk` | 在线活体 |
| 动作活体 | `palmactionlive_log_hk` | 动作活体 |
| 特征 | `palmfeature_log_hk` | 特征提取 |
| 浅层特征 | `palmshallowfeature_log_hk` | 浅特征 |
| 对齐 | `palmalignment_log_hk` | 掌纹对齐 |
| 质量 | `palmquality_log_hk` | 图像质量评估 |
| 评估 | `palmevaluation_log_hk` | 综合评估 |
| Zeus Lite | `palmzeuslite_log_hk` | zeus 轻量版 |
| 用户管理 | `palmusermanage_log_hk` | palm 用户管理 |
| 设备管理 | `palmdevicemanage_log_hk` | palm 设备管理 |
| 设备认证 | `palmdeviceauth_log_hk` | 设备鉴权 |
| 设备运维 | `palmdeviceops_log_hk` | 设备运维 |
| 订阅 | `palmsubscriber_log_hk` | 订阅推送 |
| 指令 | `palminstruction_log_hk` | 指令下发 |
| 升级 | `palmupgrade_log_hk` | 设备升级 |
| 控制台 | `palmconsole_log_hk` | 内部控制台 |
| gRPC UI Proxy | `palmgrpcuiproxy_log_hk` | gRPC UI 代理 |
| 诊断 | `palmdiagnosis_log_hk` | 诊断服务 |
| Dato 代理 | `palmdatofproxy_log_hk` | Dato 代理 |
| 信封加密工具 | `palmenvelopeencrypttool_log_hk` | 信封加密 |
| 私有识别 | `privaterecognition_log_hk` | 私有识别 |
| Demo | `wecardpalmdemo_log_hk` | wecard palm 演示 |
| AFS H5 | `afs_palm_h5_log_hk` | AFS palm H5 页面 |

### WeCard 业务链路（wecard-*，hk）

| 服务 | 接入名 | 说明 |
|---|---|---|
| 支付 | `wecardpayapp_log_hk` | 支付应用 |
| 订单 | `wecardorderapp_log_hk` | 订单应用 |
| Palm 应用 | `wecardpalmapp_log_hk` | wecard 侧掌纹应用 |
| 设备网关 | `wecarddevicegateway_log_hk` | wecard 设备网关 |
| 设备管理 | `wecarddevicemanageapp_log_hk` | 设备管理应用 |
| 设备认证 | `wecarddeviceauth_log_hk` | 设备鉴权 |
| 应用网关 | `wecardappgateway_log_hk` | 应用侧网关 |
| 应用鉴权 | `wecardappauth_log_hk` | 应用鉴权 |
| 开放网关 | `wecardopengateway_log_hk` | 对外开放网关 |
| 控制台网关 | `wecardconsolegateway_log_hk` | 控制台网关 |
| 用户管理 | `wecardusermanage_log_hk` | wecard 用户管理 |
| 前端 | `palm_pay_fe_log_hk` | palm-pay 前端 |

### 通用基础设施（hk）

| 服务 | 接入名 | 说明 |
|---|---|---|
| API 网关 | `apigateway_log_hk` | 统一 API 入口（重点关注） |
| 设备网关 | `devicegateway_log_hk` | 通用设备网关 |
| 设备接入 | `deviceaccessapp_log_hk` | 设备接入应用 |
| 设备管理 | `devicemanageapp_log_hk` | 通用设备管理 |
| 设备 OTel 采集 | `deviceotelcolapp_log_hk` | OpenTelemetry Collector |
| 设备上报（默认） | `devicereportapp_log_hk` | 设备上报应用 |
| 指令应用 | `instructionapp_log_hk` | 指令下发应用 |
| 升级应用 | `upgradeapp_log_hk` | 升级应用 |
| 升级检查脚本 | `checkandupdateupgradetaskscript_log_hk` | 升级检查脚本 |
| Helper 脚本 | `helperscript_log_hk` | 辅助脚本 |

### 设备上报（按 device_id / 场景切分，hk）

`devicereportapp_*_hk` 系列按不同设备切分，一般不需要单独查，除非用户明确指出某个 device_id：

`devicereportapp_16160`、`_18437`、`_20021`、`_21960`、`_22057`、`_22058`、`_25261`、`_27919`、`_28425`、`_32168`

（共 10 个 device 切分，比 test-gz 少一个 `_32048`）

### APM 索引（hk）

| 接入名 | 说明 |
|---|---|
| `apm_15692_hk_b2bd_trace` | APM 分布式追踪数据（trace_id 全链路查询首选） |
| `apm_15692_hk_b2bd_log` | APM 结构化日志 |

> HK APM hash 是 `b2bd`（不是 test-gz 的 `677a`），别搞混。

## HK 与 test-gz 的差异（重要）

**HK 环境缺少以下服务的独立接入**（test-gz 有，hk 无）：

| 缺失服务 | test-gz 接入名 | 说明 |
|---|---|---|
| `palmtestapi` | `palmtestapi_log_test_gz` | 测试环境专属 |
| `palmlicense` | `palmlicense_log_test_gz` | HK 无独立 license 服务 |
| `wecardactivationapp` | `wecardactivationapp_log_test_gz` | HK 走别的激活方案 |
| `wecardactivationgateway` | `wecardactivationgateway_log_test_gz` | 同上 |
| `wecardh5gateway` | `wecardh5gateway_log_test_gz` | HK 无独立 H5 网关 |
| `wecardwebgateway` | `wecardwebgateway_log_test_gz` | HK 无独立 Web 网关 |
| `wecardadminusermanage` | `wecardadminusermanage_log_test_gz` | admin 后台仅 test-gz |
| `wecarddataanalysisapp` | `wecarddataanalysisapp_log_test_gz` | 数据分析仅 test-gz |
| `wecarddataanalysistask` | `wecarddataanalysistask_log_test_gz` | 同上 |
| `palm_local_h5` | `palm_local_h5_log_test_gz` | 本地 H5 前端仅 test-gz |
| `palmalignment/palmfeature 等特殊配置` | — | 特征名可能有 mapping 差异 |

**HK 独有 / 命名差异**：

| HK 接入名 | test-gz 对应 | 说明 |
|---|---|---|
| `afs_palm_h5_log_hk` | 无 | HK 独有的 AFS palm H5 |
| （其他基本一一对应） | | |

**处理原则**：
- 拼出来的 `<service>_log_hk` 若不在 §HK 完整清单里 → **先跑 MCP 复核** `GetDataflowListByProjectId(projectId=15692, environment=hk)`（可能新增了）
- 复核后仍无 → **显式告知用户**"服务 `<service>` 在 hk 环境无独立日志接入"，让用户改到 test-gz 或从 `apigateway_log_hk` 侧查上下游
- 不要静默把用户从 hk 切回 test-gz —— 环境切换必须用户显式发起

---

# 通用参考（两环境共用）

## 常见链路组合（模式 C 巡检用）

服务名列在下方（不带后缀），使用时按当前 `env` 拼后缀：test-gz → `_log_test_gz`，hk → `_log_hk`。**部分链路在 hk 环境组成不同**（因为有些服务 hk 没有），已在说明列标注。

| 场景 | 涉及服务 | 环境适配 |
|---|---|---|
| 掌纹识别链路 | `palmrecognition` + `palmliveness` + `palmfeature` + `palmalignment` + `palmquality` | 两环境都有 |
| 支付主链路 | `apigateway` + `wecardpayapp` + `wecardorderapp` + `wecardactivationapp` | ⚠️ hk 无 `wecardactivationapp`，去掉该项 |
| 设备激活链路 | `palmactivationgateway` + `palmactivation` + `palmdeviceauth` + `palmlicense` | ⚠️ hk 无 `palmlicense`，去掉该项 |
| 设备接入链路 | `devicegateway` + `deviceaccessapp` + `devicemanageapp` + `devicereportapp` | 两环境都有 |
| WeCard 网关层 | `wecardappgateway` + `wecardopengateway` + `wecardwebgateway` + `wecardh5gateway` + `wecardconsolegateway` | ⚠️ hk 只保留 `wecardappgateway` + `wecardopengateway` + `wecardconsolegateway` |

## 服务名映射启发规则

用户口语 → 服务名（后缀按当前 `env` 拼）的映射常识：

- 「支付网关」/「支付」→ `wecardpayapp`
- 「API 网关」/「apigw」→ `apigateway`
- 「订单」→ `wecardorderapp`
- 「激活」→ 先问是"palm 激活"还是"wecard 激活"（palm-activation vs wecard-activation；⚠️ hk 无 `wecardactivation`）
- 「设备网关」→ 至少 3 个候选（`palmdevicegateway` / `wecarddevicegateway` / `devicegateway`），必须让用户选
- 「掌纹识别」→ `palmrecognition`（可能还要带上 liveness/feature）
- 「activation gateway」/「激活网关」→ 同样两个候选（palm/wecard；⚠️ hk 无 `wecardactivationgateway`）
- 「前端」→ 先问是"palm-pay"还是"local h5"（`palm_pay_fe` vs `palm_local_h5`；⚠️ hk 只有 `palm_pay_fe`，无 `palm_local_h5`）

**原则**：
- 拿不准时先问用户，而非猜测
- 服务名确定后按 `env` 拼后缀（`_log_test_gz` / `_log_hk`）
- 拼完先看是否在对应环境的清单里（HK 只有 62 个），不在则按 §映射失败时的处理走

## 仓库名 / zerus service_name → 接入名映射（skill 联动用）

`analyze-log` 与 `create-zerus` 联动时，会从会话上下文拿到 `serviceInstanceList[].name`（等同于 zerus 里的 `service.name`，通常也等同于本地 `src/<repo>/` 目录名）。**大部分情况直接拼后缀即可**：

```
<service_name> + {env 后缀}
    env = test-gz → _log_test_gz
    env = hk      → _log_hk
```

例（test-gz）：
- `palmdiagnosis` → `palmdiagnosis_log_test_gz` ✅
- `wecardpayapp` → `wecardpayapp_log_test_gz` ✅
- `apigateway` → `apigateway_log_test_gz` ✅

例（hk）：
- `palmdiagnosis` → `palmdiagnosis_log_hk` ✅
- `wecardpayapp` → `wecardpayapp_log_hk` ✅
- `apigateway` → `apigateway_log_hk` ✅

### 例外 / 特殊映射

| service_name（zerus / 仓库） | test-gz 接入名 | hk 接入名 | 说明 |
|---|---|---|---|
| `palm_pay_fe` | `palm_pay_fe_log_test_gz` | `palm_pay_fe_log_hk` | 前端，带下划线原样保留，两环境都有 |
| `palm_local_h5` | `palm_local_h5_log_test_gz` | ⚠️ **不存在** | 本地 H5 仅 test-gz |
| `palm_local` | ⚠️ **未在接入名清单中** | ⚠️ **未在接入名清单中** | 可能是纯前端仓库，无独立日志接入，两环境都没有 |
| `wecardactivationapp` | `wecardactivationapp_log_test_gz` | ⚠️ **不存在** | wecard 激活仅 test-gz |
| 任何 `apm_*` | ⚠️ **不映射**（`apm_15692_test_gz_677a_trace` 无数据，禁用） | → `apm_15692_hk_b2bd_trace` | test-gz 侧 trace 检索直接用业务接入名的 `trace_id` 字段；HK 侧 APM 有数据可并入 |

### 映射失败时的处理

如果 `service_name + {env 后缀}` 拼出来的接入名**不在对应环境的清单里**，走以下路径：

1. **先跑 MCP 复核**：`GetDataflowListByProjectId(projectId=15692, environment=<env>)` 拉当前环境最新清单——服务可能新上线未同步到本文档
2. **仍找不到**：向用户提示"服务 `<name>` 在 `<env>` 环境无独立日志接入，可能是前端 / 数据面 / 未接入 zhiyan-log 的组件"，让用户改用其他上下文（如 `apigateway_log_<env>` 从网关侧查上下游）
3. **绝不静默切环境**：hk 找不到不去 test-gz 猜，反之亦然
4. **多个仓库联动时**：能映射成功的走 L1 查询，不能映射的**跳过并告知用户**，不要静默丢弃

**联动约定：`create-zerus` 产出的 `serviceInstanceList[].name` 是本 skill 上下文的一级输入，映射失败要显式打印，不要静默 fallback 到全项目查询或跨环境查询。**
