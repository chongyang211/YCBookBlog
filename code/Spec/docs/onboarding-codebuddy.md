# 新人完整使用手册（CodeBuddy IDE 环境）

> 适用对象：刚加入团队、第一次接触 Spec Coding 工作流的工程师  
> 阅读时间：约 30 分钟  
> 配套实操：约 1.5 小时（跑完 0001 示例）

---

## 0. 欢迎 & 5 分钟全貌

### 0.1 这套体系一句话讲明白

> **我们不写"AI 自由发挥"的代码。每个改动都从一份 Spec 开始，经过 Plan、Tasks 三件套沉淀，由人审 + AI 执行 + 人合并。**

> 📋 **想先看全队 Spec 一览 / 当前迭代有哪些需求？** 打开 团队 Wiki：<TEAM-WIKI-URL>。这是团队 Spec 索引的唯一发布出口，含每个 spec 的迭代版本、Status、Owner、Spec/Plan/Tasks 链接。

### 0.2 三件套 + 六阶段全景

```
原始需求
   │
   ▼
┌──────┐  ┌──────┐  ┌──────┐  ┌─────────┐  ┌──────┐  ┌──────┐  ┌──────┐
│Draft │→ │ Plan │→ │Tasks │→ │Implement│→ │ Test │→ │Review│→ │ Push │→ Sync
│      │  │      │  │      │  │+切分支  │  │      │  │+摘要 │  │+rebase│
└──────┘  └──────┘  └──────┘  └─────────┘  └──────┘  └──────┘  └──────┘
docs/intake/  plans/   tasks/    src/(本地)   tests/   报告      远端+MR
specs/(draft→ready)
   阶段0       阶段2    阶段2.5   阶段3       阶段4    阶段5     阶段6   阶段6后
```

> 💡 **Stage 1（Spec Review）** 在阶段 0 之后由人完成（review draft → 改 status 为 `ready`），不出命令。  
> 💡 **change-summary**（变更摘要）由 `/spec-review` 与 `/spec-push` 内部自动调用 skill 产出，不单独出命令。

### 0.3 三种资产，三种用途

| 资产类型 | 是什么 | 谁来管 | 谁来用 |
|---|---|---|---|
| **Rules**（规则） | 5 个 rule 文件 + 10 条铁律，定义"什么能做、什么不能做" | 架构组维护 | AI 始终遵循、人 review 时对照 |
| **Skills**（技能） | 8 个 SKILL.md，定义"某类操作怎么做" | 架构组维护 | AI 在执行命令时按需调用 |
| **Commands**（命令） | 10 个斜杠命令（含 3 个可选），是工作流的 verb | 架构组维护 | 人**主动触发**，AI 响应 |

> **核心心法**：**人触发 Commands，Commands 调用 Skills，全过程遵循 Rules。**

---

## 1. 环境准备（约 20 分钟）

### 1.1 CodeBuddy IDE 简介

CodeBuddy 是团队使用的 AI 协作 IDE，特点：
- 内置 AI 助手，支持斜杠命令
- 通过 `.codebuddy/commands` 软链到根目录的 `commands/`，与本仓库的工作流命令完全一致
- 通过 `.codebuddy/rules/rules` 软链到根目录的 `rules/`，规则与 Claude Code 共用同一份

### 1.2 拉取 Spec 工作空间 + 业务代码仓库

#### Step A：拉本仓库（spec 工作空间）

```bash
git clone <GIT-HOST>:<ORG>/CoSpec.git spec
cd spec
```

在 CodeBuddy 中 `File → Open Folder` 选择该目录。

#### Step B：把当前 spec 涉及的业务代码仓库 clone 到 `src/<repo>/`

⚠️ **`src/` 在 `.gitignore` 中，不入本仓库**——团队成员要根据当前任务，把涉及的业务仓库 clone 到 `src/<repo>/` 下。

仓库地址与基线分支详见 [`docs/git-workflow.md`](./git-workflow.md)。常用仓库示例：

```bash
mkdir -p src && cd src
# 业务代码主仓库（基线 develop）
git clone <GIT-HOST>:<ORG>/<业务主仓库>.git
# proto 定义仓库（基线 master）
git clone <GIT-HOST>:<ORG>/<协议仓库>.git
# 其他仓库按需 clone
```

> 💡 **不需要一次性 clone 所有仓库**——按当前 spec 的「涉及仓库」表（在 plan 文件里）clone 即可，没用上的仓库不用拉。

#### Step C：每次开始一个新 spec 前，刷新基线分支

```bash
cd src/<repo>
git checkout <baseline>          # develop 或 master，看 plan
git pull -r origin <baseline>    # rebase 模式同步最新代码
```

### 1.3 第一次打开必读 5 个文件（按顺序）

| 顺序 | 文件 | 看什么 | 时间 |
|------|------|--------|------|
| 1 | `CLAUDE.md` | 项目导航总入口（@-import 了所有规则、列出 10 命令） | 3 分钟 |
| 2 | `docs/spec-coding-handbook.md` | 团队手册（一页纸 + 10 条铁律） | 5 分钟 |
| 3 | `rules/10-spec-workflow.md` | 六阶段工作流（最核心） | 10 分钟 |
| 4 | `docs/git-workflow.md` | 基线分支映射 + commit + 安全 rebase | 5 分钟 |
| 5 | 任意一个 `status: implemented` 的 spec 三件套（去 团队 Wiki 索引找：<TEAM-WIKI-URL>） | 一个真实 Spec/Plan/Tasks 长什么样 | 7 分钟 |

