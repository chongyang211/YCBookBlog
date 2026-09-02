# CoSpec｜Spec 驱动 AI 协作开发

**一句话**：**不写"AI 自由发挥"的代码——每个改动都从一份 Spec 开始，走过 Plan、Tasks 三件套，由"人审 + AI 执行 + 人合并"完成。**

本 `docs/` 目录是 CoSpec 的**通用化文档矩阵**（不绑定特定业务 / 语言 / Git 平台 / 团队）。如果你想把这套方法用到自己的项目上，从这里读起。

---

## 📚 文档矩阵

按建议阅读顺序：

| 顺序 | 文档                                          | 定位                                          | 阅读时长 |
| ---- | --------------------------------------------- | --------------------------------------------- | -------- |
| 1    | [`01-overview.md`](./01-overview.md)          | **全景总览**——是什么、为什么、四理念、十铁律 | 10 分钟  |
| 2    | [`02-workflow.md`](./02-workflow.md)          | **工作流详解**——8 阶段命令链逐段拆解         | 15 分钟  |
| 3    | [`03-principles.md`](./03-principles.md)      | **原理与思想**——每条规则背后的"为什么"       | 15 分钟  |
| 4    | [`04-how-to-use.md`](./04-how-to-use.md)      | **使用手册**——三种规模场景 + FAQ            | 20 分钟  |
| 5    | [`05-architecture.md`](./05-architecture.md)  | **目录与文件契约**——每份模板的详细字段       | 15 分钟  |
| 6    | [`06-adaptation.md`](./06-adaptation.md)      | **落地你自己的项目**——克隆 / 清洗 / 定制 / CI | 20 分钟  |
| 7    | [`07-case.md`](./07-case.md)                  | **完整案例导读**——五件套活样例的索引与教学要点 | 10 分钟 |

**总计**：约 100 分钟通读全部，或按需选读。

### 📦 完整案例（照着写的活样例）

[`07-case.md`](./07-case.md)——一个虚构但完整的「短链接服务」需求（Story 1001），从 Intake → Design → Spec → Plan → Tasks 走完全流程。五份样本文件**直接存放在各产物目录的真实位置**（`v1.0.0/`），导读见 [`07-case.md`](./07-case.md)：

| 文件（真实位置） | 展示什么 |
| ---------------- | -------- |
| [`intake/v1.0.0/1001-short-url-service.md`](../intake/v1.0.0/1001-short-url-service.md) | 原话保留、待澄清点清单 |
| [`designs/v1.0.0/1001-short-url-service-design.md`](../designs/v1.0.0/1001-short-url-service-design.md) | 3 方案对比、关键决策、**spec 拆分建议** |
| [`specs/v1.0.0/1001-short-url-core.md`](../specs/v1.0.0/1001-short-url-core.md) | 完整 spec 实例、来源标注（📥🤖🔍❓）、Sibling Specs |
| [`plans/v1.0.0/1001-short-url-core-plan.md`](../plans/v1.0.0/1001-short-url-core-plan.md) | 改动文件清单、Phase 拆分、验收覆盖矩阵 |
| [`tasks/v1.0.0/1001-short-url-core-tasks.md`](../tasks/v1.0.0/1001-short-url-core-tasks.md) | 任务勾选终态、**偏离记录**（含 spec 修订回流） |

**新手上手路径**：读本 README → 读 `01-overview.md` → 读 `07-case.md` 并通读其五件套样本（约 30 分钟）→ 挑一个真实小需求照着写。

---

## 🎯 三种典型读者的推荐路径

### 想快速判断"值不值得用"

**读**：01（10 分钟）→ 决定要不要深入。

### 想上手做实事（最实用）

**读**：01 → 04（30 分钟）→ 挑一个真实需求跑一遍。

### 想把 CoSpec 引入自己的团队

**读**：01 → 03 → 06（45 分钟）→ 按 06 的 Pilot 流程推广。

---

## 🧠 核心思想速记

### 三件套（骨架）

| 类型  | 目录     | 回答什么           |
| ----- | -------- | ------------------ |
| Spec  | `specs/` | 要做什么           |
| Plan  | `plans/` | 怎么做、改哪些文件 |
| Tasks | `tasks/` | 分几步、做到哪了   |

### 四理念（心法）

1. **Spec 是单一事实来源** —— 需求不留在 IM / 邮件 / 脑子里
2. **三件套必须落盘** —— 不允许只在对话中存在
3. **执行权 ≠ 定义权** —— AI 只执行，不改 Spec
4. **全程用 Story ID 串联** —— spec → 分支 → commit → MR

### 十铁律（可执行约束）

无 spec 不写代码 · 三件套落盘 · AI 不改 spec · MR 带 Story ID · 变更摘要必写 · tasks 实时勾 · 偏离必记录 · 分支统一 · push 前 rebase · commit 规范

### 8 阶段命令链

```txt
(Intake) → (Design) → Draft → 【人工评审】 → Plan → Tasks
      → Implement → Test → Review → (Push) → Sync
```

**唯一强制人工卡口**：spec draft → 其他技术同事评审通过 → 才能进入 Plan / Implement。

---

## 🚀 60 秒快速上手

