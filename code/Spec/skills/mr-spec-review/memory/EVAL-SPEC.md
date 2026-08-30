# Spec 评审经验沉淀与反哺 · Eval Spec

> 本文件是 `mr-spec-review` skill 的**经验沉淀与反哺闭环**的评测规格（Eval Spec）。
> 它规定：如何从历史评审结果中**提取 → 校验 → 沉淀**有效经验规则（Pattern），以及规则**入库前的自我评测门禁**与**冲突/过期规则的自动净化淘汰**逻辑。
>
> 📌 定位：`memory/README.md` 讲"记忆库是什么、怎么用"；本文件讲"记忆库如何在 eval 门禁下**只进好规则、自动清坏规则**"，是让 skill "越用越准、而不是越用越噪"的核心机制。
>
> 🤖 **执行主体是 AI agent**（非独立 ML 管道）。下述所有 metric 计算、回测、冲突检测都设计为 **LLM + Git 平台（GitHub / GitLab / 工蜂） MCP 可执行**：数据来自 `search_merge_request_notes` 的 `resolve_state` 与作者回复，判定靠 agent 的语义对齐 + `grep` 锚点校验。

---

## 0. 为什么需要这套 Eval（现状问题）

现有 Step 8.2 直接把"作者已 `resolved` 的评论"沉淀成 Pattern，存在系统性风险：

| 风险 | 说明 | 本 Eval 的对策 |
|------|------|---------------|
| **过拟合** | "作者改了"≠"可泛化规则"，可能只是一次性笔误/本 MR 独有 | 泛化门禁（§5.2）：要求 ≥2 独立上下文命中 |
| **误报累积** | Pattern 入库后永久生效，后续频繁被作者反驳也不下架 | 准确率追踪 + 误报淘汰（§7.2） |
| **规则过期** | Pattern 硬编码代码锚点，代码重构后变成误导 | 锚点有效性扫描 + 过期淘汰（§7.3） |
| **规则冲突** | 两条 Pattern 互相矛盾（如"复用 X" vs "X 已废弃") | 冲突检测（§5.3）+ 冲突淘汰（§7.4） |
| **记忆膨胀** | 只增不减，检查清单越来越长、命中信噪比下降 | 僵尸归档（§7.5）+ 合并（§7.6） |
| **信号单一** | 只有"3 MR 命中→升级"正向信号，无负向淘汰 | 完整生命周期状态机（§2） |

---

## 1. 目标与范围

**目标**：把"经验沉淀"从"无门禁的追加"升级为**可评测、自净化的闭环**，保证记忆库中每条 active 规则都满足：**高准确率、可泛化、锚点有效、无冲突、对后续评审有正向提升**。

**范围**：作用于 `memory/code-patterns.md` 与 `memory/doc-process-patterns.md` 里的 Pattern。**不改动** `review-guides/` 的人工注意点（那是专家主动维护、不走自动 eval，但可参与冲突检测）。

**非目标**：不引入外部 ML 训练/独立服务；不追求统计学严格的置信区间（样本量小，用工程化阈值 + 人工兜底）。

---

## 2. 核心概念：规则生命周期状态机

每条 Pattern 有明确**状态**，只有 `active` 才以完整置信度参与评审投递。

```
   [候选 candidate]  ← 从 resolved 评论提取
        │  §4 归一化 + §5 自我评测门禁
        ├── Precision≥0.8 且 泛化通过 且 无冲突 且 锚点有效 ──→ [active 生效]
        ├── Precision≥0.6 但 数据不足/泛化未达标 ─────────────→ [shadow 试用]
        └── Precision<0.6 或 严重冲突 ────────────────────────→ [rejected 拒收]（记审计，不入库）

   [shadow 试用] ──观察 N 次评审, 累计达标──→ [active]
                └──持续不达标──────────────→ [rejected/archived]

   [active 生效] ──§7 定期净化──┬─ Precision 跌破 0.6 ───→ [shadow 降级]
                               ├─ FP≥K 或 Precision<0.4 ─→ [retired 淘汰]
                               ├─ 锚点失效 ──────────────→ [stale 待重锚]
                               ├─ 长期 0 命中 ────────────→ [dormant 休眠→归档]
                               └─ 与他规则冲突 ──────────→ 保高汰低

   [stale] ──重锚定成功──→ [active]；──无法重锚──→ [retired]
   [retired]/[archived] → 移入 memory/attic.md（不删除，可审计/恢复）
```