读完这 5 个文件，你已经理解 70%。

### 1.4 目录速查表（5 分钟扫一遍）

```
spec/
├── CLAUDE.md                  # AI 入口，@-import 所有规则
├── README.md                  # 仓库门面（首次使用流程）
├── .gitignore                 # src/、bin/、本地配置等
├── .gitlab/
│   └── merge_request_templates/Default.md   # MR 模板（三段式）
├── docs/                      # 团队手册 + 架构文档
│   ├── spec-coding-handbook.md
│   ├── onboarding-codebuddy.md   ← 你正在读
│   ├── git-workflow.md           ⭐ git 操作手册
│   └── intake/                # 原始需求草稿区（不是 Spec！）
├── rules/                     # 5 条铁律（单一事实来源）
│   ├── 00-project-principles.md
│   ├── 10-spec-workflow.md    ⭐ 最重要
│   ├── 20-coding-rules.md
│   ├── 30-testing-rules.md
│   └── 40-documentation-rules.md
├── skills/                    # 8 个 SKILL（AI 执行技能）
│   ├── technical-design/      # 阶段 0 前（可选，技术方案）
│   ├── spec-drafting/         # 阶段 0
│   ├── spec-analysis/         # 阶段 1 / 5
│   ├── codebase-survey/       # 工具 skill：被 drafting/planning 调用
│   ├── implementation-planning/  # 阶段 2 / 2.5
│   ├── feature-implementation/   # 阶段 3
│   ├── test-writing/          # 阶段 4
│   └── change-summary/        # 工具 skill：被 review/push 调用
├── commands/                  # 斜杠命令（人触发）
│   ├── spec-intake.md         # 阶段 0 前置（可选）
│   ├── spec-draft.md          # 阶段 0
│   ├── spec-plan.md           # 阶段 2
│   ├── spec-tasks.md          # 阶段 2.5
│   ├── spec-implement.md      # 阶段 3
│   ├── spec-test.md           # 阶段 4
│   ├── spec-review.md         # 阶段 5
│   ├── spec-push.md           # 阶段 6（可选）
│   ├── spec-sync.md           # 阶段 6 后
│   └── spec-index.md          # 带外工具 — 由专人按需运行，把索引覆盖发布到 团队 Wiki
├── specs/                     # ⭐ Spec 文件（聚合索引发布到 团队 Wiki: <TEAM-WIKI-URL>）
│   ├── templates/spec-template.md
│   └── <VERSION>/<STORYID>-<slug>.md    # 命名：Story ID + slug
├── plans/                     # ⭐ Plan 文件
│   └── <STORYID>-<slug>-plan.md
├── tasks/                     # ⭐ Tasks 文件
│   └── <STORYID>-<slug>-tasks.md
├── tests/                     # 测试代码
├── src/                       # 业务代码（**已 gitignore**，团队自行 clone）
└── .codebuddy/                # CodeBuddy IDE 协作配置
    ├── commands → ../commands  # 软链：与根目录 commands/ 同步
    └── rules/
        └── rules → ../../rules # 软链：与根目录 rules/ 同步
```

---

## 2. Rules / Skills / Commands 三者关系（必读）

### 2.1 一张图看懂三者关系

```
                    ┌──────────────────┐
                    │  人（你）        │
                    └──────────────────┘
                            │
                            │ 触发
                            ▼
                    ┌──────────────────┐
                    │  Commands        │  ← 你直接交互的接口
                    │  /spec-plan 等   │
                    └──────────────────┘
                            │
                            │ 调用
                            ▼
                    ┌──────────────────┐
                    │  Skills          │  ← AI 的"操作手册"
                    │  spec-analysis 等│
                    └──────────────────┘
                            │
                            │ 必须遵循
                            ▼
                    ┌──────────────────┐
                    │  Rules           │  ← 永远在背景里生效
                    │  10-spec-workflow│
                    └──────────────────┘
```

### 2.2 Rules：永远在背景里生效

- **加载方式**：`CLAUDE.md` 通过 `@rules/*.md` 显式 import，AI 在每轮对话开始时自动加载；CodeBuddy 通过 `.codebuddy/rules/rules` 软链共享同一份
- **作用时机**：AI 每次回复都会遵循
- **5 条规则速查**：

| 文件 | 一句话总结 |
|------|------------|
| `00-project-principles.md` | Spec 是单一事实来源；先理解后行动；最小改动 |
| `10-spec-workflow.md` | **六阶段流程**（最重要，含 Push 与 Sync） |
| `20-coding-rules.md` | 不做无关重构、复用优先、保持现有风格 |
| `30-testing-rules.md` | spec 验收标准每条至少一个测试 |
| `40-documentation-rules.md` | 实现完成后必须更新 spec 状态 |

> **新人禁忌**：**永远不要让 AI 跳过这些规则。**如果 AI 想跳过（比如"为了简洁我直接重构了"），把它叫停。

### 2.3 Skills：AI 执行某类操作的"技能包"

Skills 不需要你手动调用，AI 在执行命令时会自动 reference。但你要知道有哪些：

