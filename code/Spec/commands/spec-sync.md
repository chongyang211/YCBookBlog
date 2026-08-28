# Command: spec-sync

## 命令名称

`spec-sync`

## 作用

检查 spec、代码实现、测试、文档之间的一致性，识别不同步的地方并给出修复建议。

## 输入参数

| 参数 | 必选 | 说明 |
|------|------|------|
| spec 路径 | 否 | 指定检查某个 spec；不指定则检查所有 |
| 范围 | 否 | `all`（默认）/ `spec-code` / `spec-test` / `spec-doc` |

**触发方式**：

```
请执行 spec-sync
```

或：

```
请对 specs/v1.6.0/0-example-feature.md 执行 spec-sync
```

## 执行步骤

0. **前置校验 — Story ID + 版本目录沿用与一致性**（强制）
   - 对于命令指定的（或全部）spec，逐个解析文件名中的 Story ID 与路径中的 `<VERSION>`
   - 检查项：
     - spec 文件名 STORYID 段 == spec frontmatter `Story ID` 字段
     - spec 与对应 plan / tasks 三件套的 STORYID 段全部一致
     - **spec / plan / tasks 三件套位于同一 `<VERSION>` 版本目录**（跨版本即视为不同步）
     - spec frontmatter 字段完整（`Story ID` / `Status` / `Author` 或 `Owner` / `Updated`）
   - 任何一项不一致 → 在同步报告中列为「不同步项 - 高优先级」，建议人工修正

1. **偏离记录回流检查**（最高优先级）
   - 扫描 `tasks/<VERSION>/<STORYID>-*-tasks.md` 的「偏离记录」表格
   - 列出所有 status 不在 `{spec-updated, plan-updated, accepted-as-is, rejected, escalated}` 的偏离条目
   - 对每条 `open` / `pending-decision` 偏离，提示用户：
     - 该偏离应该回流到 spec 还是 plan？
     - 该偏离是接受、拒绝、还是升级？
   - 用户决策后，由 AI 协助回流到 spec / plan，并更新 tasks 状态
   - 详细流程见 `rules/10-spec-workflow.md` 阶段三补

2. **Spec ↔ 代码同步检查**
   - spec 中定义的功能需求是否都有对应实现
   - 代码中是否有 spec 未描述的行为（潜在偏离）
   - spec 的状态字段是否准确

3. **Spec ↔ 测试同步检查**
   - spec 中的验收标准是否都有对应测试
   - 测试是否还在测试已被修改或删除的行为

4. **Spec ↔ Plan ↔ Tasks 三件套一致性**
   - plan 中的改动文件清单与代码实际是否一致
   - tasks 中所有 task 是否已勾选 `[x]`
   - tasks 的 DoD 是否全部满足
   - spec 验收标准的 checkbox 是否全部勾选

5. **Spec ↔ 文档同步检查**
   - spec 的 status 是否与实际情况一致（实施完应为 `implemented`）
   - spec 的「修订记录」与 tasks 偏离记录是否对应
   - spec frontmatter 字段是否完整（`Story ID` / `Status` / `Author` 或 `Owner` / `Updated`）
   - `docs/` 中是否有过时的描述

6. **代码 ↔ 测试同步检查**
   - 新增的公共接口是否有测试
   - 已删除的接口对应的测试是否需要清理

7. **输出同步报告**

## 输出结果

```markdown
### Sync Report

**检查范围**：[全部 / 指定 spec]
**检查时间**：[日期]

**同步状态**：

| Spec | 偏离回流 | 代码 | 测试 | 三件套一致 | 文档 | 状态 |
|------|---------|------|------|-----------|------|------|
| 0001 | ✅ 全部 closed | ✅ | ⚠️ 缺 2 个测试 | ✅ | ✅ | 部分同步 |

**待回流偏离**（最高优先级）：

| Tasks 偏离记录 | 严重度 | 当前状态 | 建议回流目标 |
|---------------|-------|---------|------------|
| T2.3 偏离 (2026-06-08) | 中 | open | spec FR-3 章节 |

**不同步项**：

1. **[Story ID] - [问题描述]**
   - 现状：[当前情况]
   - 建议：[修复方式]

**总结**：
- N 个 spec 已完全同步
- M 个 spec 存在不同步项
- 建议优先处理：[列表]
```

## 适用时机

- 定期检查项目一致性（如每周或每个迭代结束）
- 完成一批功能后做全面检查
- 怀疑某些文档或测试已过时
- 新成员加入前检查项目状态

## 关联规则与技能

- **规则**：`rules/40-documentation-rules.md`、`rules/10-spec-workflow.md`
- **技能**：`skills/spec-analysis.md`、`skills/change-summary.md`
