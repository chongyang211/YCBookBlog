# 05｜目录与文件契约

> 📖 **本篇定位**：详细讲解 CoSpec 目录结构的每一层、每份文件的格式契约、模板占位符含义。
> 🔗 **上一篇**：[04 使用手册](./04-how-to-use.md) · **下一篇**：[06 落地你自己的项目](./06-adaptation.md)

---

## 一、完整目录结构

```txt
your-project/
├─ .codebuddy/  or  .cursor/  or  .claude/    ← AI 助手配置（视工具）
├─ CLAUDE.md    or  .cursorrules              ← AI 常驻规则入口
├─ README.md                                   ← 项目说明
│
├─ rules/                                      ← 常驻规则（AI 每次都加载）
│  ├─ 00-project-principles.md                ← 核心原则
│  ├─ 10-spec-workflow.md                     ← 工作流规则（核心）
│  ├─ 20-coding-rules.md                      ← 编码规则
│  ├─ 30-testing-rules.md                     ← 测试规则
│  └─ 40-documentation-rules.md               ← 文档规则
│
├─ commands/                                   ← Slash Commands 定义
│  ├─ spec-intake.md
│  ├─ spec-design.md
│  ├─ spec-draft.md
│  ├─ spec-plan.md
│  ├─ spec-tasks.md
│  ├─ spec-implement.md
│  ├─ spec-test.md
│  ├─ spec-review.md
│  ├─ spec-push.md
│  └─ spec-sync.md
│
├─ skills/                                     ← 按需加载的技能（复杂逻辑）
│  ├─ spec-drafting/
│  │  ├─ SKILL.md
│  │  └─ README.md
│  ├─ spec-analysis/
│  ├─ codebase-survey/
│  ├─ implementation-planning/
│  ├─ feature-implementation/
│  ├─ test-writing/
│  ├─ change-summary/
│  └─ technical-design/
│
├─ docs/                                       ← 项目级文档 + 方法论文档矩阵
│  ├─ README.md                               ← 文档总入口
│  ├─ 01-overview.md ~ 07-case.md             ← 通用方法论 7 篇（全景 → 案例）
│  └─ git-workflow.md                         ← Git 工作流细则
│
├─ intake/                                     ← 原始需求录入（五类产物之一）
│  ├─ templates/
│  │  └─ intake-template.md
│  └─ v1.0.0/
│     └─ 12345-example.md
│
├─ designs/                                    ← 技术方案（可选前置）
│  ├─ templates/
│  │  └─ design-template.md
│  └─ v1.0.0/
│     └─ 12345-example-design.md
│
├─ specs/                                      ← 需求单（核心三件套之一）
│  ├─ templates/
│  │  └─ spec-template.md
│  └─ v1.0.0/
│     └─ 12345-example.md
│
├─ plans/                                      ← 实施计划
│  ├─ templates/
│  │  └─ plan-template.md
│  └─ v1.0.0/
│     └─ 12345-example-plan.md
│
├─ tasks/                                      ← 任务清单
│  ├─ templates/
│  │  └─ tasks-template.md
│  └─ v1.0.0/
│     └─ 12345-example-tasks.md
│
├─ src/                                        ← 业务代码（可能 gitignore）
├─ tests/                                      ← 测试代码
└─ .gitignore
```

---

## 二、四种目录分类

CoSpec 的目录按角色分四类：

| 分类           | 目录                                     | 特点                                  |
| -------------- | ---------------------------------------- | ------------------------------------- |
| **AI 契约层**  | `rules/` `commands/` `skills/`           | 定义"AI 如何行事"                     |
| **需求文档层** | `intake/` `designs/`                | 需求录入 + 技术方案（可选前置）       |
| **三件套层**   | `specs/` `plans/` `tasks/`               | **核心**——CoSpec 的骨架              |
| **代码层**     | `src/` `tests/`                          | 实际代码 + 测试                       |

---

## 三、命名规范（严格）

### 3.1 文件命名

所有产物按 `<VERSION>/<STORYID>-<slug>.md` 命名：

```txt
intake/v1.0.0/12345-oauth-login.md
designs/v1.0.0/12345-oauth-login-design.md
specs/v1.0.0/12345-oauth-login.md
plans/v1.0.0/12345-oauth-login-plan.md
tasks/v1.0.0/12345-oauth-login-tasks.md
```