| Skill | 触发场景 | 输出格式 |
|-------|---------|---------|
| `spec-drafting` | 把原始需求转成 spec 草稿 | spec 草稿 + 澄清问题 |
| `spec-analysis` | 分析已有 Spec 完整性、提取 FR | 理解摘要 + 完整性评估 |
| `codebase-survey` | 扫描现有代码（light/deep 两种模式） | 模块清单 / 可复用资产 / 参考代码模式 |
| `implementation-planning` | 把 spec 转化为可执行 plan | 改动范围 + 步骤 + 风险 |
| `feature-implementation` | 实施代码改动 | 代码 + 进度报告 |
| `test-writing` | 根据 spec 编写测试 | 测试代码 + 覆盖映射 |
| `change-summary` | 完成后输出变更摘要 | 摘要 + 影响范围 + 风险 |

> 💡 **codebase-survey** 比较特殊：它是**被其他 skill 调用**的工具型 skill。`spec-drafting`（阶段零）会自动调用它的 light 模式；`implementation-planning`（阶段二）会自动调用它的 deep 模式。新人**不需要**手动触发，但可以独立用："帮我看下 wecardexample-app 现状"。

> 这 8 个 Skill 是项目本身的工作流技能，AI 在执行 commands 时会自动调用，新人**不需要**手动触发。

### 2.4 Commands：你的工作台

这 8 个命令是你**主动触发**的入口。在 CodeBuddy 的 AI 对话框输入：

| 命令 | 阶段 | 输入 | 产出文件 |
|------|------|------|---------|
| `/spec-intake`（可选） | 阶段 0 前置 | PM 需求文档路径 或 描述 | `docs/intake/<VERSION>/<STORYID>-<slug>.md`（原始需求草稿） |
| `/spec-draft` | 阶段 0 | docs/intake/xxx.md 或描述 | `specs/<VERSION>/<STORYID>-<slug>.md`（status: draft） |
| `/spec-plan` | 阶段 2 | spec 路径 | `plans/<VERSION>/<STORYID>-<slug>-plan.md` |
| `/spec-tasks` | 阶段 2.5 | spec + plan 路径 | `tasks/<VERSION>/<STORYID>-<slug>-tasks.md` |
| `/spec-implement` | 阶段 3 | spec 路径 | feature 分支 + `src/` 改动 + tasks 实时勾选 |
| `/spec-test` | 阶段 4 | spec 路径 | `tests/` 文件 |
| `/spec-review` | 阶段 5 前 | spec 路径 | review 报告 |
| `/spec-push`（可选） | 阶段 6 | spec 路径（+ commit msg） | commit + 安全 rebase + push（视个人提交习惯，也可自行 git 提交） |
| `/spec-sync` | 阶段 5/6 后 | spec 路径或 all | spec 状态同步 |

> **CodeBuddy 触发方式**：
> - 方式 A：在 AI 对话框直接输入 `/spec-plan specs/v1.6.0/0-example-feature.md`
> - 方式 B：自然语言表述，如 `请根据 specs/v1.6.0/0-example-feature.md 执行 spec-plan`
> - 两种方式 CodeBuddy 都能识别，自然语言更稳。

---

## 3. 新人 0→1 全流程演示（核心章节）

> **目标**：用 `specs/v1.6.0/0-example-feature.md`（Todo 列表）走完一遍六阶段（阶段 1 → 阶段 6 + Sync）。  
> **预计时间**：1.5 小时（含 review 思考时间）  
> **完成后**：你已掌握全部流程。

> 💡 **关于阶段 0**：`0-example-feature` 是已经写好的 spec，所以本演示**跳过阶段 0**。当你接到一个**没有 spec** 的真实需求时，第一步是 `/spec-draft`（详见第 4 章）。

### 第 1 步：理解 Spec（阶段 1）— 10 分钟

**操作**：
1. 在 CodeBuddy 打开 `specs/v1.6.0/0-example-feature.md`
2. 通读全文，**特别关注**：
   - **目标 / 非目标**：知道做什么、不做什么
   - **功能需求 FR-1 ~ FR-5**：5 个 CRUD 操作
   - **验收标准**：10 条 checkbox
   - **边界情况**：8 个场景
   - **测试点**：4 类测试

**用了什么**：
- Rules：背景中已生效（你不需要主动做什么）
- Skills：暂未触发
- Commands：暂未触发

**通过标志**：
- 你能用 3 句话总结这个 Spec 在做什么
- 你能列出 3 个最关键的边界情况
- 你能说出哪条验收标准最容易遗漏

> 💡 **提示**：在 CodeBuddy 中可以直接对 AI 说"请帮我分析一下 specs/v1.6.0/0-example-feature.md"，AI 会调用 `spec-analysis` Skill 给你一份理解摘要。

---

### 第 2 步：起草 Plan（阶段 2）— 15 分钟

**操作**：
1. 在 CodeBuddy AI 对话框输入：
   ```
   请根据 specs/v1.6.0/0-example-feature.md 执行 spec-plan
   ```
2. AI 会：
   - 读取 spec
   - 调用 `spec-analysis` Skill 评估完整性
   - 调用 `implementation-planning` Skill 起草 plan
   - 输出 plan 内容**等你确认**
3. 你审 plan，关注 3 件事：
   - **改动范围**：列出的文件是否合理？
   - **关键决策**：技术选型（数据结构、存储接口等）是否合理？
   - **风险**：是否漏了什么？
4. 如果有问题：
   ```
   plan 里 Step 2 应该用 map 而不是 slice，请修改
   ```
