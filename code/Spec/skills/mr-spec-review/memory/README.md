# MR Spec Review 经验记忆库

本目录是 `mr-spec-review` skill 的**长期经验沉淀**，与主 SKILL.md 解耦，便于持续增长。

> 📌 **与 [`../review-guides/`](../review-guides/) 的分工**：
> - **本目录 `memory/`**：评审 loop 中**自动沉淀**的历史真问题模式（"踩过什么坑"），机器维护为主。
> - **`review-guides/`**：各端**稳定的评审基准 Checklist + 专家人工追加的专业注意点**（"这个端该看什么"），人工维护为主，按端分文件。
> - 评审时**两者都加载**：Step 4.0 按端加载 review-guides，Step 4.1/4.5 加载 memory，合并成检查清单。若某条 memory Pattern 明显是某个端专属且已稳定，可考虑迁进对应 review-guide。
>
> 🧪 **质量门禁 = [`EVAL-SPEC.md`](./EVAL-SPEC.md)（必读）**：本目录不是"resolved 就无脑追加"，而是走 **Eval 驱动的自净化闭环**——候选规则**入库前过自我评测门禁**（回测准确率 / 泛化 / 冲突 / 锚点），入库后按准确率、误报率、提升效果**定期净化淘汰**（过期/冲突/僵尸规则自动下架）。Pattern 的沉淀、格式、状态、淘汰**一律以 EVAL-SPEC 为准**。

## 目录内容

| 文件/目录 | 作用 |
|-----------|------|
| [`code-patterns.md`](./code-patterns.md) | ⭐ 首要：代码/方案层 Pattern |
| [`doc-process-patterns.md`](./doc-process-patterns.md) | 次要：文档/流程层 Pattern |
| [`EVAL-SPEC.md`](./EVAL-SPEC.md) | 🧪 经验闭环评测规格：提取/校验/沉淀 + 净化淘汰的完整机制与指标 |
| [`eval-corpus/`](./eval-corpus/) | 回测数据集：历史评审评论的结构化处置标注（TP/FP/FN） |
| [`attic.md`](./attic.md) | 归档区：淘汰/拒收/休眠的 Pattern（可审计、可恢复） |

## 文件结构

| 文件 | 优先级 | 主题 | 何时更新 |
|------|-------|------|---------|
| [`code-patterns.md`](./code-patterns.md) | ⭐ **首要** | 业务逻辑 / 技术方案 / 代码实现层面的经验 | 每次评审若发现新的方案层/代码层问题模式，追加 Pattern |
| [`doc-process-patterns.md`](./doc-process-patterns.md) | 次要 | 文档质量 / 流程合规 / 模板一致性层面的经验 | 每次评审若发现新的文档层问题模式，追加 Pattern |

## 为什么这样分层？

- **`code-patterns.md`（首要）**：直接影响**方案最终落地的准确性和合理性**。例如"未复用现有 hook"、"proto 字段号不一致"、"连接池反模式"——这些问题不修复，代码写出来就是坏味道或缺陷。是 review 的**核心价值**所在。
- **`doc-process-patterns.md`（次要）**：影响**协作效率与追溯性**，但不直接决定代码质量。例如"Sibling Specs 未列"、"TBD 编号跳号"、"Updated 日期未刷"——是必要卫生但优先级低。

## 使用方式（AI 执行 skill 时）