| 状态 | 含义 | 评审时行为 |
|------|------|-----------|
| `active` | 已通过门禁、生效中 | 完整加载，命中即以正常置信度投递 🔴/🟠 |
| `shadow` | 试用观察期 | 加载但**降级**：命中只在总评一句话汇总或标 `[试验性]`，不单独占 🔴/🟠 主结论 |
| `stale` | 锚点失效待重锚 | **暂停投递**，评审时提示"该规则锚点已失效，待人工重锚" |
| `dormant` | 长期未命中，休眠 | 不加载（降低检查清单噪声），保留可恢复 |
| `retired`/`archived` | 已淘汰 | 移入 `attic.md`，不参与评审 |

---

## 3. 数据基础：评审结果标注 + Eval 语料库

### 3.1 处置标注（ground-truth label）

每条评审评论的**最终处置**是评测一切规则质量的 ground truth，来自 `search_merge_request_notes` 的 `resolve_state` + 作者回复语义：

| 处置 | label | 含义 | 用途 |
|------|-------|------|------|
| 作者接受并修复（`resolved`，且 commit 确实改了对应处） | **TP**（真阳性） | 规则正确命中真问题 | 提升 Precision、计入泛化上下文 |
| 作者明确反驳/`wontfix`/标记无效/回复"这是设计如此" | **FP**（假阳性） | 规则误报 | 拉低 Precision、触发淘汰 |
| 无回应 / 仅 `unresolved` 且无作者交互 | **未定**（unlabeled） | 弱信号 | **不计入** Precision（避免噪声），仅记录 |
| 该类问题被**他人/后续轮次**发现、但本规则未命中 | **FN**（假阴性） | 规则漏报 | 计入召回/覆盖率与提升效果 |

> ⚠️ **关键纪律**：只有 `TP` 才是"被验证的真问题模式"，可作为沉淀候选；`FP` 是**负样本**，必须用于淘汰而非丢弃；`resolved` 必须**核对 commit 真的改了对应位置**，避免作者"点了 resolve 但没改"的假 TP。

### 3.2 Eval 语料库（回测数据集）

- **位置**：`memory/eval-corpus/`（schema 见该目录 `README.md`）。
- **一条记录 = 一次评审评论的结构化标注**：`{mr, round, file_anchor, category, pattern_id?, label(TP/FP/FN/unlabeled), author_reply, commit_after, date}`。
- **来源**：每轮评审收尾（Step 8.1）后，把本轮评论 + 其处置追加进语料库；历史 MR 可用 `search_merge_request_notes` 批量回灌 bootstrap。
- **作用**：候选规则**回测**（§5.1）与**提升效果度量**（§6.4）都在此语料库上跑，保证可复现、可审计。

---

## 4. 规则提取与归一化

从 TP 评论提取候选规则时，**归一化成结构化四元组**，便于回测与冲突检测：

```
trigger（触发条件）  ：在 spec/plan/design/代码里看到什么样子 → 规则被激活
probe（验证动作）    ：AI 要执行的确定性检索（可 grep 的关键词 / 要打开的文件 / 要比对的定义）
assertion（判定）    ：probe 结果满足什么条件才算"命中真问题"
fix（建议方向）      ：命中后给作者的具体修改指引
```

**去硬编码（泛化归一化，强制）**：把来源 MR 里的**具体符号**抽象成**变量占位**，否则规则只对来源 MR 有效。