5. plan OK 后，明确说：
   ```
   plan 确认，请写入 plans/v1.6.0/0-example-feature-plan.md
   ```

**用了什么**：
- Rules：`10-spec-workflow.md` 阶段二（要求 plan 必须沉淀文件）
- Skills：`spec-analysis` + `implementation-planning`
- Commands：`/spec-plan`

**产出文件**：`plans/v1.6.0/0-example-feature-plan.md`

**通过标志**：
- 文件存在于 `plans/` 目录
- 文件包含 8 个章节（设计概述 / 改动范围 / 实施步骤 / 依赖 / 风险 / 决策 / 待确认 / 复杂度）
- 你看完 plan 心里有数：每个 FR 对应哪个文件、按什么顺序写

---

### 第 3 步：拆解 Tasks（阶段 2.5）— 10 分钟

**操作**：
1. 在 CodeBuddy 中：
   ```
   请根据 specs/v1.6.0/0-example-feature.md 执行 spec-tasks
   ```
2. AI 会：
   - 检查 plan 已确认
   - 把 plan 的每个 Step 拆为单文件/单函数级 task
   - 建立"验收标准 → task"映射表
   - 输出 tasks 文件
3. 你审 tasks，关注：
   - 每条 task 是否标注了关联文件、关联 FR？
   - 验收标准映射表是否完整？（10 条 AC 都映射了吗？）
   - DoD（完成定义）是否清晰？

**用了什么**：
- Rules：`10-spec-workflow.md` 阶段二点五（要求 tasks 必须沉淀文件）
- Skills：`implementation-planning`
- Commands：`/spec-tasks`

**产出文件**：`tasks/v1.6.0/0-example-feature-tasks.md`

**通过标志**：
- 文件存在
- 每条 task 都有编号（如 T1.1）+ 关联文件 + 关联 FR
- 验收标准映射表全覆盖（所有 spec 验收标准都有对应 task）

---

### 第 4 步：执行实施（阶段 3）— 30~40 分钟

**操作**：
1. 在 CodeBuddy 中：
   ```
   请根据 specs/v1.6.0/0-example-feature.md 执行 spec-implement
   ```
2. AI 会：
   - 校验 Story ID 一致性（Step 0）
   - **拉分支**（Step 0.5）：在所有相关仓库执行 `git checkout -b feature/0-example-feature`
   - 检查 plan 与 tasks 都存在
   - 按 tasks 的 Phase 顺序执行
   - **每完成一条 task，立即在 tasks 文件中勾选 `[x]`**
3. 你做的事：
   - 确认 AI 拉的分支名符合 `{feature|hotfix}/{spec-name}` 规范
   - 每完成一个 Phase，**review 一次代码**
   - 检查改动范围是否超出 plan
   - 注意 AI 是否实时勾选 tasks（如果它批量勾选，就是违规）

**用了什么**：
- Rules：`10-spec-workflow.md` 阶段三、`20-coding-rules.md`（最小改动、复用优先）
- Skills：`feature-implementation`
- Commands：`/spec-implement`

**产出**：
- `src/` 中的代码改动（在 `feature/0-example-feature` 分支上）
- `tasks/v1.6.0/0-example-feature-tasks.md` 中所有 task 已勾选

**分支命名规范**：

| spec | 分支名 |
|------|-------|
| `specs/v1.6.0/10086-example-user-login.md` | `feature/10086-example-user-login` |
| `specs/v1.6.0/10086-example-gateway.md`（子 spec） | `feature/10086-example-gateway` |
| `specs/v1.6.0/0-fix-payment-timeout.md`（紧急修复） | `hotfix/0-fix-payment-timeout` |

> ⚠️ 跨仓库的所有 PR 必须使用同一分支名，便于追溯关联。

**典型对话流（CodeBuddy）**：

```
你：请根据 specs/v1.6.0/0-example-feature.md 执行 spec-implement

AI：[读取 spec、plan、tasks]
    [开始 Phase 1: 数据结构]
    [写入 src/todo/model.go]
    [更新 tasks: T1.1 勾选完成 ✓]
    Phase 1 完成。继续 Phase 2 吗？

你：继续

AI：[Phase 2: 存储接口]
    ...
```

**通过标志**：
- 所有 task 都已 `[x]` 勾选
- 代码改动只在 plan 范围内
- 没有"顺手优化"的代码（如果有，就是违规）

---

### 第 5 步：编写测试（阶段 4）— 15 分钟

**操作**：
1. 在 CodeBuddy 中：
   ```
   请根据 specs/v1.6.0/0-example-feature.md 执行 spec-test
   ```
2. AI 会：
   - 调用 `test-writing` Skill
   - 对照 spec 的「验收标准」和「测试点」
   - 编写测试代码到 `tests/` 目录
3. 你审测试：
   - 每条验收标准有对应测试吗？
   - 8 个边界情况都覆盖了吗？
   - 测试命名规范吗？（`Test_功能_场景_预期`）

**用了什么**：
- Rules：`30-testing-rules.md`（每条验收标准至少一个测试）
- Skills：`test-writing`
- Commands：`/spec-test`

**产出文件**：`tests/todo_test.go`（或类似）

**通过标志**：
- 测试本地运行 PASS
- 验收标准覆盖率 100%
- 边界情况无遗漏

---

