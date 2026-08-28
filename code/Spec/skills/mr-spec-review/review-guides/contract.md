# 跨端协议契约 评审依据（Cross-End Contract Review Guide）

> **横切依据**：只要 MR 涉及**跨端联调的字段 / 接口 / 错误码 / proto 字段号**，无论主端是谁，都要额外加载本文件。
> 适用仓：`proto`（palm/weixin，含原识别侧 proto 逻辑）、`palm_proto`（palm/palmpay）；以及任意端改到 `*.proto`、gRPC/HTTP 接口、错误码、序列化结构。
> 加载时机：Step 4.0 端识别判定"涉及跨端契约"时，与主端 guide 一起加载。

---

## 1. 为什么单独一份契约依据

契约是**端与端之间的合同**，一处改错，端到端联调必挂，且往往到集成/上线才暴露，返工代价最大。本项目的高发事故就是"兄弟 spec 引用同一 message，字段号却对不上"（code-patterns#4）——**联调必翻车**。

两套 proto 与命名空间一一对应（改接口先改 proto）：

| proto 仓 | module | 服务对象 |
|----------|--------|----------|
| `proto` | `git.woa.com/palm/weixin/proto` | `device_manage` + `palm_global` + 识别侧（算法/palmrecognition/palmdevicegateway） |
| `palm_proto` | `git.woa.com/palm/palmpay/palm_proto` | `palm_local` |

> ⚠️ 识别侧 `privaterecognition` 的接口定义原在 `mmpay_palm_recognition/proto`，**现已合并进 `palm/weixin/proto`**（本地 `src/proto`）——核对识别接口时直接查看该目录。

stub **不在本地生成**：proto 仓提交 MR → 流水线 `protoc` 生成 Go stub 回提 → 业务仓 `go get -u` 更新。评审要确认 **proto 先行**。

---

## 2. 评审 Checklist（契约专业维度）

### 2.1 Proto 字段（🔴 高发区）
- [ ] **字段号只加不改不删**（wire 兼容的根本）；删字段要 `reserved` 号+名，防复用。
- [ ] 字段类型不变（int32↔int64、string↔bytes 都不兼容）。
- [ ] **兄弟 spec / 多端引用同一 message**：每一份都与 **proto 源文件逐字段核对**（字段号、类型、字段列表），兄弟之间不互相抄——都对齐 proto 源（对照 code-patterns#4）。
- [ ] 字段列表完整：spec 描述的 message 字段是否漏（如 18 个字段只写 13 个，漏的可能语义相关，对照 code-patterns#12）。
- [ ] enum：只加不改值；`0` 值语义安全（默认/未知）。
- [ ] `oneof` / `map` / `repeated` 变更的 wire 兼容性。

### 2.2 接口（gRPC / HTTP）
- [ ] 新增接口 vs 改旧接口：改旧接口的请求/响应结构要向后兼容（老调用方不崩）。
- [ ] HTTP（grpc-gateway）路由/method/字段映射与 proto 一致；query/path/body 映射清晰。
- [ ] 分页/游标契约：字段名、语义（游标有效期、页大小上限如特征下发 ≤50）跨端一致。
- [ ] 幂等契约：写接口的 `RequestId`/`InstructionNo` 语义、去重窗口两端一致（对照 code-patterns#9）。

### 2.3 错误码
- [ ] **只新增不复用/不改义**；分段位（如 403xx 鉴权、409xx 冲突），语义清晰。
- [ ] **跨服务对齐同一码值**：同一个错误在 device/backend/frontend 用同一 code，前端能据此给统一 i18n 提示。
- [ ] 新错误码在各端都有处理分支（后台返回、前端展示、设备端解析）。

### 2.4 兼容性与灰度
- [ ] 变更是否需要**双方同时发布**？若是，给出发布顺序（proto→后端→前端/设备）与灰度期。
- [ ] 面向**老设备**的契约变更：老固件长期不升级，兼容期至少覆盖一个 OTA 周期（对照 [`device.md`](./device.md) 4.1）。
- [ ] 序列化/反序列化：新旧版本互相收发不报错（forward/backward compatible）。