- ❌ 过拟合写法：`若 spec 说新增 IsExampleDevice() 但已有 isExampleDevice()`
- ✅ 泛化写法：`若 spec 新增 is<X>Device()，先 grep 同族 is*Device() 的既有可见性/命名惯例，判断是否重复或不一致`

**归一化后立即自检可回测性**：probe 必须是"AI 拿到 spec + 基线代码就能确定性执行"的动作；若 probe 依赖"人的经验直觉"而无法落到 grep/文件比对 → 该候选**不适合进 memory**，应转投 `review-guides/` 人工注意点。

---

## 5. 入库前自我评测门禁（Self-Eval Gate）

候选规则**必须通过以下三关**才允许 promote 到 `active`。任一关未过 → 降 `shadow` 或 `rejected`。

### 5.1 回测关（Backtest — 准确率）

在 §3.2 的 Eval 语料库上，用候选规则的 `trigger + probe + assertion` **逐条回放历史评审记录**，判定每条记录规则是否会命中，再与该记录的真实 label 对齐：

```
在语料库中筛出与 trigger 同类的记录集合 S
对 S 中每条记录 r：
    模拟执行 probe + assertion → 判定"规则会不会对 r 产生评论"
    若会命中 且 r.label=TP  → 计 TP
    若会命中 且 r.label=FP  → 计 FP
    若不命中 且 r.label=FN  → 计 FN（漏报）
Precision = TP / (TP + FP)
```

- **门槛**：`Precision ≥ 0.8` → 通过回测关。
- **样本不足**（S 中 labeled 记录 < 3 条，冷启动常态）→ 回测关**判定为"数据不足"**，规则**只能进 `shadow`**，靠后续真实评审累计样本，不得直接 `active`。

### 5.2 泛化关（Generalization）

防止规则只对来源 MR 有效：

- **独立命中上下文数**：候选规则在语料库中 TP 命中的 **distinct `(MR × file_anchor)`** ≥ **2** → 通过。仅来源 MR 单点命中 → 泛化不足，进 `shadow`。
- **过拟合体检**：规则描述若仍含未抽象的具体符号名（未做 §4 去硬编码）→ 直接判泛化不通过，退回重写。
- **跨维度加分**（非门槛，记录用）：跨不同作者 / 不同 spec / 不同端命中 → 泛化越强，可优先 §7.7 升级为 `rules/`。

### 5.3 冲突关（Conflict）

与所有 `active` Pattern + `review-guides/` 人工注意点做**矛盾/重叠检测**：

| 冲突类型 | 判定 | 处理 |
|---------|------|------|
| **语义重复** | 触发条件 + 判定与某 active 规则 ≥80% 重合 | **不新增**；在已有规则追加来源 MR 引用（合并，§7.6） |
| **直接矛盾** | 判定/建议相反（如 A:"复用 helper X" vs B:"X 已废弃勿用"） | 标为冲突对，进 §7.4 保高汰低；候选**暂不入库**，先核实哪条对 |
| **锚点交叠但方向一致** | 命中同类位置但角度互补 | 允许共存，但在两条规则互加 `see-also` 交叉引用 |

**门槛**：无"直接矛盾"、非"语义重复" → 通过冲突关。

### 5.4 锚点有效性关（Anchor Validity）

- 候选规则 probe 里引用的代码锚点（文件路径 / 函数名 / 字段 / 常量），在**当前基线**（Step 0.2 已同步）`grep` 验证真实存在。
- 全部存在 → 通过；部分失效 → 要求把 probe 改成**语义锚点**（"同族 `is*Device` 方法"而非写死 `isExampleDevice`）后再评。

### 5.5 门禁判定汇总

```
IF Precision≥0.8 AND 泛化关通过 AND 无直接矛盾/非重复 AND 锚点有效:
    → promote ACTIVE
ELIF Precision≥0.6 AND 无直接矛盾:          # 含"数据不足"/"泛化不足"
    → SHADOW（试用，降级投递，继续累计样本）
ELSE (Precision<0.6 或 严重冲突 或 无法泛化):
    → REJECTED（写入 memory/attic.md 的"拒收记录"，附拒收理由，供审计；不进正式库）
```