### 第 6 步：Review（阶段 5 前）— 10 分钟

**操作**：
1. 在 CodeBuddy 中：
   ```
   请对 specs/v1.6.0/0-example-feature.md 执行 spec-review
   ```
2. AI 会输出：
   - **Spec 符合性表格**：每条验收标准是否满足
   - **规则合规检查**：是否违反 20/30 规则
   - **代码质量检查**：错误处理、注释、依赖
   - **总体评价**：通过 / 需修改 / 不通过

**用了什么**：
- Rules：全部 5 条
- Skills：`spec-analysis` + `change-summary`
- Commands：`/spec-review`

**通过标志**：
- 总体评价为「通过」
- 任何 ❌ 项都已修复或被你接受为已知偏差

---

### 第 7 步：提 PR（阶段 5）— 5 分钟

**操作**：
1. 在终端：
   ```bash
   git checkout -b feature/0001-todo-list
   git add .
   git commit -m "feat: implement Spec 0001 - Todo list management"
   git push -u origin feature/0001-todo-list
   ```
2. 在 GitLab/Git 平台（GitHub / GitLab / 工蜂）创建 MR/PR
3. **PR 描述自动套用** `.gitlab/merge_request_templates/Default.md` 的三段式：
   - 关联 Spec / Plan / Tasks
   - 偏离说明（无则写"无"）
   - 变更摘要（让 AI 用 `change-summary` Skill 帮你生成）

**让 AI 生成变更摘要**：
```
请根据本次 0001 的实施输出变更摘要
```

AI 会调用 `change-summary` Skill，输出：
- 改了什么 / 为什么 / 影响范围 / 风险点 / 建议验证方式

**用了什么**：
- Rules：`40-documentation-rules.md`
- Skills：`change-summary`
- Commands：（无，但 `change-summary` Skill 会被自然语言触发）

---

### 第 8 步：同步收尾（阶段 5 后）— 5 分钟

**操作**：
1. PR 合并后，在 CodeBuddy 中：
   ```
   请对 specs/v1.6.0/0-example-feature.md 执行 spec-sync
   ```
2. AI 会：
   - 更新 `specs/v1.6.0/0-example-feature.md` 的 `Status` 改为 `implemented`
   - 更新 `Updated` 日期
   - 勾选 spec 中的「验收标准」checkbox

> 💡 Spec 索引发布到 团队 Wiki（<TEAM-WIKI-URL>）由专人/工具按需运行 `/spec-index` 覆盖发布，**不在个人 onboarding 流程中执行**。

**用了什么**：
- Rules：`40-documentation-rules.md`
- Commands：`/spec-sync`

**通过标志**：
- spec 状态为 `implemented`
- 所有 task 都已勾选
- 所有验收标准都已勾选

---

### 🎉 完整流程总结

```
你的输入                  AI 触发的资产                                产出文件
──────────────────────────────────────────────────────────────────────────────────
原始需求                  →   Rules 在背景生效                       →   docs/intake/*.md
/spec-draft               →   spec-drafting + codebase-survey(light) →   specs/<VERSION>/<STORYID>-*.md（draft）
[人 review 改 ready]      →   （无）                                 →   specs/<VERSION>/<STORYID>-*.md（ready）
读 spec                   →   Rules 在背景生效                       →   （无）
/spec-plan                →   spec-analysis +                        →   plans/<VERSION>/<STORYID>-*-plan.md
                              codebase-survey(deep) +
                              implementation-planning
/spec-tasks               →   implementation-planning                →   tasks/<VERSION>/<STORYID>-*-tasks.md
/spec-implement           →   feature-implementation                 →   src/* + tasks 勾选
/spec-test                →   test-writing                           →   tests/*
/spec-review              →   spec-analysis + change-summary         →   review 报告（对话中）
/spec-push                →   change-summary                         →   commit + 安全 rebase + push -f + MR 提示
[人在Git 平台（GitHub / GitLab / 工蜂）创建 MR]         →   （无）                                 →   MR
[MR 合并完成]             →   （无）                                 →   远端 develop/master
/spec-sync                →   （无 Skill）                           →   spec 状态
```

> Spec 索引（团队 Wiki）由专人/工具按需 `/spec-index` 覆盖发布，**不出现在个人开发流程中**。

---

## 4. 第一个真实任务怎么开始

走完 0001 后，你将开始接到真实任务。**与 0001 不同的是，真实任务没有现成 spec**——你要从原始需求开始。

### 4.1 接到任务的标准动作（含阶段 0）

```
1. 任务来源：产品/Tech Lead 给你一个需求
   ↓
2. 落到 docs/intake/（二选一，视习惯）：
   - 手动：复制 docs/intake/templates/intake-template.md，命名为 docs/intake/<VERSION>/<STORYID>-<slug>.md，把原话填进去
   - 或（可选命令）：执行 `/spec-intake`，把 PM 需求文档路径或口头描述交给 AI，自动生成 intake 草稿
   ↓
3. 阶段 0：执行 /spec-draft
   ┌─────────────────────────────────────────────┐
   │ 在 CodeBuddy 输入：                         │
   │ 请基于 docs/intake/YYYY-MM-DD-xxx.md        │
   │ 执行 spec-draft                             │
   │                                             │
   │ AI 会：                                     │
   │   ① 与你确认 Story ID 与版本目录            │
   │   ② 列出 2-5 个澄清问题等你回答             │
   │   ③ 起草 specs/<VERSION>/<STORYID>-<slug>.md（status: draft）│
   │   ④ 输出起草报告（含 [TBD] 清单）           │
   └─────────────────────────────────────────────┘
   ↓
4. Spec Review：与 Tech Lead / PO 一起 review draft
   - 回答 AI 问的澄清问题
   - 补全所有 [TBD]
   - 确认验收标准
   - status 改为 ready
   ↓
5. 走第 3 章的步骤 1-8（spec → plan → tasks → implement → test → review → sync）
```