**规则**：

- `<VERSION>`：迭代版本号（如 `v1.0.0`、`v2.5.3`），你自己定义
- `<STORYID>`：纯数字需求编号，来自需求管理系统；无对应 story 用 `0` 占位
- `<slug>`：kebab-case 简短标题（如 `oauth-login`、`payment-retry`）
- **同 Story 下的三件套 slug 必须完全一致**——这是"三件套互相定位的依据"

### 3.2 分支命名

```txt
feature/<STORYID>-<slug>      ← 新功能
hotfix/<STORYID>-<slug>       ← 紧急修复
```

跨仓库同名分支——重要约定。

### 3.3 Commit 消息

```txt
<type>(<scope>): <subject> --story=<STORYID> [#finish]
```

- `<type>`：`feat` / `fix` / `docs` / `refactor` / `test` / `chore` 等（约定式 commit）
- `<scope>`：模块名
- `<subject>`：一句话
- `--story=`：**强制**
- `[#finish]`：**仅**加在最后一笔 commit

---

## 四、多 Spec 共享 Story ID 场景

一个大需求可能拆多个 spec，共享 Story ID，靠 slug 区分：

```txt
Story 12345："用户账户体系统一"

拆成 4 个子 spec：
- specs/v1.0.0/12345-account-schema-migration.md
- specs/v1.0.0/12345-account-service-refactor.md
- specs/v1.0.0/12345-account-api-v2.md
- specs/v1.0.0/12345-account-client-migration.md

每个 spec 独立走完 plan → tasks → implement 流程。
每个 spec 在 frontmatter 用 Sibling Specs 引用兄弟。
```

**frontmatter 示例**：

```markdown
> Story ID: 12345
> Status: draft
> Sibling Specs:
>   - specs/v1.0.0/12345-account-schema-migration.md
>   - specs/v1.0.0/12345-account-api-v2.md
>   - specs/v1.0.0/12345-account-client-migration.md
```

---

## 五、Spec 模板（核心）

`specs/templates/spec-template.md`：

```markdown
# [标题：简洁描述这个功能]

> **Story ID**: [12345 或 0]
> **Status**: draft | ready | in-progress | implemented | deprecated
> **Author**: [作者]
> **Created**: [日期]
> **Updated**: [日期]
> **Sibling Specs**: [可选；同 Story 下的兄弟 spec 路径]
> **Branch**: `feature/<spec-name>` 或 `hotfix/<spec-name>`

## 背景

为什么需要这个功能？当前存在什么问题或缺失？

## 目标

- 目标 1
- 目标 2

## 非目标

明确排除哪些事情**不在**本次范围内。

- 非目标 1

## 用户故事

- 作为 [角色]，我希望 [操作]，以便 [价值]

## 功能需求

### FR-1: [需求名称]

描述具体行为。

### FR-2: [需求名称]

## 非功能需求

- **性能**：[如有要求]
- **安全**：[如有要求]
- **兼容性**：[如有要求]

## 数据结构 / API / 接口影响

（新增/修改的数据模型、API、配置项）

## 状态流转 / 业务流程

（如涉及状态机或多步骤流程）

## 边界情况

| 场景 | 预期行为 |
|------|----------|
| [边界 1] | [处理方式] |

## 验收标准

- [ ] 标准 1：[具体可验证的条件]
- [ ] 标准 2：...

## 测试点

- **正常路径**：[描述]
- **边界条件**：[描述]
- **错误处理**：[描述]

## 风险与未决问题

| 项目 | 描述 | 状态 |
|------|------|------|
| [风险 1] | [描述] | open / resolved |

## 实施备注

给实现者的额外提示。

## 修订记录

| 日期 | 修订点 | 关联 tasks 偏离记录 | 修订人 |
|------|--------|---------------------|--------|
| —    | 初始版本 | —                   | [作者] |
```

**关键约束**：

- **每个章节内容必须标信息来源**（起草时）：📥 原始需求 / 🤖 AI 推断 / ❓ TBD / 🔍 现有代码
- **验收标准必须可打勾验证**——不能写"实现良好"这种模糊描述
- **`Status: draft` 是初始状态**，人审通过才能改 `ready`