> 模式 A：门禁结论 + 各 metric 展示给用户，用户可 override。
> 模式 B：按门禁自动判定，日志记录每关结果与最终状态。

---

## 6. 评测标准（Metrics 完整定义）

所有 metric 在 §3.2 语料库上计算，**滚动窗口**默认取最近 **20 次评审** 或最近 **90 天**（取先到者），避免历史陈旧数据主导。

### 6.1 规则准确率 Rule Precision（首要）

```
Precision(P) = TP(P) / (TP(P) + FP(P))
```

- **入库门槛**：≥ 0.8
- **维持门槛**：≥ 0.6（跌破 → 降 `shadow`）
- **淘汰红线**：< 0.4（或 FP≥K，见 §7.2）→ `retired`

### 6.2 误报率 False Positive Rate

两个口径并用：

- **相对误报率** `FPR_rel = FP / (TP + FP) = 1 − Precision`（衡量"每次命中里有多少是误报"）。
- **绝对误报计数** `FP_count`（累计，用于硬淘汰）。`FP_count ≥ K(默认3)` 且 `Precision<0.5` → 淘汰。

> 误报比漏报更伤 skill 信誉（作者收到误报会不信任后续评论），故误报门槛从严。

### 6.3 泛化能力 Generalization

- **独立上下文数** `distinct_contexts(P)` = 去重后 `(MR × file_anchor)` 的 TP 命中数。入库要求 ≥2。
- **跨维覆盖度** = TP 命中覆盖的不同 {作者, spec, 端} 维度值数量，越多泛化越强。
- **过拟合指数**（低越好）= `1 − distinct_contexts / TP`（TP 全集中在单一上下文 → 趋近 1，过拟合）。

### 6.4 对后续评审的提升效果 Review Uplift（闭环价值证明）

对比 Pattern 生效**前后**的历史窗口，度量三项：

| 指标 | 定义 | 期望方向 |
|------|------|---------|
| **初评发现率提升** `early_detect_lift` | 该类问题在**初评轮**即被发现的比例（生效后 − 生效前） | ↑（越早发现越好） |
| **返工轮次下降** `round_reduction` | 含该类问题的 MR 平均评审 loop 轮次（生效后 − 生效前） | ↓ |
| **漏报率下降** `miss_rate_reduction` | 该类问题被漏评（FN）的比例（生效前 − 生效后） | ↓ |

- **正向**：三项中至少两项改善 → 规则确有反哺价值，维持 `active` 并可参与 §7.7 升级评估。
- **负向/无效**：`active` 规则长期无正向 uplift 且命中稀少 → 走 §7.5 休眠归档。

> 冷启动期（语料库 < 10 次评审）uplift 无法可靠计算 → **仅记录、不作淘汰依据**，此阶段淘汰只看 Precision/误报/锚点。

---

## 7. 规则净化与淘汰逻辑

**触发时机**：① 每次 **MR merge 事件**（Step 8.2 结束后）跑一次增量净化；② 建议 CI/定时**每周全量净化扫描**一次。所有淘汰**归档而非删除**（移入 `memory/attic.md`，保留状态、统计、淘汰理由，可恢复）。

### 7.1 净化扫描总流程

```
FOR 每条 Pattern P (active/shadow):
    重算滚动窗口 TP/FP/Precision/distinct_contexts/最近命中日期
    ├─ §7.2 误报体检   → 降级/淘汰
    ├─ §7.3 锚点体检   → stale/重锚
    ├─ §7.4 冲突体检   → 保高汰低
    ├─ §7.5 活性体检   → dormant 归档
    └─ §7.6 重复体检   → 合并
更新每条 Pattern 的「状态 + 统计」元数据（§8）
```

### 7.2 误报淘汰（Precision 驱动）