### 2.5 一致性核验（联动 spec 兄弟检查）
- [ ] `Sibling Specs` frontmatter 是否列全所有引用同一契约的兄弟 spec？未列 → 报问题。
- [ ] 同 Story 的后端 spec 与设备端/前端 spec，对同一字段/错误码/指令名的拼写、类型、取值**逐一比对**。

---

## 3. 契约高频坑

| 坑 | 症状 | 后果 | 对策 |
|----|------|------|------|
| 字段号错位 | 兄弟 spec 同 message 字段号不同 | 反序列化必挂 | 都对齐 proto 源 + @兄弟作者（code-patterns#4） |
| 改字段类型 | int32 改 int64 | wire 不兼容、脏数据 | 新增字段而非改类型 |
| 错误码复用改义 | 老 code 换新含义 | 各端误判 | 只新增，跨服务对齐码值 |
| 字段列表漏 | message 18 字段只描述 13 | 漏语义字段 | 逐字段核对补全（code-patterns#12） |
| stub 未先行 | 业务代码用了 proto 里还没有的字段 | 编译不过 | proto 先行 → go get -u |
| 单端发布 | 只发后端不管设备/前端 | 联调/线上不兼容 | 明确发布顺序 + 灰度期 |

---

## 4. 本项目真实 proto 约定与专项检查（带代码依据）

> 从两套 proto 真实代码提炼，**含已发现的历史不一致痕迹**。评审契约变更逐条对照；括号内为可 grep 的真实文件/符号。

### 4.1 proto 与包命名（不统一，是 import 冲突高发点）
- [ ] `go_package` 前缀不同：`palm/weixin/proto/*`（服务 device_manage+palm_global+识别侧）、`palm/palmpay/palm_proto/*`（palm_local）。改接口先定位到**正确的 proto 仓**。⚠️ 识别侧 proto 已合并进 `palm/weixin/proto`，但历史 `go_package` 可能仍带 `mmpay_palm_recognition/*` 前缀——改识别接口先 grep 确认实际前缀，别脑补。
- [ ] 🟠 **同目录包名混用**：`proto/device_manage/comm/` 下有 `package device_manage.comm;`、`package comm;`、`package deviceaccess;`、`package instruction;` 并存——新增 proto 要跟**同目录已有**包名，别再引入新风格加剧冲突。
- [ ] 🟠 **多个 package 复用同一 go_package**（如 palm_global 的 `palmcomm`/`palmdeviceinstruction`/`palmdevicemanage`/`palmupgrade` 都指向 `.../palm_global/comm`）——同一 Go 包内 message 重名会编译冲突，新增 message 全局查重名。
- [ ] `go_package` 路径规范：曾出现少写 `/proto`（`device_sync_feature.proto` 指向 `palm_global/controller/...` 而非 `proto/palm_global/...`）——新文件对齐同层规范。

### 4.2 字段号 / enum / oneof（本项目强约定）
- [ ] 🔴 **enum 0 值必须是 UNKNOWN/INVALID**（普遍遵守）：如 `INSTRUCTION_STATE_UNKNOWN=0`（`instruction.proto`）、`IMAGE_TYPE_INVALID=0`（`palmcomm.proto`）。新增 enum 别把有效值放 0。⚠️ 命名风格混用（A 全大写 `XXX_UNKNOWN` vs B PascalCase `Device_Key_State_UNKNOWN`）——跟所在 proto 的既有风格。
- [ ] **枚举值业务分段**要延续：状态机按 10 递增（`instruction.proto` `DISTRIBUTING=10…CANCELED=90`），操作符/支付态按 100 分段（`deviceaccess.proto` `SearchDeviceOperator`、`palm_proto/comm` 支付态）——新增值插进正确段位，别打乱。
- [ ] **reserved 预留是范本**：`SyncCursor`（`device_sync_feature.proto`）用 `reserved 6 to 9` + 注释预留未来字段号——**跨端同步游标类 message 必须保留 reserved 习惯**，删字段也要 `reserved` 号+名防复用。
- [ ] **oneof 从高号段起**（为未来类型留低号）：`DeviceAuth{oneof auth_info{SeAuth se_auth=100;}}`（`deviceaccess.proto`）、`Accessory{oneof model_info{...}}`（`palmcomm.proto`）——扩展认证/型号走 oneof + 高起始号。
- [ ] proto3 `optional`：`ResourceUsage`（`palmdiagnosis.proto`）注释明确"未设置 Go 端为 nil、JSON 序列化为 null"——用 optional 表达"可缺省"语义时，两端都要处理 nil/null。

