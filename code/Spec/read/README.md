# CoSpec：Spec 驱动 AI 协作开发工作空间

> 一句话：**我们不写"AI 自由发挥"的代码**——每个改动都从一份 Spec 开始，经 Plan、Tasks 三件套沉淀，由「人审 + AI 执行 + 人合并」。
>
> 团队 · `<GIT-HOST>:<ORG>/CoSpec.git` · 团队索引：团队 Wiki 4022732388

---

## 一、是什么

### 定位

你的团队的 **Spec 驱动 AI 协作开发工作空间**。

**最反直觉的一点：这个仓库不存业务代码。** `src/` 在 `.gitignore` 里，各人按 spec 需要自行 clone 业务仓库进去。CoSpec 只管理「协作元数据」——Spec / Plan / Tasks / 规则 / 技能 / 命令 / 文档。

这么设计是因为协作方式与业务代码的变更频率、审阅对象完全不同：规则改一次全队生效，业务代码则散落在 <业务主仓库> / <协议仓库> / <设备接入仓库> 等多个仓库，靠统一的 Story ID 和分支名串联。

### 三件套（骨架）

| 类型 | 目录 | 回答什么 | 模板 |
|------|------|---------|------|
| **Spec** | `specs/` | 要做什么 | `specs/templates/spec-template.md` |
| **Plan** | `plans/` | 怎么做、改哪些文件 | `plans/templates/plan-template.md` |
| **Tasks** | `tasks/` | 分几步、做到哪了 | `tasks/templates/tasks-template.md` |
| Design（可选） | `designs/` | 用什么方案、拆几个 spec | `designs/templates/design-template.md` |

**三件套必须落盘，不允许只存在于对话里**——AI 的上下文会丢、会换会话、会换人，只有磁盘文件能跨这些边界存活。

**Design 与 Plan 的区别**（最易混淆的一对）：

> **Design 决定「用什么方案、拆几个 spec」，Plan 决定「改哪些文件、按什么步骤」。**

Design 在 spec 之前（方案级、可选），Plan 在 spec ready 之后（执行级、复杂改动必须）。

---

## 二、为什么

### 解决什么问题

传统 vibe coding 的四个硬伤：

| 问题 | 表现 |
|------|------|
| 结果不可复现 | 不同人 / 不同时间 / 不同 Agent 问，产出差异大 |
| 改动范围漂移 | 一句"顺便优化"，可能顺手改十几个文件 |
| 文档与代码脱节 | 代码改完了，需求文档还是三个月前的 |
| 改动难追溯 | 半年后看 MR，说不清当初为什么这么改 |

共同根源：**需求只存在于人的脑子里和 AI 的对话里**。

解法：流程切段 → 每段产出磁盘文件 → 设人工卡口 → Story ID 全程串联。

### 四条核心理念

1. **Spec 是单一事实来源** —— 需求不留在 IM / 邮件 / 脑子里
2. **三件套必须落盘** —— 不允许只在对话中存在
3. **执行权 ≠ 定义权** —— AI 只执行，不能改 Spec；有意见只能在 tasks「偏离记录」提建议，**由人决策**
4. **全程靠 Story ID 串联** —— spec → 分支 → commit → MR

### 十铁律

无 spec 不写代码 · 三件套落盘 · AI 不改 spec · MR 带 StoryID · 变更摘要必写 · tasks 实时勾 · 偏离必记录 · 分支统一 · push 前 rebase · commit 规范

### 价值对比

| | 直接 vibe coding | CoSpec |
|---|---|---|
| 需求在哪 | 对话里，关掉就没 | spec 文件，可评审可追溯 |
| 谁定需求 | AI 边写边猜 | 人写 spec，AI 只能提建议 |
| 改动范围 | 取决于 AI | Plan 列明文件清单，超出即偏离 |
| 进度 | "差不多做完了" | Tasks 实时勾选 |
| 换人接手 | 基本重做 | 读三件套即可 |

