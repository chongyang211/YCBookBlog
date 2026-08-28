# 代码层经验记忆（业务逻辑 / 技术方案 / 代码实现）

> ⭐ **首要经验库**。这些 Pattern 直接影响方案最终落地的**准确性和合理性**。  
> 使用方式见 [`README.md`](./README.md)。

---

## 检索使用说明

**AI 在 Step 4.5（代码验证评审）时必须先读完本文件**，然后逐个 Pattern 对当前 MR 的 spec/plan 扫描。命中即写评论并标注 `命中 code-patterns#N`。

**分类索引**：

- 🔧 **命名/引用类**：#1, #3
- 🏛 **抽象/分层类**：#2, #7, #8
- 🔗 **协议/兼容类**：#4
- ⚡ **性能/连接类**：#5, #6
- 🛡 **时效/安全/幂等类**：#9, #10
- 📦 **配置/默认值类**：#12
- 🧹 **代码卫生类**：#13

---

## Pattern 1: 引用了不存在/名字错的代码位置

**首次沉淀**: 2026-07-15，来源 MR [palm/palmpay/CoSpec!77](https://git.woa.com/palm/palmpay/CoSpec/-/merge_requests/77)

**表现**：spec 说"复用 `xxx.go` L45 的 `YYY()`"，但真实文件里没有 `YYY`；或大小写不对；或常量少字多字（如 `O4PalmAppId` vs 真实 `O4PalmAppAppId`）；或三份文档（spec/plan/tasks）对同一个东西用了不同拼写。

**验证动作**：对 spec 里出现 ≥ 2 次的每个类/方法/常量名，全仓 `grep -rn` 一次，比对真实拼写。

**为什么严重**：dev 照抄编译不过；三份文档命名不一致会让 code review 反复扯皮。

**建议方向**：spec 全文替换为真实拼写；附带发现"重复常量定义"时，提议本次顺手抽公共常量。

---

## Pattern 2: 未复用现有天然抽象，在错误层次硬编码

**首次沉淀**: 2026-07-15，来源 MR palm/palmpay/CoSpec!77（#16）

**表现**：spec 说"在 application 层加 `if instructionName == 'xxx'` 校验"——但 domain 层其实已经有 `checkXxxDevice(config, device)` 这种为该场景设计的 hook 函数，同时拿到 config + device 上下文。

**验证动作**：读 spec 涉及的 domain 实体（如 `Instruction`、`Device`），列出所有 `check*` / `validate*` / `is*` 方法；对每个方法判断"它是否已经拿到了新校验所需的上下文"——若是 → 天然落点。

**为什么严重**：错落点 = 破坏分层 + 需要在单条/批量两处重复写 + 未来其他类似需求还得再写一遍 + 违反自我声明的"单一落点"约束。

**建议方向**：下沉到已有 hook；避免"single & batch 两处重复"；保留现有 `checkFailList` 逐条 fail 语义，不引入"整体拒绝"新语义。

---

## Pattern 3: 新增方法与同族方法命名/可见性不一致

**首次沉淀**: 2026-07-15，来源 MR palm/palmpay/CoSpec!77（#17）

**表现**：现有 `isO1Device()`/`isO2Device()` 都私有（首字母小写），spec 新增却是 `IsO4Device()` 大写导出；或 spec 用小写、plan/tasks 用大写，三份文档互相冲突；或需要跨包调用所以强行改大写，破坏封装。

**验证动作**：对新增方法的**同族方法**（同 receiver、同前缀）全仓 grep，判断大小写惯例；判断跨包调用需求是否是因为"落点选错了"（结合 Pattern 2）。

**建议方向**：保持一致；若确实需要跨包调用大写，先思考是否本该避免跨包（把逻辑下沉到 domain 层）；最优解常常是"根本不新增此方法"。

---

## Pattern 4: 兄弟 spec 引用同 message 字段号不一致

**首次沉淀**: 2026-07-15，来源 MR palm/palmpay/CoSpec!77（#1）

**表现**：一个 Story 拆多个 spec（后端 + 设备端），两份都引用同一个 proto message（如 `InstructionLinkMsg`），但字段号/类型/字段列表定义不同。

**验证动作**：
1. 从 frontmatter `Sibling Specs` 或 grep Story ID 找出所有兄弟 spec
2. 打开 **proto 源文件**（`src/proto/**.proto`），逐字段比对
3. 兄弟 spec 之间不互相比对——都要与 proto 源对齐

**为什么严重**：proto 字段号错位 = 反序列化必挂 = 端到端联调必翻车。

**建议方向**：spec 直接贴 proto 源文件的真实定义（或引用不复制）；两侧同步修订并在评审时 @ 兄弟 spec 作者。

---

## Pattern 5: "全量默认改造"影响面被低估

**首次沉淀**: 2026-07-15，来源 MR palm/palmpay/CoSpec!77（#3）

**表现**：spec 里一句"顺手对所有 X 都启用 Y"（如"所有指令都走 MQTT 推送"），描述短短一句，但影响面覆盖全部现有实例。plan 却把风险评为"低/低"。

**验证动作**：
- grep 现有实例总数（如现有指令 7 条？调用点 100 处？）
- 每个实例的**下游**是否都能承接新行为？（如"MQTT 推送对所有指令生效" → 各机型 palmapp 是否都实现了对应 handler？）
- 是否有幂等/去重机制防止双通道重复触发？

**建议方向**：先只对本次涉及的**单点**启用（feature flag / 白名单），全量放独立需求做；或至少补设备端 handler 覆盖表 + 幂等去重设计。

---

## Pattern 6: 每次调用都创建新连接（gRPC/HTTP client 反模式）

**首次沉淀**: 2026-07-15，来源 MR palm/palmpay/CoSpec!77（#3+#18 合并）

**表现**：spec 建议"复用现有 `GetXxxClient()` + `defer conn.Close()` 模式"——真实实现每次 `grpc.Dial` + 三次握手 + TLS。spec 结论"best-effort 无副作用"忽略了连接层成本。

**验证动作**：打开 `GetXxxClient()` 实现，看返回的是新建连接还是连接池；估算调用频次（本次改动会让它被调用几次每秒？）。

**建议方向**：
- 抽长连接单例 / 连接池（gRPC 官方推荐 `grpc.NewClient` 而非 `Dial` + `defer Close`）
- 或至少在评论里指出"高并发场景需要连接池化，全量方案不成立"

---

## Pattern 7: 私有 helper 跨包引用 = 反向分层依赖

**首次沉淀**: 2026-07-15，来源 MR palm/palmpay/CoSpec!77（#22）

**表现**：spec 说"引用 `controller/xxx/set_yyy.go` 的 `GetXxxClient`"——但调用方在 `application/aaa/service/` 层，从 application → controller 反向依赖。且该 helper 已在项目里被复制 2~3 次。

**验证动作**：
- 全仓 grep helper 名看副本数量（多份副本 = 抽象缺失信号）
- 判断调用方 vs 被引用方的包路径是否符合 DDD 分层（Gateway → Application → Domain → Repo；controller 之间不横向引用）

**建议方向**：抽到公共层（infrastructure / application/xxx/link/）改名合适前缀，顺便消除现有重复。写进 plan 的"文件改动清单"。

---

## Pattern 8: 新方法字段与已有方法 100% 相同 = 重造轮子

**首次沉淀**: 2026-07-15，来源 MR palm/palmpay/CoSpec!77（#21）

**表现**：spec 说"新增 `ToLinkDTO()` 方法，字段与 `ToDeviceDTO()` 一致"——两个方法字段完全相同，plan 却把整个字段列表重新写一遍并再调一次 `genSignature()`。

**验证动作**：打开两个 DTO/proto 定义**逐字段核对**；若字段 100% 相同，思考三种优化：
1. `ToLinkDTO` 内部调 `ToDeviceDTO` 拿 struct 再 `proto.Marshal`
2. 与 proto 侧讨论合并两个 message
3. 如果新 message 是历史预留但从未使用的，本次是清理时机

**建议方向**：复用而非并列；DRY 是硬约束——未来 `ToDeviceDTO` 加字段时不能容忍两处同步。

---

## Pattern 9: 强时效 vs 长过期 vs best-effort 三者组合失衡（物理安全）

**首次沉淀**: 2026-07-15，来源 MR palm/palmpay/CoSpec!77（#7+#19 合并）

**表现**：spec 承诺"秒级响应"（G5 目标），但组合了：
- `ExpireDuration=86400s`（24h，与通用指令一致）
- MQTT 推送 `best-effort 失败仅打日志`
- 短轮询兜底 60~300s

最坏路径 = 半夜下发的指令次日早上才被消费执行 → **物理副作用**（真的开门/扣款/删除）。

**验证动作**：
- 从 spec 找承诺的 SLA（秒级 / 分钟级 / 小时级）
- 从代码找**兜底路径**的实际耗时（轮询周期、超时脚本扫描周期）
- 计算最坏路径 = `ExpireDuration + 兜底扫描间隔`
- 判断 SLA vs 最坏路径的差距

**建议方向**（三管齐下）：
1. 强时效指令**单独设短过期**（60~120s，不复用通用 86400）
2. 关键路径失败应**同步返回**给管理端而非默默 best-effort
3. 涉及物理副作用必须有**幂等锁**（`InstructionNo` 单指令锁）防止 MQTT + 轮询双通道重复触发

---

## Pattern 10: 高危操作复用低危权限点位

**首次沉淀**: 2026-07-15，来源 MR palm/palmpay/CoSpec!77（#8）

**表现**：物理世界或不可撤销操作（远程开门/扣款/删除/factoryReset）直接复用现有通用的 `CheckXxxPermission`，与 `syncNtp`/`restartDevice` 等无损/可恢复操作共用一颗权限。

**验证动作**：
- 从 spec 判断操作的**不可撤销性**和**物理副作用**
- 从代码判断权限系统是否可细粒度配置（RBAC 独立 permission key、`XxxSafeConfig` 字段等）

**建议方向**：
- 独立权限点位（如 `distribute:remoteDoorOpen`）
- 或二次确认 / 双人复核 / 短期审批链
- 请安全评审签字
- 探索现有 `InstructionSafeConfig` 之类字段是否是承载点

---

## Pattern 12: 配置字段列表不完整

**首次沉淀**: 2026-07-15，来源 MR palm/palmpay/CoSpec!77（#25，未投但已识别）

**表现**：`NewXxxParams` 结构体真实有 18 个字段，spec 只覆盖 13 个；漏掉的字段中可能有**语义相关**的（如 `SafeConfig` 关联安全策略、`RateLimitAmount` 与 `RateLimitUnit` 成对出现、`ExtraConfig` 关联扩展点）。

**验证动作**：打开结构体定义列出**完整字段清单**；对每个字段判断"零值是否合理""是否与本次需求语义相关"。

**建议方向**：spec 补全所有字段（零值也显式写出）；重点探索安全/限流/扩展相关字段是否是承载业务策略的天然落点，别浪费。

---

## Pattern 13: 兜底/参考路径依赖的代码本身有历史坏味道

**首次沉淀**: 2026-07-15，来源 MR palm/palmpay/CoSpec!77（#26+#27，未投但已识别）

**表现**：spec 引用某个文件作为模式参考，但该文件其实不存在（可能已被重命名/合并），或该文件里包含大量长期注释掉的代码/半成品，或引用位置行号已过时。

**验证动作**：spec 每一个"参考 xxx 文件/函数/行号"引用都要**真实打开**确认；发现历史坏味道（长期注释代码、重复常量、废弃分支），一并提议清理。

**建议方向**：spec 修正引用；本次实施时**顺手清理**相关坏味道（走独立 commit），避免作者在实施时 append 到注释块中间。

---

## 维护记录

| 日期 | 变更 | 操作人 |
|------|-----|--------|
| 2026-07-15 | 初始版本，从 MR palm/palmpay/CoSpec!77 沉淀 11 个 Pattern（编号 1-10, 12, 13；11 移到 doc-process-patterns.md） | evanye |