---

## 六、Plan 模板

`plans/templates/plan-template.md`：

```markdown
# Plan: [spec 标题]

> **Story ID**: 12345
> **Related Spec**: specs/v1.0.0/12345-<slug>.md
> **Status**: draft | approved | in-progress | done
> **Author**: [作者]
> **Created**: [日期]

## 复用的技术决策（来自 Design）

如已有 design，此处引用关键决策。若无 design，简述本 plan 的技术方向。

## 改动范围

### 新增文件

| 路径 | 用途 |
|------|------|
| src/... | ... |

### 修改文件

| 路径 | 修改要点 |
|------|----------|
| src/... | ... |

### 删除文件

（如有）

## Phase 拆分

### Phase 1: [Phase 名]

**目标**：本 Phase 完成后达到的状态

**涉及文件**：

- src/...
- tests/...

**依赖**：无 / 依赖 Phase X

**步骤概述**：

1. ...
2. ...

### Phase 2: [...]

## 风险

| 风险 | 缓解措施 |
|------|----------|
| [风险 1] | [措施] |

## 验证要点

对应 spec 的验收标准，本 plan 每个 Phase 涵盖哪些验收标准。

| 验收标准 | 由哪个 Phase 覆盖 |
|----------|-------------------|
| 标准 1 | Phase 2 |
```

---

## 七、Tasks 模板

`tasks/templates/tasks-template.md`：

```markdown
# Tasks: [spec 标题]

> **Story ID**: 12345
> **Related Plan**: plans/v1.0.0/12345-<slug>-plan.md
> **Status**: in-progress | done
> **Author**: [作者]
> **Created**: [日期]

## Phase 1: [Phase 名]

- [ ] 1.1 [任务描述]
- [ ] 1.2 [任务描述]
- [ ] 1.3 [任务描述]

## Phase 2: [Phase 名]

- [ ] 2.1 [任务描述]
- [ ] 2.2 [任务描述]

## 偏离记录

> 实施过程中如与 spec/plan 不符，在此追加。

| 日期 | 偏离点 | 原因 | 建议 spec 修订 |
|------|--------|------|----------------|
| —    | 初始   | —    | —              |

## 完成校验清单

- [ ] 所有任务勾选完成
- [ ] 所有验收标准打勾（见 spec）
- [ ] 测试全部通过
- [ ] Review 报告通过
- [ ] 变更摘要已写
- [ ] Spec status 更新为 implemented
```

---

## 八、Design 模板（可选）

`designs/templates/design-template.md`：

```markdown
# Design: [标题]

> **Story ID**: 12345
> **Status**: draft | approved | superseded
> **Author**: [作者]
> **Reviewer**: [评审人清单]
> **Created / Updated**: [日期]

## 需求背景与目标

忠实复述原始需求（来自 intake）。

## 现状分析

- 现有架构
- 模块边界
- 约束

## 总体方案

核心思路 + 架构图 / 数据流图。

## 方案对比

关键决策的备选方案 + 取舍。

| 决策点 | 方案 A | 方案 B | 方案 C | 选择 | 理由 |
|--------|--------|--------|--------|------|------|
| [决策 1] | ... | ... | ... | B | ... |

## 关键技术决策

- **决策 1**：选了 X，为什么；淘汰了 Y、Z
- **决策 2**：...

## 跨系统 / 跨仓库影响

| 系统 / 仓库 | 影响 | 需要变更 |
|-------------|------|----------|
| ... | ... | ... |

## 数据结构 / 接口影响（高层）

（不到具体字段，只到"哪些表 / 哪些接口需要动"）

## 建议的 spec 拆分

**核心产出**：本 design 建议拆几个 spec、各自边界。

| 建议 slug | 边界 | 备注 |
|-----------|------|------|
| account-schema-migration | 数据层改动 | 优先 |
| account-service-refactor | 服务层封装 | 依赖 schema |
| account-api-v2 | 对外 API | 依赖 service |

## 风险与未决问题

## 评审记录

| 日期 | 评审人 | 意见 | 处理 |
|------|--------|------|------|
| —    | —      | —    | —    |
```

---

## 九、Intake 模板

`intake/templates/intake-template.md`：

