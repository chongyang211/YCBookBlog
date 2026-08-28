# Command: spec-plan

## 命令名称

`spec-plan`

## 作用

根据指定的 spec 文件，产出一份可执行的实施计划。这是从"需求理解"到"代码实现"的关键桥梁。

## 输入参数

| 参数 | 必选 | 说明 |
|------|------|------|
| spec 路径 | 是 | `specs/` 中的 spec 文件路径 |
| 约束说明 | 否 | 额外的技术约束或偏好 |

**触发方式**：

```
请根据 specs/v1.6.0/0-example-feature.md 执行 spec-plan
```

## 执行步骤

0. **前置校验 — Story ID + 版本目录沿用**（强制）
   - 从 spec 文件名解析 Story ID：路径 `specs/<VERSION>/<STORYID>-<slug>.md` 中的 `<STORYID>` 段
   - **解析 `<VERSION>`（路径中间段），plan 写入同名版本目录 `plans/<VERSION>/`；目录不存在则先 `mkdir -p` 再写入**
   - 如 spec 路径缺少版本目录段 → 主动询问本次归属版本（如 `v1.6.0`）
   - 校验 spec frontmatter 的 `Story ID` 字段与文件名一致；不一致 → **报错并停止**，要求人工修正
   - 后续生成的 plan 文件名沿用同一 Story ID（不重新询问、不另起编号）
   - 旧版无 STORYID 段的 spec（如 `0-example-feature.md`）→ 视为 Story ID = `0`

1. **读取 spec**
   - 打开指定的 spec 文件，完整阅读
   - 参考 `skills/spec-analysis.md` 进行分析

2. **检查 spec 完整性**
   - 如果有 `[TBD]` 项或缺失章节，先列出问题
   - 重大缺失则停下等待补充，小缺失可标注后继续

3. **扫描现有代码**
   - 了解 `src/` 中与 spec 相关的现有代码
   - 识别需要新增和修改的文件

4. **产出实施计划**
   - 参考 `skills/implementation-planning.md` 的输出格式
   - 列出改动范围、步骤、依赖、风险

5. **等待确认**
   - 输出 plan 后等待人的确认
   - 根据反馈调整 plan
   - 确认后将 plan **写入** `plans/<VERSION>/<STORYID>-<slug>-plan.md`（沿用 Step 0 解析的 STORYID）

## 输出结果

- 一份磁盘文件：`plans/<VERSION>/<STORYID>-<slug>-plan.md`（结构见 `plans/templates/plan-template.md`）
- 简要总结：Phase 数量、改动文件数、关键风险与决策

## 后续动作

- 复杂改动：执行 `spec-tasks` 拆解为可追踪任务清单
- 简单改动：直接执行 `spec-implement`

## 适用时机

- 开始实现一个新 spec 之前
- 需要评估一个 spec 的工作量
- 需要将大 spec 拆分为可执行步骤

## 关联规则与技能

- **规则**：`rules/10-spec-workflow.md`（阶段一和阶段二）
- **技能**：`skills/spec-analysis.md`、`skills/implementation-planning.md`
