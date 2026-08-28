# Implementation Plan — [简短标题]

> **关联 Spec**: `specs/<VERSION>/<STORYID>-<slug>.md`  
> **Story ID**: [与 spec 一致的需求单号，如 134029304]  
> **Author**: [作者]  
> **Created**: YYYY-MM-DD  
> **Updated**: YYYY-MM-DD  
> **Status**: draft | confirmed | obsolete  

> 📁 **文件命名规范**：`plans/<VERSION>/<STORYID>-<slug>-plan.md`  
> 例：`plans/v1.6.0/134029305-payment-retry-plan.md`

## 设计概述

> 2-5 句话描述总体方案。回答："为了满足 spec，我们打算怎么做？"

## 改动范围

### 涉及仓库

> 所有涉及仓库**必须使用同一分支名**：`{feature|hotfix}/{spec-name}`  
> 完整 git 工作流见 `docs/git-workflow.md`

| 仓库 | 是否需要改动 | 基线分支 | feature 分支 |
|------|------------|---------|------------|
| [repo-a] | ✅ | `develop` | `feature/<spec-name>` |
| [repo-b] | ❌（仅引用） | `master` | — |
| [repo-c] | ✅ | `master` | `feature/<spec-name>` |

### 文件改动清单

| 操作 | 文件路径 | 关联 FR | 说明 |
|------|---------|---------|------|
| 新增 | `path/to/new.ext` | FR-1 | [做什么] |
| 修改 | `path/to/edit.ext` | FR-2 | [改什么] |
| 删除 | `path/to/old.ext` | — | [为什么删] |

## 实施步骤（按阶段）

### Phase 1: [阶段名]

**目标**：[本阶段产出什么]

1. **Step 1.1**: [步骤描述]
   - 文件：`path/to/file.ext`
   - 原因：[为什么先做这步]
   - 关键代码模式 / 参考点：[已有代码参考路径]

2. **Step 1.2**: [步骤描述]
   - ...

### Phase 2: [阶段名]

...

## 依赖关系

- Step 1.2 依赖 Step 1.1 的 [具体产物]
- Phase 2 依赖 Phase 1 全部完成

## 风险与应对

| 风险 | 概率 | 影响 | 应对方式 |
|------|------|------|---------|
| [风险 1] | 中 | 高 | [缓解措施] |
| [风险 2] | 低 | 中 | [缓解措施] |

## 关键技术决策

> 记录在 plan 阶段做的取舍，避免后续重复讨论。

- **决策 1**: [选了什么] — 理由：[为什么]，淘汰方案：[淘汰的替代方案]
- **决策 2**: ...

## 待确认项

> 本 plan 暂时无法决策、需要人/架构师确认的点。

- [ ] [待确认 1]
- [ ] [待确认 2]

## 预估复杂度

**复杂度**：低 / 中 / 高  
**预估工作量**：[人天 / story point]  
**建议拆 tasks**：是 / 否（小改动可省略 tasks/）

## 关键代码参考

| 要做什么 | 参考文件 |
|---------|---------|
| [模式 1] | `path/to/reference.ext` |
| [模式 2] | `path/to/reference.ext` |