### 4.2 找谁帮忙

| 卡点 | 找谁 |
|------|------|
| Spec 写不出来 | Tech Lead 或 同 Owner |
| Plan 拿不准（架构选型） | 架构师 |
| 实施时发现 spec 有缺陷 | 不要私自改 spec！在 tasks 文件「偏离记录」里记下来，找 Spec Author |
| 单测一直跑不过 | 找同事；让 AI 帮忙定位（"分析一下为什么 xxx_test 不通过"） |
| PR 一直被打回 | 看 review 评论，针对性补 |

### 4.3 第一周的三个里程碑

- [ ] **Day 1-2**：跑通 `0-example-feature` 完整流程
- [ ] **Day 3-5**：完成第一个真实小任务（建议挑一个 < 200 行改动）
- [ ] **Week 2**：能独立 review 别人的 Spec / Plan / MR

---

## 5. 常见错误自救手册

| 现象 | 原因 | 自救 |
|------|------|------|
| AI 跳过 plan 直接写代码 | 你没明确触发 `/spec-plan` | 立刻喊停，要求先 plan |
| AI 改了 spec 没说 | AI 违规 | 立刻还原 spec，让 AI 在 tasks 的「偏离记录」中说明 |
| AI 顺手重构了相邻代码 | 违反 `20-coding-rules.md` 最小改动原则 | 让 AI 撤销重构改动；记录到 docs/intake/ 待立项 |
| Tasks 没实时勾选 | AI 攒到最后批量勾选 | 喊停，要求每完成一个 task 立刻勾选 |
| AI 引入新依赖 | 未经允许 | 让 AI 撤销，除非 spec 明确要求 |
| Spec status 一直没更新 | 流程没走完 | 执行 `/spec-sync` |
| 测试只覆盖 happy path | 违反 `30-testing-rules.md` | 让 AI 补边界测试 |
| PR 被打回"没关联 Spec" | 没用 PR_TEMPLATE | 改 PR 描述，三段式 |
| 我不确定要不要拆 tasks | 改动 > 50 行 / 跨 3+ 文件 → 拆；否则可省 | 见 `commands/spec-tasks.md` |

---

## 6. 实施偏离 Spec 时怎么办（**重要**）

实施过程中你会经常发现：spec 写得不够细、漏了一种边界、命名与现实不符、甚至 FR 本身需要调整。**这是正常的**。但**怎么处理**决定了团队流程是否扎实。

### 6.1 核心原则：不沉默、不私改

| 严禁 | 必须 |
|------|------|
| AI/Dev 自己默默把 spec 改了 | 在 `tasks/` 的「偏离记录」中**先记下来** |
| AI/Dev 按自己的理解写代码、不告知 Spec Author | 暂停或继续依严重度决定，但**必须可见** |
| 把偏离塞进 PR 描述就完事 | 必须**回流到 spec/plan**，让下一个人看到的还是 spec = 现实 |

### 6.2 偏离回流（Sync-back）四步流程

```
            发现偏离
                │
                ▼
        ┌──────────────┐
        │ Step 1: 立刻 │  在 tasks「偏离记录」追加一行
        │ 记录          │  日期 / 偏离点 / 原因 / 严重度 / status=open
        └──────────────┘
                │
                ▼
        ┌──────────────┐
        │ Step 2: 评估 │  小 / 中 / 大 三档
        │ 严重度        │
        └──────────────┘
                │
       ┌────────┼────────┐
       ▼        ▼        ▼
     [小]     [中]     [大]
       │        │        │
       ▼        ▼        ▼
    继续实施  暂停task  停止spec
       │        │        │
       └────┬───┴────────┘
            ▼
        ┌──────────────┐
        │ Step 3: 回流 │  改 spec → 改 plan → 改 tasks
        │ 同步          │  spec status: in-progress
        └──────────────┘
                │
                ▼
        ┌──────────────┐
        │ Step 4: 验证 │  执行 /spec-sync
        │ 三方一致      │  确认 spec/plan/tasks 对齐
        └──────────────┘
```

### 6.3 三档严重度判定与处理

#### 🟢 小（minor）—— 措辞、命名、补充细节

**典型场景**：
- spec 里说"返回 not found"，实际代码返回 `code=100034`
- spec 里写 "user_id"，实际字段名 "userId"
- spec 漏了一句"日志要脱敏"

**处理（不暂停实施）**：

1. tasks 偏离记录追加一行，status: `open`
2. 完成当前 task，继续下一个
3. 当前 Phase 结束后，**Dev Owner**（你）直接修 spec 的措辞：
   - 不改 FR 边界、不改验收标准
   - 在 spec 末尾「修订记录」中追加一行
   - spec `Updated` 日期刷新
4. tasks 偏离记录 status → `spec-updated`
5. spec status **不变**

#### 🟡 中（major）—— 流程多/少一步、参数名/类型变更