- `active` 且 `Precision ∈ [0.4, 0.6)` → 降 `shadow`（观察是否回升）。
- `active/shadow` 且 `Precision < 0.4`，或 `FP_count ≥ K(3)` 且 `Precision < 0.5` → `retired`（归档，附最近 FP 案例链接）。
- `shadow` 观察满 N(默认5) 次评审仍未达 `active` 门槛 → `rejected`。

### 7.3 过期淘汰（锚点失效）

- 对 probe 引用的代码锚点在**当前基线** `grep` 校验：
  - 全部存在 → 保持。
  - 部分失效 → 标 `stale`，**暂停投递**：
    - 模式 A：提示用户"规则 #N 锚点 `xxx` 已不存在，是否重锚/淘汰？"
    - 模式 B：尝试**自动重锚**（用语义锚点替代写死符号，见 §4/§5.4）；重锚后回测通过 → 复 `active`；无法重锚 → `retired`。
- **语义锚点规则**（不引用具体符号，只描述模式）→ 天然抗过期，免锚点体检。

### 7.4 冲突淘汰（矛盾对）

- 检出"直接矛盾"对 `(P_a, P_b)`：
  - 保留 **Precision 更高**者；并列则保留**更新/锚点有效**者。
  - 落败者降 `shadow`（不立即删，可能是特定上下文例外）；若持续被矛盾覆盖 → `retired`。
  - 在保留者中记录"已合并/取代 P_x"的来源，避免信息丢失。

### 7.5 僵尸归档（活性驱动）

- 连续 `M(默认10)` 次评审 **0 命中**，或最近命中 `> 120 天` → `dormant`（停止加载，降检查清单噪声）。
- `dormant` 再经一个净化周期仍 0 命中 → 归档 `attic.md`。
- 归档规则若日后同类问题重现（人工/新评审命中同模式）→ 可从 `attic.md` **恢复为 shadow** 重新试用。

### 7.6 重复合并

- 语义 ≥80% 重合的多条 → 合并为一条：保留描述更泛化者，累加所有来源 MR、累加统计。
- 合并后编号保留最小者，其余编号记入"已合并至 #N"，避免悬空引用。

### 7.7 升级项目级规则（正向出口）

- 某 `active` 规则同时满足：`distinct_contexts ≥ 3`（≥3 独立 MR）+ `Precision ≥ 0.85` + `跨维覆盖 ≥2` + uplift 正向 → **提议升级为 `rules/` 项目级规则**。
- 升级后 memory 中该 Pattern 标注"已升级 rules/xx"，转为 `dormant`（规则已由更高优先级承载，避免双重加载）。

---

## 8. Pattern 元数据格式（lifecycle + stats）

**新沉淀的 Pattern 采用增强格式**（在 `memory/README.md` 旧格式基础上加"状态 + 统计"两行）：

```markdown
### Pattern N: [简短标题]

**状态**: active            <!-- active|shadow|stale|dormant|retired -->
**统计**: 命中 8 · TP 6 · FP 1 · 未定 1 · Precision 0.86 · 独立上下文 3 · 最近命中 2026-07-26   <!-- 由净化扫描自动维护 -->
**首次沉淀**: 2026-07-26，来源 MR [!NN](url)
**来源 MR**: !77(TP), !83(TP), !91(TP)
**trigger（触发条件）**：<泛化描述，去硬编码>
**probe（验证动作）**：<可 grep/可打开文件的确定性检索>
**assertion（判定）**：<满足什么才算命中真问题>
**为什么严重**：<后果>
**fix（建议方向）**：<具体修改指引>
```

- **存量 Pattern 迁移**：现有 #1~#13、D1~D8 均来自真实 resolved 问题，默认 `状态: active`；统计字段在下次净化扫描时**惰性补齐**（首次扫描把来源 MR 记为 1 个 TP 上下文）。无需一次性重写全部旧条目。
- **`review-guides/` 人工注意点不加此元数据**（专家维护、不走自动 eval），但**参与冲突检测**（§5.3）。

