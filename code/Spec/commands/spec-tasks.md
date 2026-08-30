# Command: spec-tasks

## 命令名称

`spec-tasks`

## 作用

根据已确认的 plan，将实施步骤拆解为可勾选、可追踪、可交接的任务清单文件（`tasks/<VERSION>/<STORYID>-<slug>-tasks.md`）。这是 plan 与 implement 之间的关键桥梁，用于消除「不同人/不同 Agent 执行结果差异大」的问题。

## 输入参数

| 参数 | 必选 | 说明 |
|------|------|------|
| spec 路径 | 是 | 对应的 spec 文件路径 |
| plan 路径 | 是 | 已确认的 plan 文件路径（必须先存在） |

**触发方式**：

```
请根据 specs/v1.6.0/10086-example-user-login.md 执行 spec-tasks
```

或：

```
plan 已确认，请执行 spec-tasks
```

## 执行步骤

0. **前置校验 — Story ID + 版本目录沿用**（强制）
   - 从 spec 文件名解析 Story ID：路径 `specs/<VERSION>/<STORYID>-<slug>.md` 中的 `<STORYID>` 段
   - **解析 `<VERSION>`，tasks 写入 `tasks/<VERSION>/`；目录不存在则先 `mkdir -p` 再写入**
   - 校验 spec frontmatter 的 `Story ID` 字段与文件名一致；不一致 → **报错并停止**
   - 校验 plan 文件名 STORYID 段与 spec 一致；不一致 → 报错（三件套必须对齐）
   - 后续生成的 tasks 文件名沿用同一 Story ID

1. **确认前置条件**
   - 确认 `plans/<VERSION>/<STORYID>-<slug>-plan.md` 文件存在且 status 为 `confirmed`
   - 如果 plan 不存在，提示先执行 `spec-plan`
   - 如果 plan 还在 `draft`，提示先 review 并确认

2. **复制模板**
   - 以 `tasks/templates/tasks-template.md` 为基础
   - 命名为 `tasks/<VERSION>/<STORYID>-<slug>-tasks.md`（沿用 Step 0 解析的 NNNN/STORYID/slug）

3. **从 plan 拆解任务**
   - 把 plan 中的每个 Step 拆为单文件/单函数级颗粒度的 task
   - 每条 task 必须标注：
     - 任务编号（`T<Phase>.<序号>`，如 `T2.3`）
     - 任务标题（动词开头，简洁）
     - 关联文件路径
     - 关联 FR 或验收标准
   - Phase 划分与 plan 保持一致

4. **建立验收标准映射表**
   - 把 spec 中**每条**验收标准列入「验收标准映射」表
   - 映射到对应的 task 编号
   - 没有 task 覆盖的验收标准 → 必须补 task 或标注「中间件层处理」

5. **填写完成定义（DoD）**
   - 全部 task 已勾选
   - spec 验收标准 checkbox 已逐条勾选
   - 单元测试已编写并 PASS
   - 变更摘要已输出
   - spec 状态已更新为 `implemented`

6. **写入文件并提示**
   - 输出 task 总数、Phase 划分、关键关注点
   - 提示后续动作：`实施过程中按 task 编号实时勾选 [x]`

## 输出结果

- 一份 `tasks/<VERSION>/<STORYID>-<slug>-tasks.md` 文件（结构见 `tasks/templates/tasks-template.md`）
- 简要总结：Phase 数量、task 总数、覆盖的 FR 与 AC 数

## 何时可以跳过

- 单点 bugfix 且 plan 已经足够清晰（< 50 行代码改动）
- 纯文档/注释修改
- 单文件单函数级别的改动

跳过时必须在变更摘要中说明理由。

## 适用时机

- Plan 已确认，准备进入 implement 阶段
- 大型 spec 需要并行/交接执行
- 多人协作同一 spec，需要可见的进度追踪

## 关联规则与技能

- **规则**：`rules/10-spec-workflow.md`（阶段二点五）
- **模板**：`tasks/templates/tasks-template.md`
- **技能**：`skills/implementation-planning/SKILL.md`（task 拆解参考其拆步逻辑）