代价：多了文档工作量。因此做了分级——小需求可直接从 `/spec-draft` 起步，Intake / Design / Push 都是可选。

---

## 三、怎么用

### 工作流

```
(Intake) → (Design) → Draft → 个人review → 提交+同事评审 → Plan → Tasks
   → Implement → Test → Review → (Push) → 合并 → Sync
```

**唯一强制卡口**：spec 写完必须提交、由**其他技术同事**评审通过，才能进 plan / 实现。个人 review 不能代替同事评审。

### 命令清单

| 阶段 | 命令 | 输出 |
|------|------|------|
| 0 前置（可选） | `/spec-intake` | `docs/intake/<VER>/<ID>-<slug>.md` |
| 0 前（可选） | `/spec-design` | `designs/<VER>/<ID>-<slug>-design.md` |
| 0 | `/spec-draft` | `specs/<VER>/<ID>-<slug>.md`（status: draft） |
| 1 | **（人工评审）** | status 改 `ready` |
| 2 / 2.5 | `/spec-plan` `/spec-tasks` | plan / tasks 文件 |
| 3 | `/spec-implement` | feature 分支 + `src/` 改动 |
| 4 / 5 | `/spec-test` `/spec-review` | 测试 / review 报告 |
| 6（可选） | `/spec-push` | commit + 安全 rebase + push |
| 6 后 | `/spec-sync` | 状态同步 |

> `/spec-index` 是**带外工具**：扫描 specs 生成索引并覆盖同步到 团队 Wiki。**不属于个人流程**，由专人按需运行——人人都跑会把半成品状态覆盖上去。

### 快速开始

```bash
# 1. 拉仓库
git clone <GIT-HOST>:<ORG>/CoSpec.git spec && cd spec

# 2. 按需 clone 业务代码（src/ 已 gitignore）
mkdir -p src && cd src
git clone <GIT-HOST>:<ORG>/<业务主仓库>.git   # 业务主仓（基线 develop）
git clone <GIT-HOST>:<ORG>/<协议仓库>.git   # proto 仓（基线 master）

# 3. 走命令链
/spec-draft      # 起草（AI 先做代码侦察，再澄清五类问题）
# 个人 review → 提交 → 同事评审 → status: ready
/spec-plan       # 实施计划
/spec-tasks      # 任务清单
/spec-implement  # 实现（tasks 实时勾选）
/spec-test       # 测试
/spec-review     # 评审报告
/spec-push       # 可选：提交
```

起草时每个章节会标来源，便于判断需补哪些：📥 原始需求 / 🤖 AI 推断（需确认）/ ❓ TBD / 🔍 现有代码。

### 命名与 Git

```
docs/intake/<VER>/<STORYID>-<slug>.md
designs/<VER>/<STORYID>-<slug>-design.md
specs/<VER>/<STORYID>-<slug>.md
plans/<VER>/<STORYID>-<slug>-plan.md
tasks/<VER>/<STORYID>-<slug>-tasks.md
```

- `STORYID` 纯数字，无 story 用 `0` 占位；`slug` 用 kebab-case
- **三件套的 STORYID + slug 必须一致**（互相定位的依据）
- 按迭代版本归档到 `<VER>/`（如 `v2.0.0/`）；`templates/`、`README.md` 是跨版本元文件，留在目录根
- 早期 `NNNN-<STORYID>-<slug>` 顺序号形式**已废弃**（多人并行易冲突）

| 项 | 规范 |
|----|------|
| 分支 | `{feature\|hotfix}/<spec-name>`（= spec 文件名去 `.md`），**跨仓库同名** |
| Commit | `<type>(<scope>): <subject> --story=<STORYID> [#finish]`（`#finish` 仅加在最后一笔） |
| Push 前 | 安全 rebase：基线 `git pull -r` → feature `git rebase 基线` → `git push -f` |
| MR 模板 | `.gitlab/merge_request_templates/Default.md`（关联 Spec / 偏离说明 / 变更摘要） |

---

## 四、规则与技能

**规则**（常驻，经 `CLAUDE.md` 的 `@rules/...` 导入）：