---

## 9. 闭环调度：与 SKILL.md 步骤的对应

```
                          ┌──────────────── 评审 loop（push 触发）──────────────┐
  MR push ─→ 评审(Step0~7) ─→ 投递评论(带指纹) ─→ 收尾(Step8.1)
                                                     │
                                     追加本轮评论+处置 → Eval 语料库(§3.2)
                          └────────────────────────────────────────────────────┘

  MR merge ─→ Step 8.2 经验沉淀（eval 门禁版）
                │
                ├─ ① 提取: resolved 且 commit 已改 → TP 候选（§3.1/§4 归一化四元组）
                ├─ ② 入库门禁: 回测+泛化+冲突+锚点（§5）→ active / shadow / rejected
                ├─ ③ 负样本: rejected 评论 → 作为 FP 更新相关规则统计（§3.1/§6）
                └─ ④ 增量净化: 对涉及规则跑 §7 体检
                       ↓
  (每周/CI 定时) ─→ 全量净化扫描（§7.1）→ 更新所有 Pattern 状态与统计
```

**关键改动 vs 旧 Step 8.2**：
1. 沉淀候选**不再直接入库**，先过 §5 门禁 → 只有达标才 `active`，不达标进 `shadow` 试用。
2. **负样本（被拒绝评论）不再丢弃**，而是回灌为 FP，驱动误报淘汰（旧流程完全忽略）。
3. 新增**定期净化**，处理过期/冲突/僵尸（旧流程只增不减）。
4. 每条规则带**状态 + 统计**，投递时 `shadow`/`stale` 规则降级或暂停（旧流程所有 Pattern 等权）。

### 9.5 记忆回写与远端持久化（headless / 共享工作区，**必读**）

> ⚠️ **核心约束**：沉淀/净化产生的所有变更都落在 CoSpec 仓的 `memory/`（`code-patterns.md`、`doc-process-patterns.md`、`eval-corpus/`、`attic.md`）。skill 跑在 agent（CI / headless）上时，**绝不能只改共享工作副本**——那样会：① 被 Step 0 的 `fetch + reset` 冲掉；② 不 push 就无法反哺其他会话/人（"越用越准"断链）；③ 违反"共享 CoSpec 仓只停基线、只读、不切分支"的并发安全原则（见 SKILL.md 🔒 并发安全原则）。

> 🧩 **Knot 部署下的 logic/data 分离（前提，见 SKILL.md Step 0.3）**：skill 逻辑（SKILL.md/scripts）从 Knot **冻结包**执行；但 memory/eval-corpus/review-guides 是**数据**，**唯一真相在 CoSpec 仓**。因此本节的**读**与**写**都针对 CoSpec 仓的记忆分支，**绝不读/写 Knot 打包副本**（那是冻结快照，会让闭环反哺失效）。

**读取（运行时拿最新经验）**：一律经Git 平台（GitHub / GitLab / 工蜂） MCP `get_blob_content(project_id=<CoSpec>, sha=<chore/mr-review-memory>, file_path=skills/mr-spec-review/memory/...)` 读记忆分支 head，拿到**跨会话共享的最新**规则库（不受 Knot 冻结、也不受本地 master 基线滞后影响）。MCP 不可用 → 退回 `$COSPEC_ROOT` 本地副本并注明可能非最新；连 CoSpec 都定位不到 → 用 Knot 冻结种子且**本轮不写回**。

**因此记忆回写走「Git 平台（GitHub / GitLab / 工蜂） MCP 写 API 提交到专用分支」，零本地 git 副作用：**

```
读远端最新 (get_blob_content, ref=记忆分支)
   → 本地内存里算好新内容（追加/合并/改状态）
   → create_or_update_file / batch_modify_files 提交到专用记忆分支
   （不 checkout、不 reset、不动共享工作副本 → 并发安全）
```

**① 专用记忆分支**：固定一条长期分支承载所有记忆变更，如 `chore/mr-review-memory`（与被评审 MR 的 feature 分支隔离，互不干扰）。

