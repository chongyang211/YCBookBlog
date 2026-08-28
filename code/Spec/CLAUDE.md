# CLAUDE.md — Spec-driven AI 协作开发工作空间

## 项目概述

本项目是「刷掌支付」团队的 **Spec 驱动 AI 协作开发工作空间**，统一管理需求规格、实施计划、任务清单与 AI 协作规则。所有开发工作以 Spec 文件为单一事实来源，遵循「(Design) → Draft → Spec → Plan → Tasks → Implement → Test → Review → Push → Sync」全链路工作流。其中 Design（技术方案）是**可选前置环节**，仅在需求较大、需要先评审技术方案时使用。

**核心理念**：Spec / Plan / Tasks 必须沉淀为磁盘文件，不允许只在对话中存在；Story ID 是全程关联线索；分支命名跨仓库严格统一为 `{feature|hotfix}/<spec-name>`。

## 当前活跃 Spec

> 完整索引在 iWiki：<https://iwiki.woa.com/p/4022732388>。本地不再维护聚合的 `INDEX.md` 文件，避免 MR 冲突。
> iWiki 索引由带外命令 `/spec-index` 按需完整覆盖发布，**不在个人开发流程中执行**——个人分支只是部分进度，集体发布交由专人/工具。新需求请走 `/spec-draft` 命令起草。

## 规则引用

以下规则文件通过 `@` 显式导入（Claude Code 加载 CLAUDE.md 时一并加载）：

@rules/00-project-principles.md
@rules/10-spec-workflow.md
@rules/20-coding-rules.md
@rules/30-testing-rules.md
@rules/40-documentation-rules.md

> 维护提示：
> - 规则文件统一放在仓库根目录的 `rules/`，是单一事实来源。
> - Claude Code 通过 `@rules/...` 显式导入加载。
> - CodeBuddy 通过 `.codebuddy/rules/rules/`（软链）自动递归加载，无需 `@` 导入语法。

## Skills 引用

| 技能 | 路径 | 用途 |
|------|------|------|
| 技术方案 | `skills/technical-design/SKILL.md` | 需求大时，起草 spec 前先产出供评审的技术方案 |
| Spec 起草 | `skills/spec-drafting/SKILL.md` | 把原始需求转成 spec 草稿 |
| Spec 分析 | `skills/spec-analysis/SKILL.md` | 分析已有 spec 完整性 |
| MR Spec 评审 | `skills/mr-spec-review/SKILL.md` | 评审他人提交的 spec/plan/tasks MR，可联合代码评审，结论投递为 MR 评论 |
| MR 评审意见处理 | `skills/mr-review-resolve/SKILL.md` | 作者侧：拉取 MR 评论 → 与用户逐条评估处置 → 可选 AI 自动修复 → 回写评论标 resolved |
| 代码侦察 | `skills/codebase-survey/SKILL.md` | 扫描现有代码，识别相关模块 / 可复用资产 / 参考模式（被 spec-drafting 与 implementation-planning 调用） |
| 实施计划 | `skills/implementation-planning/SKILL.md` | 制定实施计划 |
| 功能实现 | `skills/feature-implementation/SKILL.md` | 执行代码实现 |
| 测试编写 | `skills/test-writing/SKILL.md` | 编写测试用例 |
| 变更摘要 | `skills/change-summary/SKILL.md` | 生成变更摘要（被 `/spec-review`、`/spec-push`、`/spec-sync` 内部调用，不单独出命令） |

## 命令清单（10 个工作流命令）