```bash
# 1. 克隆本仓（作为骨架）
git clone <YOUR-COSPEC-REPO> my-project && cd my-project

# 2. 初始化空的元数据目录
mkdir -p specs/v1.0.0 plans/v1.0.0 tasks/v1.0.0 \
         intake/v1.0.0 designs/v1.0.0

# 3. 让 AI 助手识别 rules（Cursor / Codebuddy / Continue 等）
#    详见 06-adaptation.md § 二

# 4. 起草第一个 spec
/spec-draft 帮我起草一个 spec：<你的需求>

# 5. 人审后进入实施
/spec-plan → /spec-tasks → /spec-implement → /spec-sync
```

**详细起步指南**：见 [`04-how-to-use.md`](./04-how-to-use.md) 的"上手起步的 15 分钟"章节。

---

## ✨ CoSpec 与传统开发的对比

| 维度              | 直接 vibe coding      | CoSpec                                   |
| ----------------- | --------------------- | ---------------------------------------- |
| 需求存在哪里      | 对话里，关掉就没      | **磁盘 spec 文件**，可评审可追溯         |
| 谁定需求          | AI 边写边猜           | **人写 spec，AI 只能提建议**             |
| 改动范围可控性    | 取决于 AI 心情        | **Plan 列明文件清单，超出即偏离**        |
| 进度追踪          | "差不多做完了"        | **Tasks 实时勾选，可 diff**              |
| 换人接手成本      | 基本重做              | **读三件套即可**                         |
| 半年后可追溯性    | 极差                  | **Story ID 一键回溯全部产物**            |

**唯一代价**：多了文档工作量（15-30 分钟/需求）。**收益**：半年后省下的考古时间就足以覆盖。

---

## 🛠 CoSpec 里包含什么

除了本 `docs/` 下的通用化文档矩阵，仓库还提供：

| 目录           | 内容                                     |
| -------------- | ---------------------------------------- |
| `rules/`       | 5 份常驻规则（AI 每次会话都加载）        |
| `commands/`    | 10 份 Slash Command 定义                 |
| `skills/`      | 8+ 份按需加载的技能                      |
| `specs/`       | Spec 模板 + 实例                         |
| `plans/`       | Plan 模板 + 实例                         |
| `tasks/`       | Tasks 模板 + 实例                        |
| `designs/`     | Design 模板 + 实例（可选前置）           |
| `intake/`      | Intake 模板 + 实例（可选前置）           |
| `tests/`       | 测试代码                                 |

**详细目录说明**：见 [`05-architecture.md`](./05-architecture.md)。

---

## ❓ 常见问题

### 这需要特定 AI 助手吗？

**不需要**。任何支持"加载项目级规则文件 + 定义 slash command"的 AI 助手都能用（Cursor / Codebuddy / Continue / Cline / Cody 等）。见 [`06-adaptation.md`](./06-adaptation.md) § 二的适配说明。

### 需要特定编程语言吗？

**不需要**。CoSpec 是语言无关的——它约束的是"人 + AI 协作方式"，不约束具体代码写法。

### 需要特定 Git 平台吗？

**不需要**。GitHub / GitLab / Gitee / Bitbucket / 自建 Git 都可以。CI 校验示例在 [`06-adaptation.md`](./06-adaptation.md) 提供 GitHub Actions 版本，可移植到其他平台。

### 小项目也能用吗？

**能**。CoSpec 是分级使用的——小需求可只用 `/spec-draft` + `/spec-implement` + `/spec-sync`，跳过 intake / design / plan / tasks / push。详见 [`04-how-to-use.md`](./04-how-to-use.md) § 二的"场景 A：极简需求"。

### 与 Jira / Trello 等冲突吗？

**不冲突**——CoSpec 与需求管理系统互补：

- Jira / Trello 管理需求生命周期
- CoSpec 管理"从需求到代码"的执行契约
- 用 Jira Story ID 作 CoSpec Story ID → 天然打通

---

## 📖 延伸阅读

如果你熟悉后想更深入：

- 阅读 `rules/10-spec-workflow.md`（工作流的完整定义，611 行）
- 阅读各 `commands/spec-*.md`（每个命令的详细执行步骤）
- 阅读各 `skills/*/SKILL.md`（技能的执行逻辑）
- 阅读 [`git-workflow.md`](./git-workflow.md)（Git 专项操作手册）

**顺序建议**：先把本 `docs/` 目录的 7 篇方法论文档读完，再看具体命令 / 技能 / 工具实操——**先理解思想，再看细节**。

---

## 📁 docs 目录的用途与规范

`docs/` 存放与 spec 互补的补充文档，包括：

- **架构决策记录（ADR）**：为什么选择某种技术方案
- **模块设计文档**：复杂模块的内部设计说明
- **部署 / 运维说明**：如何构建、部署、运行项目
- **接口文档**：API 使用说明
- **会议 / 讨论记录**：影响设计的重要讨论结论（如 `v1-week2-feedback.md` 团队反馈问卷）

**与 Spec 的区别**：spec 描述"要做什么"（功能需求、验收标准）；docs 描述"为什么这样做"和"怎样运行 / 维护"。

**什么时候更新 docs**：架构或技术方案重大变更、部署流程变化、新增重要外部依赖、接口 breaking change、团队协作约定变更时。

---

## 🎁 一份给自己的礼物

**当你半年后回来看当初写的代码，只需 5 分钟就能找回全部上下文——这就是 CoSpec 送给未来自己的礼物。**

---

**开始阅读** → [`01-overview.md`](./01-overview.md)
