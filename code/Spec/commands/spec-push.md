# Command: spec-push

## 命令名称

`spec-push`

## 作用

在阶段五（review）通过后，按团队 git 工作流标准执行 commit + 安全 rebase + push 动作，准备开 MR。

这是工作流的**收尾命令**，确保提交的代码满足：
- commit message 符合规范
- feature 分支已 rebase 到基线最新版本
- 多仓库 push 顺序正确（被依赖优先）

## 输入参数

| 参数 | 必选 | 说明 |
|------|------|------|
| spec 路径 | 是 | 对应的 spec 文件路径 |
| commit message | 否 | 不指定则 AI 根据变更摘要起草 |
| `#finish` 标识 | 否 | 默认加；如本 spec 不是 Story 的最后一笔则需显式说明不加 |

**触发方式**：

```
请根据 specs/v1.6.0/134389917-dashboard-online-palm-statistics.md 执行 spec-push
```

或：

```
review 通过，请执行 spec-push（commit msg：feat(palm): 首页面板新增空中录掌关联数据&数据可配置）
```

## 执行步骤

0. **前置校验 — Story ID + 版本目录沿用**（强制）
   - 从 spec 文件名解析 Story ID 与路径中的 `<VERSION>`
   - 校验 spec frontmatter `Story ID` 与文件名一致
   - **校验 spec / plan / tasks 三件套位于同一 `<VERSION>` 版本目录**
   - 后续提交、分支名沿用此 Story ID（分支名不含版本目录前缀）

1. **前置条件检查**
   - spec status 已可改为 `implemented`（验收通过、所有 task 勾选、tests 通过）
   - tasks 文件中所有偏离记录 status 不为 `open` / `pending-decision`
   - 各仓库当前都在 `{feature|hotfix}/<spec-name>` 分支
   - 各仓库 working tree 状态明确（已 stage 的文件 + 未追踪的新文件 都符合 plan 范围）

2. **本地 Commit**（每个改动仓库）
   - 用 `git status` 列出改动
   - 与 plan 「涉及仓库」表对照，确认无超范围改动
   - `git add` 相关文件（**不要** `git add .`，避免误提交无关文件）
   - 起草 commit message 并展示给用户确认：
     ```
     <type>(<scope>): <subject> --story=<STORYID> [#finish]
     ```
     - `type` 默认 `feat`（除非 spec 性质明显是 fix/refactor/docs 等）
     - `scope` 从 plan 涉及模块推断
     - `subject` 复用 spec 标题或 change-summary 输出
     - `--story=<STORYID>` 必填（从 spec frontmatter 取）
     - `#finish` 默认加（除非用户说明本 spec 不是 Story 的最后一笔）
   - 用户确认后执行 `git commit -m "..."`
   - 多仓库**逐个**处理，避免遗漏
   - 详细规范见 `docs/git-workflow.md` 第 2 章

3. **安全 Rebase**（每个改动仓库）
   ```bash
   git checkout <baseline>
   git pull -r origin <baseline>
   git checkout {feature|hotfix}/<spec-name>
   git rebase <baseline>
   ```
   - 如有冲突：
     - 暂停命令，提示用户解决冲突
     - 解决后必须**重新跑单元测试**（rebase 可能引入新的运行时不一致）
     - 测试通过后继续
   - 基线分支从 plan「涉及仓库」表读取

4. **Push**（每个改动仓库）
   - 首次 push：`git push -u origin {feature|hotfix}/<spec-name>`
   - 已存在远程分支（rebase 后）：`git push -f origin {feature|hotfix}/<spec-name>`
   - **多仓库 push 顺序**：按 plan 中标注的依赖关系排序，**被依赖优先**（如 proto → 业务代码）

5. **创建 MR 提示**
   - AI 输出每个仓库的 MR 创建链接 / 命令模板：
     ```
     # 工蜂 UI 创建 MR：
     # 源分支：feature/<spec-name>
     # 目标分支：<baseline>
     # 描述自动套用：.gitlab/merge_request_templates/Default.md
     ```
   - 提示用户：多仓库 MR 在描述中互相引用（`palm_proto!123`）
   - 提示用户：被依赖的仓库（如 proto）需**先合并**，业务仓库再 rebase + push

6. **更新 spec 状态**
   - 提示用户：MR 全部合并后，执行 `/spec-sync` 把 spec status 改为 `implemented`

## 输出结果

- 每个改动仓库的 commit 哈希、push 状态
- MR 创建提示（链接 / 模板）
- 多仓库合并顺序建议
- 后续动作清单（合并 → /spec-sync）

## 适用时机

- 阶段五 review 通过、所有验收标准已勾选
- spec 状态准备改为 `implemented`
- 多仓库 push 需统一节奏

## 不适用时机

- 实施过程中的中间 push（直接 `git push` 即可，不必走完整 rebase）
- spec 仍在 `in-progress`（先完成实施再 push）
- 偏离记录还有 `open` 项（先走 `/spec-sync` 偏离回流）

## 关联规则与技能

- **规则**：`rules/10-spec-workflow.md`（阶段六）
- **详细 Git 流程**：`docs/git-workflow.md`
- **MR 模板**：`.gitlab/merge_request_templates/Default.md`
- **技能**：`skills/change-summary/SKILL.md`（用于起草 commit subject）