```markdown
---
Story ID: 12345
Received: [日期]
Source: [来源：会议 / 邮件 / IM / 其他]
Reporter: [提出人]
Version: [归属版本]
---

# [原始需求标题]

## 原始需求（原话）

> [直接引用原话，不改写]

## 相关背景

- 相关系统 / 模块
- 相关竞品
- 相关约束

## 待澄清点

- [ ] 问题 1
- [ ] 问题 2

## 相关资源

- 链接 1（IM 消息 / 邮件 / 会议纪要）
- 链接 2
```

**关键**：**intake 保留"原话"性质**——不要在 intake 里就开始设计方案。

---

## 十、Rules 层文件说明

### 10.0 三层如何协作（先看这张图）

```txt
        ┌──────────────┐
        │   人（你）    │
        └──────┬───────┘
               │ 触发
               ▼
        ┌──────────────┐
        │  Commands    │  ← 人直接交互的接口（/spec-plan 等）
        └──────┬───────┘
               │ 调用
               ▼
        ┌──────────────┐
        │  Skills      │  ← AI 的"操作手册"（spec-analysis 等，自动调用）
        └──────┬───────┘
               │ 必须遵循
               ▼
        ┌──────────────┐
        │  Rules       │  ← 永远在背景里生效（10-spec-workflow 等）
        └──────────────┘
```

**一句话**：**Commands 是人的入口，Skills 是 AI 的操作手册，Rules 是永远生效的背景约束**——一个 Command 通常调用多个 Skill，全部行为受 Rules 制约。

### 10.1 `00-project-principles.md`

**内容**：项目最核心原则（7 条左右），是"地基"。

**特点**：不可打折扣，是所有其他规则的宪法。

### 10.2 `10-spec-workflow.md`

**内容**：CoSpec 8 阶段工作流的完整定义。

**特点**：**最重要的一份规则文件**——AI 处理任何 spec 任务前必读。

### 10.3 `20-coding-rules.md`

**内容**：编码规则（最小改动、复用优先、避免无关重构等）。

**特点**：约束 AI 在 `src/` 里的行为。

### 10.4 `30-testing-rules.md`

**内容**：测试规则（何时必须测试、命名规范、覆盖要求）。

### 10.5 `40-documentation-rules.md`

**内容**：文档同步规则（哪些变更必须更新哪些文档）。

---

## 十一、Commands 层文件格式

每个命令文件 `commands/<name>.md` 的固定结构：

```markdown
# Command: <name>

## 命令名称
`<name>`

## 作用
（一段话说明该命令做什么、在流程中的位置）

## 输入参数
| 参数 | 必选 | 说明 |
|------|------|------|
| ... | ... | ... |

**触发方式**：
（示例对话）

## 执行步骤
（AI 应按顺序执行的步骤，逐条列出）

## 输出结果
（会产出哪些文件、哪些 chat 输出）

## 后续动作
（用户下一步应做什么）

## 适用时机
（何时用 / 何时不用）

## 关联规则与技能
（引用哪些 rules / skills）

## 与其他命令的衔接
（数据流图）
```

---

## 十二、Skills 层文件格式

Skills 是"按需加载的复杂逻辑"（vs Rules 是"每次都加载的常驻规则"）。

每个 skill 目录 `skills/<name>/` 的固定结构：

```txt
skills/<name>/
├─ SKILL.md          ← 主入口
└─ README.md         ← 该 skill 的说明（可选）
```

`SKILL.md` 的 frontmatter：

```markdown
---
name: <name>
description: 何时应触发这个 skill 的一句话说明
---

# <名字>

## 核心原则（必读）
（列出该 skill 的核心约束）

## 执行步骤
### Step 1: ...
### Step 2: ...

## 常见误区
```

**skill 的触发**：由 command 或 rule 显式调用（如 `/spec-draft` 会调用 `skills/spec-drafting/SKILL.md`）。

**核心 skills 速查**（8 个工作流 skill + 2 个扩展 skill）：