**② 写入时机分离（降并发冲突）**：

| 时机 | 写什么 | 冲突风险 |
|------|--------|---------|
| 每轮评审（push）收尾 | 仅 append `eval-corpus/records.jsonl` | 低（追加型；并发用"读最新→追加→提交，失败重试"） |
| MR **merge** 事件 | Pattern 门禁沉淀 + 净化写 `memory/*.md` + `attic.md` | 低（merge 频率远低于 push，天然错峰） |

> 把"重写 `memory/*.md`"限定在 merge 事件（而非每次 push），可把并发写压到最低——多个 MR 的评审 loop 高频 push 时只碰 append-only 的语料库。

**③ 持久化到 remote 的两种运营模式（团队选一）**：

| 模式 | 做法 | 你需要"定期"做的事 |
|------|------|-------------------|
| **A. 自动直推记忆分支** | eval 门禁已卡质量，agent 直接 commit 到 `chore/mr-review-memory` | 定期把该分支合入 master（可配自动 FF） |
| **B. 自动累积一条 memory-curation MR**（**推荐**） | 变更自动开/更新到一条长期存在的记忆 MR（`create_merge_request` + 后续 `create_or_update_file` 追加 commit） | **只需定期 review + merge 这条 MR**——给规则库演进留一道人工闸门 |

> ✅ **回答"是否要手动去 agent 提交"**：不需要。回写是闭环自动的一环（agent 通过 MCP 提交）；人工侧只保留"**定期 review + merge 那条记忆 MR**"这一道把关（模式 B），或"定期合并记忆分支"（模式 A）。**绝不需要登录 agent 手动敲 git commit。**

**④ 并发写安全**：
- 同一文件的提交冲突（`create_or_update_file` 基于旧 sha）→ **重读最新内容 → 重算 → 重试**（幂等，最多几次）。
- eval-corpus 用**追加**语义、按指纹去重，天然幂等。
- 密钥：提交用的 `GONGFENG_TOKEN` 需对 CoSpec 仓有 write 权限，env-only、掩码注入（遵守 @security_rules）。

**⑤ 模式 A（本地手动）**：memory 就在你本地工作副本里，正常 `git add/commit/push`（或走你平时的 spec MR 流程）即可，无需 MCP 写 API；但**仍不要在共享/并发工作区**直接改，避免踩其他会话。

---

## 10. AI 执行清单（照单执行）

### 10.1 收尾时（每轮评审 Step 8.1 后）— 喂语料库
1. 取本轮所投评论 + 指纹 + 当前 `resolve_state`，**追加**到 `memory/eval-corpus/records.jsonl`（模式 B 走 §9.5 MCP 写 API 提交到记忆分支；追加型、按指纹去重、幂等）。
2. 复评轮：把上一轮 `unresolved→resolved` 的记录 label 更新为 `TP`（**须核对对应 commit 真的改了**）；`unresolved→作者反驳` 更新为 `FP`。

### 10.2 入库时（MR merge / Step 8.2）— 门禁沉淀
1. **筛候选**：本轮新 `TP`（resolved + commit 已改）→ 候选；被拒 `FP` → 记负样本。
2. **归一化**：候选写成 §4 四元组，去硬编码。
3. **过门禁**：跑 §5.1 回测 / §5.2 泛化 / §5.3 冲突 / §5.4 锚点 → 判 `active`/`shadow`/`rejected`。
4. **落盘**：按 §8 格式把新内容算好，**通过 §9.5 的 MCP 写 API 提交到专用记忆分支**（不改共享工作副本）——写入对应 `code-patterns.md` / `doc-process-patterns.md`（含状态+统计）；`rejected` 写 `attic.md` 拒收记录。
5. **回灌 FP**：把本轮被拒评论对应的规则统计更新（FP+1，重算 Precision），触发 §7.2 判定。

