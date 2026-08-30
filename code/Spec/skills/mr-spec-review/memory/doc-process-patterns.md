# 文档/流程层经验记忆（Doc & Process Patterns）

> 次要经验库。这些 Pattern 影响**协作效率与追溯性**，但不直接决定方案与代码的质量。  
> 首要经验见 [`code-patterns.md`](./code-patterns.md)。使用方式见 [`README.md`](./README.md)。

---

## 检索使用说明

**AI 在 Step 4.1（文档层评审）时先读完本文件**，然后逐个 Pattern 对当前 MR 的 spec/plan/tasks/design 扫描。命中即写评论并标注 `命中 doc-patterns#N`。

**注意**：这类问题**不能替代**代码层评审——即使 doc-patterns 命中 0 个，也不代表方案没问题，还需按 code-patterns.md 走第二轮。

---

## Pattern D1: `Sibling Specs` frontmatter 未列全

**首次沉淀**: 2026-07-15，来源 MR <ORG>/CoSpec!77（#2）

**表现**：同一 Story 拆多个 spec（后端 + 前端 + 设备端），但子 spec 的 frontmatter `Sibling Specs: —` 空着，或只列了其中一部分。

**验证动作**：grep 同 Story ID / 同业务关键词，找出仓库里所有关联 spec；对照 spec 的 `Sibling Specs` 字段是否列全。

**为什么关键**：违反 `rules/10-spec-workflow.md` 硬约束；影响后续 review/追溯断链。

**建议方向**：直接给出正确写法 `Sibling Specs: specs/vX.Y.Z/xxx.md, specs/vX.Y.Z/yyy.md`。

---

## Pattern D2: Status 与实际流程/文档不匹配

**首次沉淀**: 2026-07-15，来源 MR <ORG>/CoSpec!77（#5 + tasks 状态）

**表现**：
- spec `Status: ready` 但修订记录显示 ready 之后又新增了 FR（应回落 `in-progress` / `draft`）
- plan/design 仍 `draft` 但方案对比里已把新 FR 写死
- tasks 显示 Phase 1-3 全部 `[x] 已完成`，但代码根本没写（本 MR 只有文档）

**验证动作**：交叉核对 spec/plan/design/tasks 四份文档的 `Status`；核对修订记录最新日期 vs frontmatter `Updated`；核对 tasks 勾选状态 vs 是否有实施 commit。

**建议方向**：ready 后新增 FR 应回落状态；tasks 未实施时保持 `[ ]`。

---

## Pattern D3: 兄弟 spec / 三件套之间命名/字段/取值不一致

**首次沉淀**: 2026-07-15，来源 MR <ORG>/CoSpec!77（#20）

**表现**：spec/plan/tasks 三份文档对同一个东西命名/取值不一致（如 spec 前文 `ExampleAppId` 后文 `ExampleAppAppId`；spec 用小写 `isExampleDevice` plan 用大写 `IsExampleDevice`）。**这是 doc 层症状；根因见 code-patterns#1**。

**验证动作**：对每个高频出现（≥3 次）的类/方法/常量名，跨三份文档 grep 一遍，看拼写是否一致。

**建议方向**：先确认真实代码里的正确名字（走 code-patterns#1 的验证动作），再统一到三份文档。

---

## Pattern D4: TBD / FR / 修订记录编号跳号或缺失

**首次沉淀**: 2026-07-15，来源 MR <ORG>/CoSpec!77（#11）

**表现**：spec「风险与未决问题」表格只保留 `TBD-3~6`，`TBD-1/2` 编号缺失；或 FR 编号跳过；或修订记录条目在中间缺一条。

**验证动作**：扫编号连续性。

**建议方向**：补全缺失编号的描述，或删掉编号只留描述。轻微问题（🟡），但提示读者存在编辑历史遗留。

---

## Pattern D5: `Updated` 日期与修订记录最新日期不同步

**首次沉淀**: 2026-07-15，来源 MR <ORG>/CoSpec!77（#10）

**表现**：frontmatter `Updated: 2026-07-07` 与文档末尾修订记录 `2026-07-14 新增 FR-4` 不同步。

**验证动作**：对比 frontmatter `Updated` vs 修订记录最新一条日期。

**建议方向**：刷新到最新修订日期。轻微但影响追溯。

---

## Pattern D6: plan 「文件改动清单」缺配套注册

**首次沉淀**: 2026-07-15，来源 MR <ORG>/CoSpec!77（#9）

**表现**：新增错误码只列了「定义常量」这一行，忘了同步注册：
- 错误码 → HTTP status 映射
- 错误码 → 三语 msg 映射
- 集中的错误码枚举清单文件

类似地：新增 i18n key 只列了业务代码，忘了三语 json 文件；新增数据表字段只列了 struct，忘了 DDL。

**验证动作**：对每个"新增 XXX"改动，思考配套需要注册的地方（映射表、清单文件、i18n、DDL），核对 plan 是否列全。

**建议方向**：作者本地扫一下 `infrastructure/errcodes/` / `i18n/` / `sql/` 类目录结构，把配套注册也列入 plan 的文件改动清单；错误码取值需在实施前定死避免 magic name。

---

## Pattern D7: MR 内三件套（spec/plan/tasks）颗粒度不匹配

**首次沉淀**: 2026-07-15，来源 MR <ORG>/CoSpec!77

**表现**：
- spec 说"FR-4 全量指令 MQTT 推送"，plan 却只描述 `remoteDoorOpen` 的推送步骤（覆盖不全）
- 或 spec 完全没提某个改动，plan/tasks 却列了
- 或 plan 里的 Phase 与 spec 的 FR 不能一一对应

**验证动作**：把 spec 的 FR 列表与 plan 的 Phase / tasks 的 Phase 交叉映射，看是否 1:1（或明确的 M:N）。

**建议方向**：补充/裁剪，让三件套颗粒度对齐。

---

## Pattern D8: 未列历史 review 已解决/未解决状态汇总

**首次沉淀**: 待补充

**表现**：MR 有多轮 review 历史，本次修订未在 MR 描述或 commit message 里说明"针对第 N 轮哪些评论做了什么修改"，导致 reviewer 需要自己翻旧评论对照。

**建议方向**：作者在 commit message 里带 `--fix-review=MR!58` 之类的关联；或在 MR 描述里维护"修订汇总"表格。

---

## 维护记录

| 日期 | 变更 | 操作人 |
|------|-----|--------|
| 2026-07-15 | 初始版本，从 MR <ORG>/CoSpec!77 沉淀 8 个 doc/process Pattern（D1-D8） | evanye |
