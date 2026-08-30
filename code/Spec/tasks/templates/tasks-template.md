# Tasks — [简短标题]

> **关联 Spec**: `specs/<VERSION>/<STORYID>-<slug>.md`  
> **关联 Plan**: `plans/<VERSION>/<STORYID>-<slug>-plan.md`  
> **Story ID**: [与 spec 一致的需求单号，如 10086]  
> **Owner**: [负责人]  
> **Created**: YYYY-MM-DD  
> **Updated**: YYYY-MM-DD  

> 📁 **文件命名规范**：`tasks/<VERSION>/<STORYID>-<slug>-tasks.md`  
> 例：`tasks/v1.6.0/10088-payment-retry-tasks.md`

## 进度概览

- [ ] Phase 1: [阶段名]（X/Y）
- [ ] Phase 2: [阶段名]（X/Y）
- [ ] Phase 3: [阶段名]（X/Y）

## 任务清单

### Phase 1: [例如：Proto 定义]

- [ ] **T1.1** [任务标题] — `path/to/file.ext` — 关联 FR-1
- [ ] **T1.2** [任务标题] — `path/to/file.ext` — 关联 FR-1

### Phase 2: [例如：核心实现]

- [ ] **T2.1** [任务标题] — `path/to/file.ext` — 关联 FR-2
- [ ] **T2.2** [任务标题] — `path/to/file.ext` — 关联 FR-3

### Phase 3: [例如：测试与联调]

- [ ] **T3.1** [测试标题] — `tests/xxx_test.go` — 关联验收标准 AC-X
- [ ] **T3.2** [集成测试] — 关联验收标准 AC-Y

## 验收标准映射

> 每条 spec 验收标准至少对应一条 task。

| Spec 验收标准 | 对应 Task |
|--------------|----------|
| AC-1 [标准描述] | T1.1, T2.1 |
| AC-2 [标准描述] | T2.2 |
| AC-3 [标准描述] | T3.1 |

## 偏离记录

> 实施过程中如果偏离了 spec 或 plan，**必须**在此处记录，不允许沉默偏离。  
> 处理流程见 `rules/10-spec-workflow.md` 阶段三补「偏离与回流」。

| 日期 | 偏离点 | 原因 | 严重度 | 状态 | 处理建议 |
|------|--------|------|-------|------|---------|
| YYYY-MM-DD | [偏离描述] | [为什么] | 小/中/大 | open | 更新 spec / 更新 plan / 接受现状 |

**status 枚举**：
- `open` — 刚记录，未决策
- `pending-decision` — 暂停 task，等 Spec Author 决策
- `spec-updated` — 已回流到 spec（spec「修订记录」已更新）
- `plan-updated` — 已回流到 plan（spec 不变）
- `accepted-as-is` — 决策为接受偏离，不改 spec/plan
- `rejected` — 决策为按原 spec 实施
- `escalated` — 已升级为新 spec 或 spec 重做

> ⚠️ **PR 合并前**：所有偏离记录必须不为 `open` / `pending-decision`，否则 `/spec-sync` 会报错。

## 阻塞记录

| 日期 | 阻塞内容 | 等待对象 | 状态 |
|------|---------|---------|------|
| YYYY-MM-DD | [阻塞描述] | [谁/什么] | open / resolved |

## 完成定义（DoD）

- [ ] 所有 task 已勾选
- [ ] spec 中所有验收标准 checkbox 已勾选
- [ ] 单元测试已编写并 PASS
- [ ] 变更摘要已输出
- [ ] spec 状态已更新为 `implemented`
- [ ] PR 已关联本 tasks 文件