**典型场景**：
- spec 说"调用 A → 调用 B"，实际发现需要先调 `A0` 校验
- spec 说"参数是 string"，发现现有体系是 `int64`
- 验收标准里漏了一条容错场景

**处理（必须暂停当前 task）**：

1. 把当前改动 commit 到临时分支或 stash
2. tasks 偏离记录 status → `pending-decision`
3. **Dev Owner** 找 **Spec Author**（产品/Tech Lead）开 15 分钟短会
4. 决策三选一：
   | 决策 | 操作 |
   |------|------|
   | **a. 修订 spec** | 走 6.4 节流程，恢复实施 |
   | **b. 拒绝偏离** | tasks 状态 → `rejected`，按原 spec 实施 |
   | **c. 拆独立 spec** | 当前 spec 不变，新立 `specs/NNNN+1`；当前 task 标记暂停 |
5. 决策记录在 tasks 偏离记录的「处理建议」列

#### 🔴 大（escalate）—— FR 边界变化、新增/删除功能

**典型场景**：
- 实施时发现"用户已录掌"的判定要扩展到"任意手已录掌"
- 实施时发现 Phase 2 的功能其实可以拆成 3 个独立功能
- 实施时发现某个 FR 在现有架构下根本无法实现

**处理（停止整个 spec 实施）**：

1. 立即停止当前 spec 的所有实施
2. tasks 偏离记录 status → `escalated`
3. **重新走阶段 1**（Spec 评审会）
4. 决策三选一：
   | 决策 | 操作 |
   |------|------|
   | **a. 大改当前 spec** | spec status → `draft`，重新评审 → `ready` 后**重新生成 plan/tasks** |
   | **b. 拆为多个 spec** | 当前 spec 缩小范围；新功能立独立 spec |
   | **c. 暂停 spec** | spec status → `deprecated`，记录原因 |

### 6.4 Spec 修订（minor / major 决策为「修订 spec」时）

**步骤**：

1. **Spec Author** 修改 spec 文件：
   - 改对应章节
   - `Status` 改为 `in-progress`（说明仍在演进）
   - `Updated` 日期刷新
   - 在 spec **末尾「修订记录」**章节追加：
     ```markdown
     ## 修订记录
     | 日期 | 修订点 | 关联 tasks 偏离记录 | 修订人 |
     |------|--------|---------------------|--------|
     | 2026-06-08 | FR-3 增加 `userId` 校验场景 | T2.3 偏离记录 | evanye |
     ```

2. **Dev Owner** 评估 plan 是否需要联动改：
   - 如需 → 改 `plans/<VERSION>/<STORYID>-*-plan.md` 对应章节，刷新 `Updated`
   - 不需 → 跳过

3. **Dev Owner** 更新 tasks 文件：
   - 偏离记录的 status → `spec-updated`
   - 如新增 task → 在对应 Phase 末尾追加，标题加前缀 `[追加]`
   - 如某 task 已废弃 → 用 ~~T2.3~~ 划线，**不删除**（保留追溯）

4. 恢复实施

5. 全部 Phase 完成后，**强制**执行 `/spec-sync`：
   ```
   请对 specs/v1.6.0/10086-example-user-login.md 执行 spec-sync
   ```
   AI 会检查 spec / plan / tasks / 代码是否四方一致，输出同步报告。

### 6.5 PR 描述里怎么体现偏离

PR 描述模板里有「偏离说明」段落，这时候不能写"无"：

```markdown
## 偏离说明

实施过程中发现 2 处偏离，已回流到 spec：

1. T2.3 偏离：FR-3 漏了 `userId` 为空的校验场景
   - 严重度：中
   - 处理：spec 已修订（见 spec 修订记录 2026-06-08）
   
2. T3.1 偏离：测试场景命名与 spec 不一致
   - 严重度：小
   - 处理：spec 措辞已修订
```

### 6.6 速查：偏离处理决策树

```
我发现 spec 与现实不符
        │
        ├─ 只是措辞/命名/补充细节？
        │     └─ 🟢 继续实施 + tasks 记录 + Phase 末改 spec → spec-updated
        │
        ├─ 流程/参数变化？
        │     └─ 🟡 暂停 task + 找 Spec Author + 三选一决策
        │
        └─ 功能边界/FR 本身要改？
              └─ 🔴 停止 spec + 重走评审会 + 重新生成 plan/tasks
```

> 关键心法：**spec 是活文档**。偏离不是失败，沉默偏离才是失败。

---

## 7. 速查卡（打印贴显示器旁）

### 7.1 流程速查

```
任务来 → docs/intake/  → /spec-draft → specs/ (draft)
        （可选 /spec-intake 生成草稿）
                       → 人 review  → specs/ (ready)
                       → /spec-plan → plans/
                       → /spec-tasks→ tasks/
                       → /spec-impl → feature 分支 + src/
                       → /spec-test → tests/
                       → /spec-review→ 报告
                       → /spec-push → commit + safe rebase + push -f
                       → Git 平台（GitHub / GitLab / 工蜂） MR (三段式)
                       → MR 合并
                       → /spec-sync → 状态同步
```

### 7.2 命令速查