### 10.3 净化时（merge 增量 / 每周全量）— 自净化
1. 对每条 `active`/`shadow` 规则重算滚动窗口统计（§6）。
2. 依次跑 §7.2 误报 / §7.3 锚点 / §7.4 冲突 / §7.5 活性 / §7.6 重复体检。
3. 更新状态与统计元数据；淘汰项移 `attic.md`（附理由）。
4. 满足 §7.7 的规则 → 提议升级 `rules/`。
5. **产出净化报告**（附收尾汇报或 CI 日志）：

```
🧪 规则净化报告（窗口：最近 20 次评审）
  入库：新增 active {a}、shadow {s}、拒收 {r}
  升级：{Pattern#N「标题」→ shadow（降级，Precision {p}）}
  淘汰：{Pattern#M「标题」→ retired（FP {fp}/Precision {p}）}
  过期：{Pattern#K → stale（锚点 `xxx` 已失效，{已自动重锚/待人工}）}
  休眠：{Pattern#J → dormant（连续 {n} 次 0 命中）}
  合并：{#X 并入 #Y}
  升级规则：{#Z → 提议写入 rules/（distinct {c}、Precision {p}）}
  当前库存：active {A} · shadow {S} · dormant/attic {D}
```

### 10.4 评审加载时（Step 4.1/4.5/4.6）— 按状态用规则
- 只把 `active` 规则以完整置信度纳入检查清单、命中即正常投递。
- `shadow` 规则加载但**降级**：命中只在总评汇总或标 `[试验性建议]`，不占 🔴/🟠 主结论、不带"必须修改"措辞。
- `stale`/`dormant`/`retired` **不加载**（`stale` 可在报告提一句"规则待重锚"）。

---

## 11. 参数一览（可按团队调整）

| 参数 | 默认 | 含义 | 出处 |
|------|------|------|------|
| 入库 Precision 门槛 | 0.80 | promote active 的准确率下限 | §5.1/§6.1 |
| 维持 Precision 门槛 | 0.60 | 低于则降 shadow | §6.1/§7.2 |
| 淘汰 Precision 红线 | 0.40 | 低于则 retired | §6.1/§7.2 |
| 绝对误报淘汰 K | 3 | FP≥K 且 Precision<0.5 → retired | §6.2/§7.2 |
| 入库最少独立上下文 | 2 | 泛化门槛 | §5.2/§6.3 |
| 升级 rules 独立上下文 | 3 | + Precision≥0.85 | §7.7 |
| shadow 观察窗 N | 5 | 试用满 N 次仍不达标 → 拒收 | §7.2 |
| 僵尸阈值 M | 10 次 / 120 天 | 0 命中即 dormant | §7.5 |
| 滚动窗口 | 20 次 / 90 天 | metric 计算窗口 | §6 |
| 冷启动阈值 | 语料库<10 次评审 | 期间不以 uplift 淘汰 | §6.4 |

---

## 12. 与其他资产的关系

- **`memory/README.md`**：记忆库总说明；本文件是其"质量门禁"细则，Pattern 格式以本文件 §8 为准。
- **`memory/attic.md`**：淘汰/拒收规则的归档区（可审计、可恢复）。
- **`memory/eval-corpus/`**：回测与 uplift 度量的数据集。
- **`review-guides/`**：人工注意点不走自动 eval，但参与 §5.3 冲突检测；某 `active` Pattern 若明显是某端稳定专业项，可迁入对应 guide（转 `dormant`）。
- **`SKILL.md` Step 8.2**：执行入口，按本文件 §10 清单跑闭环。
- **`rules/`**：§7.7 升级出口。

---

## 维护记录

| 日期 | 变更 | 操作人 |
|------|-----|--------|
| 2026-07-27 | 初版：定义规则生命周期状态机、入库前自我评测门禁（回测/泛化/冲突/锚点）、评测标准（Precision/误报率/泛化/uplift）、净化淘汰逻辑（误报/过期/冲突/僵尸/合并/升级）、AI 执行清单 | evanye |
