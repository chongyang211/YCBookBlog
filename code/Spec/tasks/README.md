# Tasks 目录

## 作用

`tasks/` 是 **Spec 实施过程中的执行流水**——把 `plans/` 里的实施步骤拆成可勾选、可追踪、可交接的颗粒度任务。

| 文档类型 | 关注点 | 粒度 | 在工作流中的位置 |
|---------|--------|------|-----------------|
| `specs/` | What & Why | 业务功能 | 阶段 1 — 任务识别 |
| `plans/` | How（设计决策） | 模块/接口/风险 | 阶段 2 — 实施计划 |
| `tasks/` | Step-by-step（执行流水） | 单文件/单函数级 | 阶段 3 — 实施过程持续更新 |

## 命名规范

```
tasks/<VERSION>/<STORYID>-<slug>-tasks.md
```

> 📂 **版本目录层级**：tasks 文件按迭代版本归档到 `<VERSION>/` 子目录（如 `v1.6.0/`）；`templates/`、`README.md` 保留在 `tasks/` 根。

- `<VERSION>` + `STORYID` + `slug` 三段与对应 spec / plan **完全一致**，末尾加 `-tasks` 后缀
- `STORYID` 为纯数字需求单号；无对应 story 用 `0`

例：

```
specs/v1.6.0/134029304-online-palm-registration.md
plans/v1.6.0/134029304-online-palm-registration-plan.md
tasks/v1.6.0/134029304-online-palm-registration-tasks.md
```

## 何时创建

- **必须**：新功能、跨模块重构 — 在 plan 确认后、实施开始前创建
- **可选**：单点 bugfix、单文件改动 — 任务在 plan 中已经清晰即可不拆 tasks
- **不需要**：纯文档/注释修改

## 何时更新

`tasks/` 是**活文档**：

- 实施过程中**实时勾选** `[x]` 已完成项
- 发现新的子任务时**追加**到清单末尾
- 偏离 plan 时**记录**到「偏离记录」章节，并建议同步更新 spec/plan
- 任务完成 + 测试通过后，整体写入 spec 的「验收标准」勾选

## 与 Spec 验收标准的映射

每条 task 应可追溯到 spec 的某条 FR（功能需求）或验收标准。模板里有专门的映射表。

## 模板

`tasks/templates/tasks-template.md`