1. **进入 Step 4.5「代码验证评审」前**，必须**先读 `code-patterns.md`** 全文
2. **进入 Step 4.1「文档层评审」时**，必须**先读 `doc-process-patterns.md`** 全文
3. 逐条 Pattern 对照 spec/plan 扫描，命中即在评审报告中标注 `命中 code-patterns#N` 或 `命中 doc-patterns#N`
4. 评审完成后（Step 8.2 记忆沉淀）**走 [`EVAL-SPEC.md`](./EVAL-SPEC.md) 的 eval 门禁闭环**（概要如下，细则以 EVAL-SPEC 为准）：
   - **复评轮**：把"作者已接受并 `resolved`（且 commit 真的改了对应处）"的问题作为**候选**——这是被验证过的真问题模式（label=TP）。
   - **只有 TP 才是候选，FP 是负样本不能丢**：被作者拒绝/标记无效的评论（label=FP）**不沉淀**，但要**回灌为对应规则的误报统计**，用于净化淘汰（旧流程会直接丢弃，导致坏规则无法下架）。
   - **入库前过自我评测门禁**（EVAL-SPEC §5）：候选归一化成四元组 → 回测准确率 ≥0.8 + 泛化 ≥2 独立上下文 + 无冲突 + 锚点有效 → 才 `active`；数据不足/泛化不够 → `shadow` 试用；不达标 → `rejected`（记 `attic.md`）。
   - **定期净化淘汰**（EVAL-SPEC §7）：按准确率/误报率/锚点有效性/活性，对存量规则做过期、冲突、僵尸清理，淘汰项归档 `attic.md`。
   - **模式 A（本地手动）**：门禁结论 + metric 展示给用户，认可后写入。
   - **模式 B（CI/agent 自主）**：按门禁自动判定并写入，日志输出净化报告（EVAL-SPEC §10.3）。

## Pattern 条目格式（追加时遵循）

> 完整字段与状态语义见 [`EVAL-SPEC.md`](./EVAL-SPEC.md) §8。新沉淀一律用下方**含状态 + 统计**的增强格式。

```markdown
### Pattern N: [简短标题]

**状态**: active            <!-- active|shadow|stale|dormant|retired，见 EVAL-SPEC §2 -->
**统计**: 命中 8 · TP 6 · FP 1 · Precision 0.86 · 独立上下文 3 · 最近命中 2026-07-26   <!-- 净化扫描自动维护 -->
**首次沉淀**: {YYYY-MM-DD}，来源 MR {url}
**来源 MR**: {列出所有命中并 TP 的 MR}
**trigger（触发条件）**：<泛化描述，去硬编码——不写死具体符号名>
**probe（验证动作）**：<AI 要执行的确定性代码检索/文件读取>
**assertion（判定）**：<probe 结果满足什么才算命中真问题>
**为什么严重（可选）**：<后果是什么>
**fix（建议方向）**：<给作者的具体修改指引>
```

> 📌 **存量 Pattern（#1~#13 / D1~D8）无需一次性重写**：它们来自真实 resolved 问题，默认视为 `状态: active`，统计字段在下次净化扫描时惰性补齐（来源 MR 记为 1 个 TP 上下文）。

## 维护原则

> 完整量化门槛与淘汰逻辑见 [`EVAL-SPEC.md`](./EVAL-SPEC.md)。此处为要点速览：

- **只进好规则**：候选必须过入库门禁（回测 Precision≥0.8 + 泛化≥2 独立上下文 + 无冲突 + 锚点有效），不达标进 `shadow` 或 `rejected`。
- **自动清坏规则**：Precision 跌破维持门槛、误报累计、锚点失效、长期 0 命中、与他规则冲突 → 自动降级/淘汰（归档 `attic.md`，不删除）。
- **误报比漏报更伤信誉**：被作者反驳的评论要回灌为 FP 统计，驱动净化——绝不无视。
- **不重复**：语义 ≥80% 重合 → 合并，不新增（累加来源与统计）。
- **不臃肿**：单个 Pattern 描述 < 100 字；`dormant` 规则不加载，降低检查清单噪声。
- **命中率高 + 高准确 → 升级规则**：`distinct_contexts≥3` 且 `Precision≥0.85` 且 uplift 正向 → 提议升级为 `rules/` 项目级规则。
- **可追溯 / 可恢复**：每条 Pattern 带来源 MR 与统计；淘汰项归档可审计、同类重现可复活为 `shadow`。