| 技能 | 路径 | 用途 |
|------|------|------|
| 技术方案 | `skills/technical-design/` | 需求大时，起草 spec 前先产出供评审的技术方案 |
| Spec 起草 | `skills/spec-drafting/` | 把原始需求转成 spec 草稿 |
| Spec 分析 | `skills/spec-analysis/` | 分析已有 spec 完整性 |
| 代码侦察 | `skills/codebase-survey/` | 扫描现有代码（被 spec-drafting / implementation-planning 自动调用，也可独立用） |
| 实施计划 | `skills/implementation-planning/` | 制定实施计划 |
| 功能实现 | `skills/feature-implementation/` | 执行代码实现 |
| 测试编写 | `skills/test-writing/` | 编写测试用例 |
| 变更摘要 | `skills/change-summary/` | 生成变更摘要（被 review / push / sync 内部调用，无独立命令） |
| MR Spec 评审 | `skills/mr-spec-review/` | 评审他人提交的 spec / plan / tasks MR，结论投递为 MR 评论 |
| MR 评审处理 | `skills/mr-review-resolve/` | 作者侧：逐条评估 MR 评论并处置、回写 resolved |

---

## 十三、CLAUDE.md（AI 助手入口）

项目根的 `CLAUDE.md`（或 `.cursorrules` 等，视 AI 助手而定）是 AI 加载规则的入口。本仓库已预置 `CLAUDE.md` 作为可直接使用的示例；落地到你团队时按需修改（Claude Code 用 `@rules/*` 显式导入；CodeBuddy 可改用 `.codebuddy/` 软链，见 [06 §二](./06-adaptation.md)）：

```markdown
# Project: <项目名>

本项目使用 CoSpec 范式（Spec 驱动 AI 协作开发）。

## 当前活跃 Spec 索引

完整索引在 <TEAM-WIKI-URL>（或你的需求系统）。本地不维护聚合 INDEX 文件，避免 MR 冲突；索引由带外命令 `/spec-index` 按需发布。

## 常驻规则（每次会话都加载）

@rules/00-project-principles.md
@rules/10-spec-workflow.md
@rules/20-coding-rules.md
@rules/30-testing-rules.md
@rules/40-documentation-rules.md

## 可用命令

见 commands/ 目录。可用 slash command 触发：
- /spec-intake
- /spec-design
- /spec-draft
- /spec-plan
- /spec-tasks
- /spec-implement
- /spec-test
- /spec-review
- /spec-push
- /spec-sync

## 可用技能

见 skills/ 目录。由命令或规则显式调用。

## 关键原则速记

- AI 只有执行权，无定义权
- 三件套必须落盘：spec / plan / tasks
- 无 spec 不写代码
- MR 必带 Story ID
- Tasks 实时勾选
- 偏离必记录

## 项目特定说明

（此处填写你项目的特定说明，例如：技术栈、部署方式、团队约定等）
```

---

## 十四、`.gitignore` 建议

如果 `src/` 是业务代码（每人自行 clone），可以 gitignore：

```gitignore
# CoSpec 只管协作元数据，业务代码不入本仓
/src/

# 但保留 src/README.md 说明如何 clone 业务代码
!/src/README.md
```

如果 `src/` 是本仓的一部分（例如是完整应用），则**不需要 gitignore**。

---

## 十五、目录设计的思想总结

CoSpec 的目录结构**每一处都对应一个设计意图**：

| 目录             | 设计意图                                      |
| ---------------- | --------------------------------------------- |
| `rules/`         | 常驻约束，让 AI 每次会话都记得                |
| `commands/`      | Slash Command 标准化人机交互入口              |
| `skills/`        | 复杂逻辑按需加载，避免炸 context              |
| `intake/`   | 原始需求原样保留，可审计                       |
| `designs/`       | 大需求前置方案评审，避免 spec 反复返工        |
| `specs/`         | 三件套的核心，"要做什么"的唯一事实来源        |
| `plans/`         | 从"要做什么"到"怎么做"的桥梁                  |
| `tasks/`         | 从"怎么做"到"做到哪了"的进度追踪              |
| `templates/`     | 一致性保障：新文件都从模板开始                |
| `v<VERSION>/`    | 按迭代归档，避免根目录爆炸                     |

---

## 十六、下一步

- 想把 CoSpec 用到自己的项目上 → [06 落地你自己的项目](./06-adaptation.md)
- 想重看整体理念 → [01 全景总览](./01-overview.md)