| 文件 | 主题 |
|------|------|
| `00-project-principles.md` | 7 条原则（地基） |
| `10-spec-workflow.md` | **核心工作流**（611 行，7 个阶段） |
| `20-coding-rules.md` | 编码规则 |
| `30-testing-rules.md` | 测试规则 |
| `40-documentation-rules.md` | 文档规则 |

`00` 里有两条直接对抗 AI「顺手改」倾向的原则，值得单独记：

- **最小改动** —— 不做"顺手"的优化 / 重构 / 风格调整；发现其他问题**记录下来但本次不改**
- **不做无关重构** —— 重构是**独立的 spec 主题**，不能夹带在功能需求里

**技能**（按需调用）：

| 核心 8 个 | 用途 |
|-----------|------|
| `technical-design` | 需求大时先出技术方案（含 spec 拆分建议） |
| `spec-drafting` | 原始需求 → spec 草稿 |
| `spec-analysis` | 分析已有 spec 完整性 |
| `codebase-survey` | 扫描现有代码（light 给 drafting，deep 给 planning） |
| `implementation-planning` | 制定实施计划 |
| `feature-implementation` | 执行代码实现 |
| `test-writing` | 编写测试 |
| `change-summary` | 变更摘要（内部调用，非独立命令） |

另有 6 个扩展技能（文档未同步）：`mr-spec-review`、`mr-review-resolve`、`<团队专属技能>`、`<团队专属技能>`、`<团队专属技能>`、`skill-review`。

> `codebase-survey` 的存在理由写得很到位：**「AI 没看代码就起草 spec/plan，本质是另一种 vibe coding。」**

---

## 五、现状与待办

**模板完备，但只跑通了一条链路**：

| 目录 | 真实实例 |
|------|---------|
| intake / designs / specs | 各 1 个（Story **10086** O4 网络诊断措施） |
| plans / tasks | **0 个** |

该 spec 状态为 `draft`（尚未评审到 ready，故 plan / tasks 未生成），涉及 <终端应用仓库> / <管理后台仓库> / <设备接入仓库> 三仓。

**通读发现的 3 处不一致**：

| # | 问题 |
|---|------|
| 1 | 技能数量三处打架：README 说 8 个、`CLAUDE.md` 列 10 个、磁盘实际 **14 个** |
| 2 | `specs/templates/spec-template.md`「修订记录」章节被**重复粘贴两次**（126-133 / 135-142 行） |
| 3 | 文档引用的历史 spec（v1.6.0 / v1.7.0）在工作副本上不存在 |

---

## 延伸阅读

| 顺序 | 文件 | 时间 |
|------|------|------|
| 1 | 本文（全局视图） | 10 分钟 |
| 2 | `docs/spec-flow-overview.md`（流程图 + 每步关注点） | 5 分钟 |
| 3 | `docs/spec-coding-handbook.md`（一页纸 10 铁律） | 5 分钟 |
| — | `docs/onboarding-codebuddy.md`（新人完整手册，含实操与自救） | 30 分钟 |
| — | `docs/git-workflow.md`（基线分支 / commit / rebase / 多仓库） | 按需 |

---

## 速查卡

```
三件套    Spec 做什么 / Plan 怎么做 / Tasks 做到哪
四理念    单一事实来源 · 必须落盘 · 执行权≠定义权 · Story ID 串联
十铁律    无spec不写代码 · 三件套落盘 · AI不改spec · MR带StoryID · 变更摘要必写
          tasks实时勾 · 偏离必记录 · 分支统一 · push前rebase · commit规范

命令链    intake? → design? → draft →【评审→ready】→ plan → tasks
          → implement → test → review → push? → sync

分支      {feature|hotfix}/<spec-name>   跨仓库同名
提交      <type>(<scope>): <subject> --story=<STORYID> [#finish]
状态机    draft → ready → in-progress → implemented → deprecated
索引      团队 Wiki 4022732388（/spec-index 带外发布，个人勿跑）
```
