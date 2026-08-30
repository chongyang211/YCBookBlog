# 分端评审依据库（Per-End Review Guides）

> 本目录是 `mr-spec-review` skill 的**分端评审依据**。评审阶段不再"一锅烩"，而是**先识别 MR 涉及哪些端，再按端加载对应依据文档**，用每个端专业、准确的视角去评。
>
> 与 [`../memory/`](../memory/) 的区别：
> - `memory/`（code-patterns / doc-process-patterns）= **自动沉淀**的历史真问题模式，随评审 loop 增长，机器维护为主。
> - `review-guides/`（本目录）= **各端稳定的评审基准与 checklist** + **人工可维护的专业注意点**，是"评审这个端应该看什么"的权威依据，人工维护为主。
> - 两者在评审时**都要加载**：memory 是"历史踩过的坑"，guide 是"这个端的专业检查项"。

---

## 1. 端划分与路由表（权威来源）

skill 在 **Step 4.0 端识别** 时，依据 spec/design 里引用/影响的 `src/` 仓库（来自「关键挂载点 / 改动文件清单 / 跨系统影响表 / Branch」）映射到端，加载对应 guide。一个 MR 可命中多个端。

| 端 | 评审依据 | 命中的 `src/` 目录（路径前缀） | 语言 / 栈 |
|----|---------|------------------------------|-----------|
| **后台服务端** | [`backend.md`](./backend.md) | `<设备管理仓库>`、`<业务主仓库>`、`<业务主仓库>`、`<激活服务仓库>`、`<IoT 服务仓库>`（云侧部分）、`infrastructure` | Go 1.23+ / gRPC + grpc-gateway / DDD |
| **识别 · 算法端** | [`algorithm.md`](./algorithm.md) | `algorithm-repo`、识别侧 `<业务主仓库>`/`proto`（逻辑已合并入 <ORG>/<子组>） | Go / 算法编排 gRPC |
| **前端** | [`frontend.md`](./frontend.md) | `<前端仓库>`、`<支付仓库>/web`、`<SDK 仓库>/web` | Vue3 / Vite / TS |
| **设备端（固件 · 边缘）** | [`device.md`](./device.md) | `<IoT 服务仓库>`、`paymax_device`（`<终端应用仓库>`/`<设备接入仓库>`/`<管理后台仓库>`）、`<IoT 服务仓库>`（设备侧 Android 框架） | C++ / CMake、Android(Java/Kotlin) |
| **移动端 · 接入 SDK** | [`mobile-sdk.md`](./mobile-sdk.md) | `<SDK 仓库>`（`android`/`ios`）、`<支付仓库>/Android` | Kotlin / Swift / JS |
| **跨端协议契约**（横切） | [`contract.md`](./contract.md) | `proto`、`<协议仓库>`；以及任意端改到 `*.proto` / 接口 / 错误码 / 字段号 | protobuf / IDL |

> ⚠️ **契约端是横切的**：只要 MR 涉及跨端联调的字段/接口/错误码/proto 变更，**无论主端是谁，都要额外加载 `contract.md`**（对应 code-patterns#4 兄弟 spec 字段号一致）。
>
> ⚠️ **识别侧仓库归并**：`<ORG>/<算法组>` 原有的 `<业务主仓库>`/`proto` 逻辑已合并进 `<ORG>/<子组>` 的 `<业务主仓库>`/`proto`（`src/<业务主仓库>`、`src/proto`），原 `src/<旧流水线目录>/` 分组已废弃删除；识别算法仓 `algorithm-repo` 平铺在 `src/algorithm-repo`。路由识别侧 <业务主仓库>/proto 时直接用 `<业务主仓库>`/`proto` 目录。

### 端识别兜底

- spec/design 未明确写涉及哪些仓库 → 从「Branch 字段」「跨系统影响表」「关键挂载点」推断；仍无法判定 → **在报告里说明"端归属不明确，按最相关端评审"**，模式 A 可问用户。
- 命中多个端 → **全部加载**，评审报告按端分组（见 SKILL.md Step 5）。
- intake/design-only MR（技术方案）→ 一样先识别端，用对应 guide 的"架构约定 / 高频坑"评方案是否贴合该端现状。

---

## 2. skill 如何使用本目录（强制流程）