```bash
# 阶段 0 前置（可选）：PM 需求文档/描述 → intake 草稿
请基于 docs/产品需求-xxx.md 执行 spec-intake

# 阶段 0：原始需求 → spec 草稿
请基于 docs/intake/YYYY-MM-DD-xxx.md 执行 spec-draft

# 阶段 2：起 plan
请根据 specs/<VERSION>/<STORYID>-<slug>.md 执行 spec-plan

# 阶段 2.5：拆 tasks
请根据 specs/<VERSION>/<STORYID>-<slug>.md 执行 spec-tasks

# 阶段 3：实施（自动切基线 pull -r、拉 feature 分支）
请根据 specs/<VERSION>/<STORYID>-<slug>.md 执行 spec-implement

# 阶段 4：写测试
请根据 specs/<VERSION>/<STORYID>-<slug>.md 执行 spec-test

# 阶段 5 前：Review
请对 specs/<VERSION>/<STORYID>-<slug>.md 执行 spec-review

# 阶段 6：commit + 安全 rebase + push（生成提交、push -f、提示开 MR）
请对 specs/<VERSION>/<STORYID>-<slug>.md 执行 spec-push

# 阶段 6 后：MR 合并完成后 spec 状态同步
请对 specs/<VERSION>/<STORYID>-<slug>.md 执行 spec-sync
```

> Git 流程细节见 `docs/git-workflow.md`（基线分支映射、commit message 规范、安全 rebase 步骤）。

### 7.3 十条铁律（贴在工位）

1. 没有 Spec，不写代码
2. Plan 与 Tasks 必须沉淀为文件
3. 执行权 ≠ 定义权（AI 不能改 Spec）
4. PR 必须可追溯到 Story ID
5. 变更摘要不可省略
6. 实时勾选 tasks（不允许批量补）
7. 偏离必须记录
8. 分支名严格统一：`{feature|hotfix}/{spec-name}`，多仓库一致
9. Push 前必须安全 rebase（基线 pull -r → feature rebase 基线 → push -f）
10. Commit message 严格规范：`<type>(<scope>): <subject> --story=<id> [#finish]`

---

## 8. 你完成 Onboarding 的标志

- [ ] 读完 1.3 的 5 个文件
- [ ] 跑通 `0-example-feature` 全流程，所有产出文件齐全
- [ ] 完成第一个真实小任务并合并 MR（用 `/spec-push` + Git 平台（GitHub / GitLab / 工蜂） MR）
- [ ] 能给同事讲清楚 Rules / Skills / Commands 的关系
- [ ] 在 review 别人的 MR 时，能对照「十条铁律」给出建议
- [ ] 第二周开始，写下了你自己的「容易踩坑点」笔记

---

## 9. Q&A

**Q1：CodeBuddy 与 Claude Code 有什么区别？**  
A：CodeBuddy 是团队默认 IDE，通过 `.codebuddy/commands` 与 `.codebuddy/rules/rules` 软链直接读取根目录的 `commands/` 和 `rules/`。Claude Code 通过 `CLAUDE.md` 的 `@rules/*` 显式导入加载同一份规则。两者读取**同一份**源文件，体验一致。

**Q2：我能不能跳过 plan 或 tasks？**  
A：纯 bugfix（< 50 行）或纯文档修改可以跳。其他场景都不行。详见 `rules/10-spec-workflow.md` 的「流程例外」。

**Q3：Spec 改动了怎么办？**  
A：spec 的修改需要 review；通过后所有相关 plan/tasks 重新评估。如果只是补充非功能性细节，更新 `Updated` 日期即可。

**Q4：能否多人并行做同一个 Spec？**  
A：可以。每人认领 tasks 中的一个 Phase，在 tasks 文件中标注 owner。建议各自开 worktree 或子分支。

**Q5：tests/ 目录现在是空的，测试到底放哪？**  
A：业务测试放在 `src/` 各模块的同级目录（Go 惯例 `*_test.go`）；spec 流程产生的集成测试放 `tests/`。这是项目自身的约定，团队第一个真实 spec 实施时会建立样本。

**Q6：我发现 spec 有错误，怎么改？**  
A：**绝对不要**自己改。在 tasks 文件「偏离记录」中记下来，告诉 Spec Author，让他改。

**Q7：Skills 和 Commands 有什么区别？**  
A：Commands 是**人触发**的工作流入口（8 个）；Skills 是 AI **内部使用**的"操作手册"（7 个，含 codebase-survey 与 change-summary 两个工具型 skill）。一个 Command 通常会调用多个 Skill。

**Q8：为什么 CLAUDE.md 用 @ 导入规则？**  
A：让 Claude Code 在每次对话开始时自动加载所有规则文件，作为系统级背景指令。

**Q9：`src/` 为什么不在仓库里？**  
A：`src/` 已加 `.gitignore`——本仓库只管理 spec / plan / tasks / rules / skills / commands 这一套**协作元数据**，业务代码仓库由各人按需 clone 到 `src/<repo>/` 下。仓库地址与基线分支见 `docs/git-workflow.md`。

**Q10：change-summary 怎么没有 `/spec-summary` 命令？**  
A：变更摘要由 `/spec-review` 和 `/spec-push` 内部自动调用 `change-summary` skill 产出，不出独立命令。摘要会出现在 review 报告 + commit subject + MR 描述里，不会缺。

---

## 10. 下一步

读完本手册 → 开始第 3 章实操 → 完成后通知你的 mentor → 接第一个真实任务。

> 本手册是活文档，发现错误或不清楚的地方欢迎 PR。
