# Command: spec-implement

## 命令名称

`spec-implement`

## 作用

根据已确认的 spec 和 implementation plan，在 `src/` 中执行代码实现。

## 输入参数

| 参数 | 必选 | 说明 |
|------|------|------|
| spec 路径 | 是 | 对应的 spec 文件路径 |
| plan 确认 | 是 | 需要先有已确认的 implementation plan |

**触发方式**：

```
确认 plan，请执行 spec-implement
```

或：

```
请根据 specs/v1.6.0/0-example-feature.md 执行 spec-implement
```

## 执行步骤

0. **前置校验 — Story ID + 版本目录沿用**（强制）
   - 从 spec 文件名解析 Story ID：路径 `specs/<VERSION>/<STORYID>-<slug>.md`
   - **解析 `<VERSION>`，定位同版本目录下的 `plans/<VERSION>/`、`tasks/<VERSION>/`；测试产物写入 `tests/`**
   - 校验 spec frontmatter `Story ID` 字段与文件名一致；不一致 → **报错并停止**
   - 校验 plan / tasks 文件名 STORYID 段与 spec 一致
   - 旧版无 STORYID 段的 spec → 视为 Story ID = `0`

0.5. **前置 Git 准备**（强制）

   每个 plan「涉及仓库」表中标 ✅ 需要改动的仓库，执行：

   ```bash
   # 1. 切到基线分支
   git checkout <baseline>

   # 2. rebase 模式同步最新（避免 merge commit 污染）
   git pull -r origin <baseline>

   # 3. 从基线分支拉出 feature 分支
   git checkout -b {feature|hotfix}/<spec-name>
   ```

   - **基线分支**：从 plan「涉及仓库」表的「基线分支」列读取（详见 `docs/git-workflow.md` 第 1 章）
   - **分支前缀**：默认 `feature/`；紧急 bug 修复用 `hotfix/`（须显式声明）
   - **分支名**：spec 文件名去 `.md` 后缀，如 `134389917-dashboard-online-palm-statistics`
   - **多仓库**：所有相关仓库使用**同一分支名**
   - 如 feature 分支已存在（多人协作 / 恢复实施）→ 切到该分支即可，不重新创建
   - 拉分支前**必须**确认 working tree 干净，避免污染

1. **确认前置条件**
   - 确认有对应的 spec 和已确认的 plan（`plans/<VERSION>/<STORYID>-<slug>-plan.md` 文件存在）
   - 如果 plan 不存在，提示先执行 `spec-plan`
   - 复杂改动应已有 `tasks/<VERSION>/<STORYID>-<slug>-tasks.md`；如果没有，提示先执行 `spec-tasks`
   - 确认所有相关仓库都位于正确的 feature 分支（Step 0.5 已切换）

2. **按 plan + tasks 逐步实施**
   - 参考 `skills/feature-implementation.md` 的执行方式
   - 每步先读取文件、再修改、再确认
   - 严格按 plan 范围执行
   - **每完成一条 task，立即在 tasks 文件中勾选 `[x]`**（不允许积攒到最后批量更新）

3. **实施过程中的检查**
   - 每步完成后简要报告
   - 发现问题时及时说明
   - 不擅自扩大改动范围
   - 偏离 plan/spec → 必须记录到 tasks 文件的「偏离记录」章节

4. **完成后进入下一阶段**
   - 输出实施完成确认（含 tasks 完成情况）
   - 建议执行 `spec-test`（如尚未做）→ `spec-review` → `spec-push`
   - **不要**在本阶段直接 push；最终 push 由 `/spec-push` 统一处理（含安全 rebase）

## 输出结果

- 每步的完成报告
- 全部完成后的汇总：修改了哪些文件、新增了哪些文件
- 建议进入 `spec-test` 阶段

## 适用时机

- plan 已确认，准备开始写代码
- 上一次实施被中断，需要继续

## 关联规则与技能

- **规则**：`rules/10-spec-workflow.md`（阶段三）、`rules/20-coding-rules.md`
- **技能**：`skills/feature-implementation.md`