| 命令 | 阶段 | 输入 | 输出 |
|------|------|------|------|
| `/spec-intake` | 0 前置 — Intake（可选） | PM 需求文档路径 或 对话描述 | `docs/intake/<VERSION>/<STORYID>-<slug>.md`（原始需求草稿） |
| `/spec-design` | 0 前 — Design（可选） | `docs/intake/<VERSION>/<id>-<slug>.md`（需求大时） | `designs/<VERSION>/<STORYID>-<slug>-design.md`（status: draft → approved） |
| `/spec-draft` | 0 — Draft | `docs/intake/<VERSION>/<id>-<slug>.md` 或描述（+ 可选 design） | `specs/<VERSION>/<STORYID>-<slug>.md`（status: draft） |
| `/spec-plan` | 2 — Plan | spec 路径 | `plans/<VERSION>/<STORYID>-<slug>-plan.md` |
| `/spec-tasks` | 2.5 — Tasks | spec + plan 路径 | `tasks/<VERSION>/<STORYID>-<slug>-tasks.md` |
| `/spec-implement` | 3 — Implement | spec 路径 | feature 分支 + `src/` 改动 + tasks 实时勾选 |
| `/spec-test` | 4 — Test | spec 路径 | `tests/` 测试文件 |
| `/spec-review` | 5 — Review | spec 路径 | review 报告（含变更摘要） |
| `/spec-push`（可选） | 6 — Push | spec 路径（+ 可选 commit msg） | commit + 安全 rebase + push -f + MR 提示（视个人提交习惯，也可自行 git 提交） |
| `/spec-sync` | 6 后 — Sync | spec 路径或 `all` | spec 状态同步 |

> **带外工具**：`/spec-index` 扫描 `specs/` 生成索引并完整覆盖同步到 iWiki（<https://iwiki.woa.com/p/4022732388>）。它**不属于个人开发流程**，由专人/工具按需运行；个人分支上不要执行，避免把半成品状态覆盖到 iWiki。

## 工作流产物三件套

| 类型 | 目录 | 模板 |
|------|------|------|
| Spec（要做什么） | `specs/` | `specs/templates/spec-template.md` |
| Plan（怎么做） | `plans/` | `plans/templates/plan-template.md` |
| Tasks（步步执行） | `tasks/` | `tasks/templates/tasks-template.md` |

> Spec → Plan → Tasks 必须**沉淀为文件**，不允许只在对话中存在。详见 `rules/10-spec-workflow.md`。
>
> 此外，需求较大时可在 Spec 之前先产出**技术方案**（`designs/<VERSION>/<STORYID>-<slug>-design.md`，模板 `designs/templates/design-template.md`），评审通过后再起草 spec。它是可选前置产物，不属于必备三件套。
>
> 📂 **版本目录层级（强制）**：intake / designs / specs / plans / tasks 五类产物均按迭代版本归档到 `<VERSION>/` 子目录（如 `v1.6.0/`）；`templates/` / `README.md` 是跨版本元文件，留在各目录根。聚合索引由 `/spec-index` 发布到 iWiki，仓库内不再维护 `INDEX.md`。

## Git 工作流

- **分支命名**：所有相关仓库一律用 `{feature|hotfix}/<spec-name>`，跨仓库一致
- **Commit Message**：`<type>(<scope>): <subject> --story=<STORYID> [#finish]`（`#finish` 仅加在 Story 最后一笔）
- **Push 前必须安全 rebase**：基线 `git pull -r` → feature `git rebase 基线` → `git push -f`
- **MR 模板**：`.gitlab/merge_request_templates/Default.md`（三段式：关联 Spec / 偏离说明 / 变更摘要）
- **基线分支映射 + 完整流程**：详见 [`docs/git-workflow.md`](./docs/git-workflow.md)

## 业务代码（src/）

- `src/` 在 `.gitignore` 中——团队成员需要根据当前 spec 涉及的仓库**自行 clone 到 `src/<repo>/`**，本仓库不存放业务代码。仓库地址与基线分支见 `docs/git-workflow.md`。
- 编码时需加载 `src/<repo>/` 内的 `CLAUDE.md` / `CODEBUDDY.md` / `.codebuddy/rules/` 等工程规则文件，保持代码风格与目标仓库一致。

## 新人入口

- 📘 [`docs/onboarding-codebuddy.md`](./docs/onboarding-codebuddy.md) — **新人完整使用手册**（含端到端实操、CodeBuddy IDE 操作示范、常见错误自救）
- 📗 [`docs/spec-coding-handbook.md`](./docs/spec-coding-handbook.md) — 团队手册（一页纸版本）
- 🧭 [`docs/git-workflow.md`](./docs/git-workflow.md) — Git 流程操作手册（基线分支 / commit / 安全 rebase / 多仓库合并）
