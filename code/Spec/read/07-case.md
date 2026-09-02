# 07｜完整案例：短链接服务（Intake → Design → Spec → Plan → Tasks 全流程活样例）

> 📖 **本篇定位**：一个**虚构但完整**的需求，从原始需求到任务清单走完 CoSpec 全流程的活样例导读。
> 案例刻意选用中性的「短链接服务」——不绑定任何业务领域，任何团队都能看懂。
> 五份样本文件**直接存放在各产物目录的真实位置**（`v1.0.0/` 版本目录下），可对照模板阅读、照着写。
> 🔗 **上一篇**：[06 落地你自己的项目](./06-adaptation.md) · **回到系列首页**：[README](./README.md)

---

## 一、案例背景（30 秒）

某内容平台（博客 / 资讯类）收到运营需求：**文章 URL 太长，分享到社交平台会被截断；同时希望统计分享后的点击量**。

这是一个**中等偏大**的需求（要新建后端服务、涉及统计、跨端分享入口），因此走完整链路：

```txt
Intake（原始需求落盘）
  → Design（技术方案评审：短码方案怎么选？拆几个 spec？）
  → Spec × 2（Design 建议拆 2 个子 spec，共享 Story ID）
  → Plan（本案例展示 core 子 spec 的实施计划）
  → Tasks（实施任务清单，含偏离记录样例）
```

## 二、五件套的真实位置

统一设定：**Story ID = 1001**（示例编号；真实场景用你团队需求系统的编号）、**版本 = v1.0.0**。

| 顺序 | 文件（真实路径） | 展示什么 |
| ---- | ---------------- | -------- |
| 1 | [`intake/v1.0.0/1001-short-url-service.md`](../intake/v1.0.0/1001-short-url-service.md) | 原话保留、frontmatter 溯源、7 个待澄清点 |
| 2 | [`designs/v1.0.0/1001-short-url-service-design.md`](../designs/v1.0.0/1001-short-url-service-design.md) | 3 方案对比（含淘汰理由）、6 项关键决策、**spec 拆分建议**、评审记录 |
| 3 | [`specs/v1.0.0/1001-short-url-core.md`](../specs/v1.0.0/1001-short-url-core.md) | 完整 spec 模板实例、来源标注（📥🤖🔍❓）、Sibling Specs、9 条可验证 AC |
| 4 | [`plans/v1.0.0/1001-short-url-core-plan.md`](../plans/v1.0.0/1001-short-url-core-plan.md) | 改动文件清单（超出即偏离）、4 Phase、验收标准覆盖矩阵 |
| 5 | [`tasks/v1.0.0/1001-short-url-core-tasks.md`](../tasks/v1.0.0/1001-short-url-core-tasks.md) | 22 项任务勾选终态、**偏离记录**（含 spec 修订回流） |

## 三、本案例刻意展示的 8 个关键机制

读五份文件时，重点留意这些机制（这是案例的教学价值所在）：

| # | 机制 | 在哪看 |
| - | ---- | ------ |
| 1 | **Intake 保留原话**——口语化、有歧义，但**不改写** | intake 的「原始需求（原话）」 |
| 2 | **Design 给 3 个候选方案**，淘汰理由写清楚，不许只给单一方案 | design 的「方案对比」 |
| 3 | **Design 的核心产出是 spec 拆分建议**——大需求拆 2 个子 spec | design 的「建议的 spec 拆分」 |
| 4 | **多 spec 共享 Story ID**：两个子 spec 都叫 1001，靠 slug 区分 | spec frontmatter `Sibling Specs` |
| 5 | **Spec 每个章节标来源**：📥 原始需求 / 🤖 AI 推断 / 🔍 现有代码 / ❓ TBD | spec 全文行内标注 |
| 6 | **验收标准可勾选、可验证**——不写"性能良好"这种模糊话 | spec 的「验收标准」 |
| 7 | **Plan 列明改动文件清单**——超出清单即偏离 | plan 的「改动范围」 |
| 8 | **偏离必记录**：一条"文件超清单"偏离 + 一条"建议 spec 修订"偏离 | tasks 的「偏离记录」 |

## 四、五件套的时间线（这个案例发生了什么）

```txt
Day 1  运营在 IM 提需求 → /spec-intake 落盘（intake 文件，Status: draft）
Day 2  /spec-design：澄清 5 个问题 → 3 方案对比 → 团队评审通过（design，approved）
Day 3  /spec-draft：按 design 拆分建议起草 2 个子 spec
       - 1001-short-url-core（本案例的 spec 文件）
       - 1001-short-url-stats（未展示，结构相同）
Day 4  同事评审两个 spec → 补 TBD → Status: ready
Day 5  /spec-plan → /spec-tasks
Day 5-8  /spec-implement：切分支 feature/1001-short-url-core，
         按 tasks 逐项勾选；期间产生 2 条偏离记录
Day 8  /spec-test → /spec-review → MR 合并 → /spec-sync（Status: implemented）
```

## 五、小需求怎么办？

本案例是**中大型需求**的完整走法。小需求（改几行代码）不需要这么多文件：

```txt
/spec-draft → 人审 → /spec-implement → /spec-sync
```

intake / design / plan / tasks 都可跳过——详见 [`04-how-to-use.md`](./04-how-to-use.md) 的「场景 A：极简需求」。

## 六、上手练习（推荐）

1. 读一遍五份文件（约 20 分钟）
2. 挑你自己项目里的一个**真实小需求**
3. 只写 spec 一个文件（复制 `specs/templates/spec-template.md` 到 `specs/<VERSION>/`）
4. 找一位同事评审，把 Status 改 ready
5. 实施、勾验收标准、sync

完成这一轮，你就掌握了 CoSpec 的最小闭环。

---

**开始阅读** → [`intake/v1.0.0/1001-short-url-service.md`](../intake/v1.0.0/1001-short-url-service.md) · 返回 [`README.md`](./README.md)
