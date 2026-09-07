# CoSpec｜Spec 驱动 AI 协作开发

**一句话**：**不写"AI 自由发挥"的代码——每个改动都从一份 Spec 开始，走过 Plan、Tasks 三件套，由"人审 + AI 执行 + 人合并"完成。**

本 `docs/` 目录是 CoSpec 的**通用化文档矩阵**（不绑定特定业务 / 语言 / Git 平台 / 团队）。如果你想把这套方法用到自己的项目上，从这里读起。

想先了解「CoSpec 是什么、为什么、四理念、十铁律」→ 读 [`01-overview.md`](./01-overview.md)。

---

## 📚 文档矩阵

按建议阅读顺序：

| 顺序 | 文档 | 定位 | 阅读时长 |
| ---- | ---- | ---- | -------- |
| 1 | [`01-overview.md`](./01-overview.md) | **全景总览**——是什么、为什么、四理念、十铁律 | 10 分钟 |
| 2 | [`02-workflow.md`](./02-workflow.md) | **工作流详解**——8 阶段命令链逐段拆解 | 15 分钟 |
| 3 | [`03-principles.md`](./03-principles.md) | **原理与思想**——每条规则背后的"为什么" | 15 分钟 |
| 4 | [`04-how-to-use.md`](./04-how-to-use.md) | **使用手册**——三种规模场景 + FAQ | 20 分钟 |
| 5 | [`05-architecture.md`](./05-architecture.md) | **目录与文件契约**——每份模板的详细字段 | 15 分钟 |
| 6 | [`06-adaptation.md`](./06-adaptation.md) | **落地你自己的项目**——克隆 / 清洗 / 定制 / CI | 20 分钟 |
| 7 | [`07-case.md`](./07-case.md) | **完整案例导读**——五件套活样例的索引与教学要点 | 10 分钟 |

**总计**：约 100 分钟通读全部，或按需选读。

---

## 🎯 三种典型读者的推荐路径

### 想快速判断「值不值得用」

**读**：01（10 分钟）→ 决定要不要深入。

### 想上手做实事（最实用）

**读**：01 → 04（30 分钟）→ 挑一个真实需求跑一遍。

### 想把 CoSpec 引入自己的团队

**读**：01 → 03 → 06（45 分钟）→ 按 06 的 Pilot 流程推广。

---

## 📦 完整案例（照着写的活样例）

一个虚构但完整的「短链接服务」需求（Story 1001），从 Intake → Design → Spec → Plan → Tasks 走完全流程。五份样本文件**直接存放在各产物目录的真实位置**（`v1.0.0/`），导读见 [`07-case.md`](./07-case.md)：

| 文件（真实位置） | 展示什么 |
| ---------------- | -------- |
| [`intake/v1.0.0/1001-short-url-service.md`](../intake/v1.0.0/1001-short-url-service.md) | 原话保留、待澄清点清单 |
| [`designs/v1.0.0/1001-short-url-service-design.md`](../designs/v1.0.0/1001-short-url-service-design.md) | 3 方案对比、关键决策、spec 拆分建议 |
| [`specs/v1.0.0/1001-short-url-core.md`](../specs/v1.0.0/1001-short-url-core.md) | 完整 spec 实例、来源标注、Sibling Specs |
| [`plans/v1.0.0/1001-short-url-core-plan.md`](../plans/v1.0.0/1001-short-url-core-plan.md) | 改动文件清单、Phase 拆分、验收覆盖矩阵 |
| [`tasks/v1.0.0/1001-short-url-core-tasks.md`](../tasks/v1.0.0/1001-short-url-core-tasks.md) | 任务勾选终态、偏离记录（含 spec 修订回流） |

**新手上手路径**：读本 README → 读 `01-overview.md` → 读 `07-case.md` 并通读其五件套样本（约 30 分钟）→ 挑一个真实小需求照着写。

---

## 📖 延伸阅读

方法论文档读完后，再看具体实现：

- [`rules/10-spec-workflow.md`](../rules/10-spec-workflow.md)（工作流的完整定义）
- [`commands/spec-*.md`](../commands/)（每个命令的详细执行步骤）
- [`skills/*/SKILL.md`](../skills/)（技能的执行逻辑）
- [`git-workflow.md`](./git-workflow.md)（Git 专项操作手册）

**顺序建议**：先理解思想，再看细节。

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

**开始阅读** → [`01-overview.md`](./01-overview.md)