1. **Step 4.0**：识别 MR 涉及的端 → 列出命中的 guide 文件清单。
2. **进入 Step 4.1 / 4.5 / 4.6 评审前**：对每个命中的端，**读完对应 guide 全文**（含底部「人工追加评审注意点」专区），把 Checklist + 人工注意点全部加载进当前评审的检查清单。
3. 逐条对照 spec/design/代码扫描，命中即写评论并标注来源：
   - Checklist 命中 → 标 `命中 <end>-guide#<小节>`（如 `命中 backend-guide#分层`）。
   - 人工注意点命中 → 标 `命中 <end>-guide#人工-<编号>`（如 `命中 device-guide#人工-3`）。
4. 报告与投递时，**问题按端分组**，让作者一眼看清"哪个端有几个问题"。

---

## 3. 人工追加评审注意点（团队专家维护）

每份 guide 底部都有一个用稳定标记包裹的专区：

```markdown
## 🖊 人工追加评审注意点（Human-Curated Review Points）

<!-- HUMAN-CURATED:START -->
<!-- 在本行下方按格式追加条目，勿删除首尾两个标记 -->

<!-- HUMAN-CURATED:END -->
```

### 3.1 为什么要有这个专区

- 让**资深专家**把"只有做过这个端才知道"的专业注意点沉淀进来，skill 评审时**强制加载**，等同于给 AI 装上该端专家的经验。
- 与 `memory/` 的自动沉淀互补：memory 是"评审 loop 里被验证的坑"，人工专区是"专家主动前置的检查项"，**不需要等踩坑就能生效**。

### 3.2 追加条目格式（每条注意点遵循）

```markdown
### 人工-N: [简短标题]（🔴/🟠/🟡）

**关注点**：<评审这个端时，具体要看什么、什么写法/设计要警惕>

**判定依据**：<对照哪个代码位置 / 哪条规范 / 什么现状来判定，最好给可 grep 的关键词或文件路径>

**建议方向**：<命中后给作者的具体修改指引>

**追加人 / 日期**：<@who / YYYY-MM-DD>
```

### 3.3 维护规则

- **编号连续**：`人工-1`、`人工-2` …，只在两个 `HUMAN-CURATED` 标记之间追加，**不要删除标记本身**（skill 靠它定位专区）。
- **风险分级**：标题后用 🔴/🟠/🟡 标默认风险等级，与 SKILL.md「输出风险分级标准」一致。
- **优先级等同 Checklist**：人工注意点与该 guide 上方的 Checklist 同等重要，skill 必须逐条检查。
- **可追溯**：每条带追加人 + 日期，便于回溯上下文、后续讨论。
- **别塞历史坑**：一次性笔误、具体某 MR 的偶发问题应走 `memory/` 沉淀；本专区放的是**该端稳定、通用的专业注意点**。
- **命中率高 → 升级规则**：某条人工注意点若长期高频命中，可提炼进 `rules/` 成为项目级规则。

### 3.4 谁来加

- 各端 owner / 资深专家。评审中发现"这个端还该看 XX 但 guide 没覆盖"时，欢迎直接追加。
- 模式 A（本地手动）下，skill 若发现某端反复出现同类问题、且不在 guide 内，可**提议**用户把它固化进对应 guide 的人工专区。

---

## 4. Guide 与其他资产的关系

```
skills/mr-spec-review/
├── SKILL.md                      # 评审主流程（Step 4.0 端识别 → 按端加载 guide）
├── review-guides/                # 本目录：分端评审依据（人工维护为主）
│   ├── README.md                 # 端路由表 + 人工追加规范（本文件）
│   ├── backend.md / algorithm.md / frontend.md / device.md / mobile-sdk.md / contract.md
├── memory/                       # 历史真问题模式（自动沉淀为主）
│   ├── code-patterns.md / doc-process-patterns.md / README.md
└── scripts/prepare-src.sh        # 同步 src 各端基线代码
```

- **评审时的加载顺序**：memory（历史坑）+ 命中端的 review-guide（专业依据 + 人工注意点）→ 合并成当前 MR 的检查清单。
- **guide 里引用代码位置**要与 [`../scripts/prepare-src.sh`](../scripts/prepare-src.sh) 同步的**基线现状**对齐；发现 guide 描述与代码不符时，以代码为准并顺手订正 guide。