### 4.3 设备网关 message（🔴 老设备兼容 + 命名一致性高发区）
- [ ] 🔴 **同一业务概念跨仓命名不一致**（真实痕迹）：指令关联消息在 `device_manage/comm` proto 叫 `InstructionLinkMsg`、设备侧 `CloudCmd.proto`（C++/Android）与 palm_global 侧又用 `Instruction`/`InstructionForDevice` 等——**兄弟 spec / 多端引用同一概念时，逐字段核对 proto 源**，别假设同名同结构（对照 code-patterns#4）。
- [ ] 设备网关（`palmdevicegateway`/`devicegateway`/`wecarddevicegateway`）的 message 变更是**老设备兼容重灾区**：字段号只加不改、老字段默认值兼容、错误码只加不改（强制走 [`device.md`](./device.md) §2.1 + SKILL.md 4.3）。

### 4.4 HTTP 映射（grpc-gateway `google.api.http`）
- [ ] RPC 用 `option (google.api.http)` 声明；写操作 `post + body:"*"`，读操作 `get` + 路径参数（`palmactivationgateway.proto` `/activation/devices/{device_sn}/...`）——新接口路径遵循现有 REST 风格 + 资源命名。
- [ ] `additional_bindings` 用于多路径兼容（`palmdevicegateway.proto`）——改路径别直接删老 binding，加 `additional_bindings` 保留旧路径过渡。

### 4.5 错误码与 stub 生成
- [ ] 错误码是**纯 Go `errcodes`**（`infrastructure/errcodes`、`palm_local/common/errcodes`），不在 proto 里——契约评审错误码时联动 [`backend.md`](./backend.md) §5.3（iota 顺序铁律：只加不改、不动顺序）。
- [ ] 🔴 **stub 由 proto 仓流水线 `protoc` 生成回提、业务仓 `go get -u` 更新**，本地不生成——评审确认 **proto MR 先行合入**，业务代码别抢跑用还没生成的字段（否则编译不过）。protoc 版本：palm/weixin·palm/palmpay 用 v5.27.1，识别侧 27.1。

---

## 5. 与各端 guide 的关系

契约是横切的，命中时**与主端 guide 叠加**使用：
- 后台改接口 → 本文件 + [`backend.md`](./backend.md)。
- 设备接口变更 → 本文件 + [`device.md`](./device.md) + SKILL.md 4.3（设备兼容性硬性）。
- SDK 公开协议 → 本文件 + [`mobile-sdk.md`](./mobile-sdk.md)。
- 识别接口 → 本文件 + [`algorithm.md`](./algorithm.md)。
- 前端对接字段/错误码 → 本文件 + [`frontend.md`](./frontend.md)。

---

## 🖊 人工追加评审注意点（Human-Curated Review Points）

> 团队架构/协议 owner 在此追加跨端契约的专业注意点，skill 涉及契约变更时**必须逐条加载检查**。格式见 [`README.md`](./README.md) 第 3 节。

<!-- HUMAN-CURATED:START -->
<!-- 在本行下方按格式追加条目，勿删除首尾两个标记 -->

<!-- HUMAN-CURATED:END -->
