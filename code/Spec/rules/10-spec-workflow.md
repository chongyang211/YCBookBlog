# Spec 工作流规则

本文件是本仓库**最核心的工作流规则**。AI 在处理任何基于 spec 的任务时，必须严格按照以下流程执行。

---

## 阶段零前：技术方案（Design）—— 可选

**触发条件**：收到原始需求后，发现需求**较大、较复杂、跨多个模块/仓库，或存在多种技术方案需要评审拍板**。仓库中尚无对应 spec。

> 💡 这是一个**可选的前置环节**。小需求 / 改动清晰的需求**跳过本阶段**，直接进入阶段零（Draft）。

**为什么需要这一步**：

> 需求大时，直接起草 spec 容易因为技术方向未定而反复返工。先做技术方案、经团队评审，把「用什么方案做、要拆成几个 spec」定下来，再基于「需求 + 方案」起草 spec，spec 会更准确、边界更清晰。

**核心原则**：

- Design 是**方案级**产物，回答「技术上怎么做、为什么这样选」，**不写**到具体文件的实施步骤（那是阶段二 Plan 的事）
- Design **不替业务方拍板**目标 / 验收标准（那是阶段零 Draft + 人的事）
- 关键技术决策**必须给方案对比**（≥2 个备选 + 取舍），供评审拍板
- **核心产出是「建议的 spec 拆分」**——大需求拆成几个子 spec、各自边界
- 输出 status **一定是 `draft`**，必须经团队评审才能改 `approved`

**Design vs Plan（关键区分，避免混淆）**：

| 维度 | 技术方案 Design（阶段零前·可选） | 实施计划 Plan（阶段二） |
|------|------|------|
| 时机 | spec 之前（需求大 / 方案不确定时） | spec 已 ready 之后 |
| 输入 | 原始需求 `docs/intake/` | 已 ready 的 spec |
| 关注点 | 架构、技术选型、方案对比、模块划分、**spec 拆分建议** | 落地到具体文件的改动清单、Phase 步骤 |
| 颗粒度 | 方案级（怎么做、为什么） | 执行级（改哪些文件、按什么顺序） |
| 产物 | `designs/<VERSION>/<STORYID>-<slug>-design.md` | `plans/<VERSION>/<STORYID>-<slug>-plan.md` |
| 是否必须 | 否，仅大/复杂需求 | 复杂改动必须 |

> 一句话：**Design 决定「用什么方案、拆几个 spec」，Plan 决定「改哪些文件、按什么步骤」。** Plan 阶段应直接复用 Design 已沉淀的关键技术决策，不重复讨论。

**必须执行的步骤**：

1. 调用 `/spec-design`，基于 `docs/intake/<VERSION>/<STORYID>-<slug>.md` 产出技术方案
2. **先澄清（不可跳过，可多轮）**：与用户确认①需求性质（全新 vs 增量）②涉及仓库及是否已 clone 到 `src/`（未 clone 先提示 clone）③现有实现定位④核心逻辑与边界⑤非功能约束⑥范围边界
3. 对已 clone 的涉及仓库调用 `skills/codebase-survey`（架构视角：模块边界、调用链、跨系统交互，不深入函数实现）
4. **先给 2-3 个候选方案让用户选**：在对话中输出候选方案对比表 + 推荐，**等用户选定后**再落盘，不允许只给单一方案或未经确认就写文件
5. 以选定方案为主线按 `designs/templates/design-template.md` 填写：总体方案、方案对比（含被淘汰方案）、关键技术决策、跨系统影响、**建议的 spec 拆分**、风险
6. 写入 `designs/<VERSION>/<STORYID>-<slug>-design.md`（`Status: draft`）
7. 团队（架构师 / Tech Lead / 相关方）评审关键决策与 spec 拆分建议
8. 评审意见回填 design 的「评审记录」章节；评审通过后 `Status` 改为 `approved`
9. 进入阶段零：`/spec-draft` 基于「intake + 已 approved 的 design」起草 spec（如建议拆多个，逐个起草子 spec）

**Design 文件必须包含的章节**：

1. **需求背景与目标**：忠实复述 intake
2. **现状分析**：现有架构、模块边界、约束
3. **总体方案**：核心思路 + 架构/数据流图
4. **方案对比**：关键决策的备选方案 + 取舍
5. **关键技术决策**：选了什么 / 为什么 / 淘汰了什么
6. **跨系统 / 跨仓库影响**
7. **数据结构 / 接口影响（高层）**
8. **建议的 spec 拆分**：拆几个 spec、各自边界与 slug 建议
9. **风险与未决问题**
10. **评审记录**

**禁止行为**：

- 不与用户澄清（需求性质 / 仓库 clone 状态 / 边界）就直接出方案
- 只给单一方案，或未经用户选定就把方案落盘成文件
- 涉及仓库未 clone 却凭空给出该仓库的具体结论
- 把技术方案写成具体文件的实施步骤（越界到 Plan）
- 替业务方臆造目标 / 验收标准
- 把 `Status` 直接设为 `approved`，跳过评审
- 大需求的技术方案缺失「建议的 spec 拆分」

---

## 阶段零：需求转 Spec（Draft）

**触发条件**：收到原始需求（口头描述、邮件、IM、`docs/intake/xxx.md` 等），仓库中**尚无**对应 spec。

**核心原则**：

- AI 是**起草助手**，不是业务方
- 原始需求里没有的信息，AI **不能臆造**，必须标 `[TBD]` 或主动问
- 输出的 spec status **一定是 `draft`**，必须经人 review 才能进入阶段一

**必须执行的步骤**：

1. 把原始需求落到 `docs/intake/<VERSION>/<STORYID>-<slug>.md`（保留出处与原话；如 story id 尚未分配用 `0` 占位，分配后再重命名）
2. 调用 `/spec-draft` 起草 `specs/<VERSION>/<STORYID>-<slug>.md`
   - **如已走过「阶段零前·技术方案」**：起草必须同时读取 `designs/<VERSION>/<STORYID>-<slug>-design.md`（应为 `approved` 状态），把方案中的关键技术决策、spec 拆分建议作为起草输入；如建议拆多个子 spec，逐个起草
   - 起草前**必须先做轻量代码侦察**（调用 `skills/codebase-survey` light 模式）
   - 侦察结果合并到 spec 的「实施备注」+「关键代码参考」+「风险与未决问题」
3. AI **主动列出 2-5 个澄清问题**，等用户回答（包括确认 Story ID）
4. 用户回答后 AI 写入文件（`Status: draft`，frontmatter 含 `Story ID` 字段）
5. PO / 架构师 / Tech Lead review 草稿，回答 `[TBD]` 项
6. `Status` 改为 `ready` 后才进入阶段一

**文件命名规范（三件套对齐）**：

| 文件类型 | 命名 | 示例 |
|---------|------|------|
| Intake | `docs/intake/<VERSION>/<STORYID>-<slug>.md` | `docs/intake/v1.6.0/134389917-dashboard-online-palm-statistics.md` |
| Design（可选） | `designs/<VERSION>/<STORYID>-<slug>-design.md` | `designs/v1.6.0/134389917-dashboard-online-palm-statistics-design.md` |
| Spec | `specs/<VERSION>/<STORYID>-<slug>.md` | `specs/v1.6.0/134029305-payment-retry.md` |
| Plan | `plans/<VERSION>/<STORYID>-<slug>-plan.md` | `plans/v1.6.0/134029305-payment-retry-plan.md` |
| Tasks | `tasks/<VERSION>/<STORYID>-<slug>-tasks.md` | `tasks/v1.6.0/134029305-payment-retry-tasks.md` |

> 📂 **版本目录层级（强制）**：所有产物按迭代版本归档到 `<VERSION>/` 子目录（如 `v1.6.0/`）。`templates/`、`README.md` 是跨版本元文件，保留在各目录根。`<VERSION>` 由当前迭代决定，同一迭代的 intake / design / spec / plan / tasks 放进同名版本目录。聚合索引由 `/spec-index` 发布到 iWiki，不在仓库内维护。

- `STORYID`：纯数字需求单号；无对应 story 用 `0` 占位
- `<slug>`：kebab-case 简短描述
- spec / plan / tasks 三件套的 STORYID + slug **必须完全一致**

**多 spec 共享 Story ID（关键语义）**：

> Story ID **不一定唯一**——一个大需求可拆给多人，产生多个 spec，它们共享同一 Story ID。  
> 这是合法且常见的场景。

| 场景 | 命名示例 |
|------|---------|
| 单 spec | `specs/v1.6.0/134029305-payment-retry.md` |
| 大需求拆分（同 Story 多 spec） | `specs/v1.6.0/134029304-gateway-changes.md`<br>`specs/v1.6.0/134029304-controller-impl.md`<br>`specs/v1.6.0/134029304-passvendor-channel.md` |

- **真正的唯一标识**：`<STORYID>-<slug>` 整体（即文件路径）
- **slug 的作用**：在同一 Story 内区分不同的子 spec，必须有业务区分度（按模块 / 子功能命名）
- **spec 间引用**：子 spec 在 frontmatter `Sibling Specs` 字段列出兄弟 spec 路径
- **唯一性硬约束**：spec 文件名（`<STORYID>-<slug>.md` 整体）**全局唯一**——同 Story 下 slug 必须不同；跨 Story 不允许撞名

**分支命名规范（实施阶段拉分支）**：

> 进入阶段三（implement）时，必须为每个 spec 在所有相关仓库拉分支。

格式：

```
{prefix}/{spec-name}
```

- `{prefix}`：
  - `feature/` — 新功能 / 新 spec / 大需求拆分（默认）
  - `hotfix/` — 紧急 bug 修复（须显式声明）
- `{spec-name}`：spec 文件名去 `.md` 后缀（与 plan/tasks 文件名 stem 一致）

示例：

| spec | 分支名 |
|------|-------|
| `specs/v1.6.0/134029304-online-palm-registration.md` | `feature/134029304-online-palm-registration` |
| `specs/v1.6.0/134029304-gateway-changes.md`（子 spec） | `feature/134029304-gateway-changes` |
| `specs/v1.6.0/0-fix-payment-timeout.md`（紧急修复） | `hotfix/0-fix-payment-timeout` |

**跨仓库一致性**：

- 一个 spec 可能涉及多个仓库（如 `palm_proto` + `palm_local`）
- 所有相关仓库**必须使用同一分支名**，便于追溯关联
- 仓库清单从 plan 的「涉及仓库」表读取
- 多仓库 PR 在描述中相互引用（`相关 PR: palm_proto#123`）

> ⚠️ 设计取舍：早期版本曾使用 `NNNN-<STORYID>-<slug>` 格式（含顺序号 NNNN），但 NNNN 在多人并行开发时易冲突且难维护，**已废弃**。Story ID + slug 组合已足够定位，无需额外顺序号。

**Story ID 录入与沿用机制**：

> Story ID 是**全流程的需求追溯线索**——必须在阶段零（起草时）一次性确定，后续所有阶段沿用。  
> ⚠️ Story ID **不强制唯一**：一个大需求可拆为多个 spec，共享同一 Story ID（靠 slug 区分）。

| 阶段 | 命令 | Story ID 来源 |
|------|------|--------------|
| 阶段零前（可选） | `/spec-design` | **录入/沿用**：用户对话 → intake frontmatter → AI 主动询问；暂无用 `0` 占位 |
| 阶段零 | `/spec-draft` | **入口录入/沿用**：用户对话 → intake/design frontmatter → AI 主动询问。如已有同 Story 的 spec，AI 必须主动确认是否新建子 spec |
| 阶段二 | `/spec-plan` | **沿用**：从 spec 文件名解析 + 校验 frontmatter 一致 |
| 阶段二点五 | `/spec-tasks` | **沿用**：从 spec/plan 文件名解析 |
| 阶段三 | `/spec-implement` | **沿用**：从 spec 文件名解析 |
| 阶段四 | `/spec-test` | **沿用**：从 spec 文件名解析 |
| 阶段五 | `/spec-review` `/spec-sync` | **沿用**：从 spec 文件名解析 |

**版本目录录入与沿用机制（强制）**：

> `<VERSION>`（迭代版本号，如 `v1.6.0`）是**全流程的归档维度**——在最起点（intake 创建时）确定，后续每个命令生成的文件都放进**同一版本目录**，全程不漂移。

- **录入点**：用户在 `docs/intake/<VERSION>/<file>.md` 创建需求文档时，`<VERSION>` 即确定
- **沿用**：后续每个命令都**从输入文件路径解析出 `<VERSION>`**（`<dir>/<VERSION>/<file>` 的中间段），产物写入对应目录的**同名 `<VERSION>` 子目录**：
  - `/spec-design` 读 `docs/intake/<VERSION>/…` → 写 `designs/<VERSION>/…`
  - `/spec-draft` 读 `docs/intake/<VERSION>/…`(+`designs/<VERSION>/…`) → 写 `specs/<VERSION>/…`
  - `/spec-plan` 读 `specs/<VERSION>/…` → 写 `plans/<VERSION>/…`
  - `/spec-tasks` 读 `specs/<VERSION>/…`+`plans/<VERSION>/…` → 写 `tasks/<VERSION>/…`
  - `/spec-implement` `/spec-test` `/spec-review` `/spec-push` `/spec-sync` → 从 spec 路径解析 `<VERSION>`，定位 / 写入同版本目录下的 plan / tasks / tests
- **目标版本目录不存在 → 先自动创建（`mkdir -p`）再写入**，不允许写到目录根
- 同一需求的 intake / design / spec / plan / tasks **必须在同一 `<VERSION>` 目录**；跨版本即视为不一致
- 不重新询问版本号——**除非**输入路径缺少版本目录段（旧文件直接在 `specs/` 根）→ 此时必须主动问「本次归属哪个版本目录（如 `v1.6.0`）？」；用户显式指定版本则以其为准（用于跨版本新建）

**校验铁律**：
- `/spec-draft` 没有 Story ID 输入 → 必须主动问，不允许跳过；暂无 story 用 `0` 占位
- `/spec-draft` 检测到同 Story 已有 spec → 必须主动询问"新建子 spec / 修改已有 / 换 Story ID"
- 其他命令收到 spec 路径时，**必须**先解析 Story ID + slug，并校验 frontmatter `Story ID` 一致
- spec 文件名 STORYID 与 frontmatter `Story ID` 字段不一致 → 报错，要求人工修正
- spec/plan/tasks 三件套的 STORYID + slug 整体必须完全一致（plan/tasks 通过 spec 路径确定）
- 旧版无 STORYID 段的 spec（如 `0-example-feature.md`）→ 视为 Story ID = `0`，新生成的 plan/tasks 也用 `0`

**人 vs AI 分工**：

| 章节 | AI | 人 |
|------|----|----|
| 编号 / slug / 元信息 | ✅ 起草 | 审 |
| 背景（复述原始需求）| ✅ 起草 | 审 |
| **目标** | ❌ 不能臆造 | ✅ 必须由人定 |
| 用户故事 | ✅ 起草 | 审 |
| 功能需求（FR）拆分 | ✅ 起草建议 | ✅ 拍板边界 |
| **验收标准** | 🤝 起草建议 | ✅ 最终拍板 |
| **非功能需求** | ❌ 不能默认 | ✅ 必须由人定 |
| **安全 / 合规** | ❌ 不能臆造 | ✅ 必须由人定 |
| 边界情况 | ✅ 推断 | 审 + 补 |
| 实施备注 / 风险 | ✅ 起草 | 审 |
| 修订记录 | ✅ 占位 | — |

**禁止行为**：

- AI 凭空臆造业务目标 / 验收标准 / ROI
- AI 假设非功能需求（如默认要求"性能 < 100ms"）
- 把 `Status` 直接设为 `ready`
- 不问澄清问题就硬填章节
- 直接修改已有 spec 当作"起草"（已有 spec 修改走 `/spec-sync`）

---

## 阶段一：任务识别与 Spec 定位

**触发条件**：收到任何功能开发、修改、优化类任务。

**必须执行的步骤**：

1. **扫描 `specs/` 目录**，查找与任务相关的 spec 文件
2. 如果找到相关 spec：
   - 完整阅读该 spec
   - 在回复中简要总结对 spec 的理解（2-5 句话）
   - 指出 spec 中任何模糊、缺失或矛盾的地方
3. 如果没有找到相关 spec：
   - 明确告知"未找到相关 spec"
   - 建议先创建 spec，或询问是否按口头描述直接执行
4. 如果 spec 状态为 `draft` 且存在 `[TBD]` 项：
   - 列出所有 TBD 项
   - 说明哪些 TBD 会影响实施，哪些可以暂时忽略

**禁止行为**：
- 不读 spec 就直接开始写代码
- 对 spec 中的模糊部分自行假设而不说明

---

## 阶段二：实施计划（Plan）

**触发条件**：Spec 已读取并理解，准备开始实施。

**前置侦察**：

> ⚠️ Plan 起草**必须**先做深度代码侦察。  
> 调用 `skills/codebase-survey` deep 模式，输出合并到 plan 的「改动范围」+「关键代码参考」+「实施步骤」参考字段。  
> 如果阶段零已做 light 侦察，deep 侦察应**扩展**而非重做（轻量 → 深度的逐步深入）。

**必须沉淀的产物**：

> ⚠️ **铁律**：Plan **必须沉淀为磁盘文件**（`plans/<VERSION>/<STORYID>-<slug>-plan.md`），不允许只在对话中存在。  
> 这是确保不同人/不同 Agent 的执行结果一致的关键机制。  
> 模板见 `plans/templates/plan-template.md`。

**Plan 文件必须包含的章节**：

1. **设计概述**：2-5 句话描述总体方案
2. **改动范围**：涉及仓库 + 文件改动清单（操作 / 路径 / 关联 FR / 说明）
3. **实施步骤**：按 Phase 分阶段列出，每步骤说明做什么、为什么、参考代码
4. **依赖关系**：步骤之间的先后依赖
5. **风险与应对**：风险概率 / 影响 / 缓解措施
6. **关键技术决策**：选了什么 / 为什么 / 淘汰了哪些替代方案
7. **待确认项**：拿不准的地方，等待人确认
8. **预估复杂度**：低 / 中 / 高 + 是否需要拆 tasks

**禁止行为**：
- 跳过 plan 直接写代码（除非人明确说"直接实现"）
- Plan 仅在对话中存在，不写入 `plans/` 文件
- Plan 中包含与 spec 无关的改动

---

## 阶段二点五：任务拆解（Tasks）

**触发条件**：Plan 已确认，准备开始实施；改动跨多个文件或多个阶段。

**必须沉淀的产物**：

> ⚠️ **铁律**：复杂改动必须沉淀 `tasks/<VERSION>/<STORYID>-<slug>-tasks.md` 文件，作为**实施过程中的活文档**。  
> 模板见 `tasks/templates/tasks-template.md`。

**Tasks 文件必须包含的章节**：

1. **进度概览**：各 Phase 的完成情况
2. **任务清单**：按 Phase 拆分，每条 task 标注关联文件 + 关联 FR
3. **验收标准映射**：spec 每条验收标准对应哪些 task
4. **偏离记录**：实施中偏离 spec/plan 的所有点，包含日期、原因、处理建议
5. **阻塞记录**：遇到的阻塞、等待对象、状态
6. **完成定义（DoD）**：判断本 spec 是否真正实施完成的清单

**何时可以省略 Tasks**：

- 单点 bugfix 且 plan 已经足够清晰（< 50 行代码改动）
- 纯文档/注释修改
- 单文件单函数级别的改动

**禁止行为**：
- 复杂改动不拆 tasks，只在对话中"心算"
- 实施过程中不更新 tasks 状态（实时勾选 `[x]`）
- 偏离 spec/plan 但不记录到「偏离记录」

---

## 阶段三：实施（Implement）

**触发条件**：Plan 已确认（plan 文件已写入 `plans/`）；如有 tasks，则 tasks 文件已就绪。

**前置动作 — Git 准备**：

> ⚠️ 进入阶段三**必须**先在所有相关仓库准备好 feature 分支。完整流程：

```bash
# 每个改动仓库执行：
git checkout <baseline>           # 切到基线分支
git pull -r origin <baseline>     # rebase 模式同步最新
git checkout -b {feature|hotfix}/<spec-name>   # 拉出 feature 分支
```

- 基线分支从 plan「涉及仓库」表读取（详见 `docs/git-workflow.md`）
- 所有相关仓库使用**同一分支名**
- 如分支已存在（多人协作 / 恢复实施）→ `git switch` 切到该分支即可

**实施期间的 Commit**：

实施过程中可以产生**多个 commit**，每个 commit 都按规范写 message：

```
<type>(<scope>): <subject> --story=<STORYID> [#finish]
```

例：`feat(palm): 首页面板新增空中录掌关联数据 --story=134389917`

- 中间过程的 commit **不要**加 `#finish`
- 最后一笔（或唯一一笔）加 `#finish` 用于关闭 story
- 详见 `docs/git-workflow.md` 第 2 章

**必须遵循的原则**：

1. **严格按 plan 执行**，不擅自增加或减少改动范围
2. **每次修改前先读取目标文件**，确认当前内容，避免覆盖
3. **限制改动范围**：只修改与当前 spec 直接相关的代码
4. **保持现有代码风格**：缩进、命名、注释风格与现有代码一致
5. **涉及接口变化时**，在实施前提醒可能的影响
6. **涉及数据结构变化时**，确认是否需要迁移或兼容处理

**代码质量要求**：
- 新增代码必须包含必要的输入验证和错误处理
- 不引入未使用的导入或依赖
- 公共接口必须有清晰的注释

**禁止行为**：
- 修改与 spec 无关的代码
- 在实施过程中擅自扩大或缩小 spec 范围
- 输出大段未修改的上下文代码

---

## 阶段三补：偏离与回流（Sync-back）

**触发条件**：实施过程中发现 spec 或 plan 与现实不符（措辞、命名、流程、FR 边界）。

**核心原则**：**不沉默、不私改**。

- 严禁 AI 或 Dev 自己默默把 spec 改了
- 严禁按自己的理解写代码而不告知 Spec Author
- 偏离必须**先记录、再决策、最后回流到源文件**

**必须执行的步骤**：

### Step 1: 立即记录

在 `tasks/<VERSION>/<STORYID>-*-tasks.md` 的「偏离记录」表格追加一行：

```
| 日期 | 偏离点 | 原因 | 严重度 | 状态 |
| 2026-06-08 | FR-3 漏 userId 校验 | 实施时发现 | 中 | open |
```

### Step 2: 严重度判定

| 严重度 | 判定标准 | 是否暂停实施 |
|--------|---------|-------------|
| 🟢 小（minor） | 措辞、命名、补充细节，不改 FR 边界、不改验收标准 | 否，继续做完 Phase |
| 🟡 中（major） | 流程多/少一步、参数名/类型变更、漏 1 条容错 | 是，暂停当前 task |
| 🔴 大（escalate） | FR 边界变化、新增/删除功能、架构无法支持 | 是，停止整个 spec 实施 |

### Step 3: 决策

| 严重度 | 决策路径 |
|--------|---------|
| 🟢 小 | Dev Owner Phase 末直接修 spec 措辞 → status 改为 `spec-updated` |
| 🟡 中 | Dev Owner + Spec Author 短会三选一：a) 修订 spec b) 拒绝偏离 c) 拆独立 spec |
| 🔴 大 | 重走阶段一（Spec 评审会）三选一：a) 大改 spec b) 拆为多个 spec c) 暂停 spec |

### Step 4: 回流到源文件

**Spec 修订**（决策为「修订 spec」时）：
1. Spec Author 改 spec 对应章节
2. spec `Status` 改为 `in-progress`
3. spec `Updated` 日期刷新
4. 在 spec **末尾「修订记录」**追加：日期 / 修订点 / 关联 tasks 偏离记录 / 修订人

**Plan 联动**（如必要）：
- 改 `plans/<VERSION>/<STORYID>-*-plan.md` 对应章节，刷新 `Updated`

**Tasks 更新**：
- 偏离记录 status → `spec-updated` / `rejected` / `escalated` 之一
- 新增 task → 对应 Phase 末尾追加，标题加前缀 `[追加]`
- 废弃 task → 用 ~~`T2.3`~~ 划线，**不删除**（保留追溯）

### Step 5: 强制验证

实施完成后必须执行 `/spec-sync`：

```
请对 specs/<VERSION>/<STORYID>-<slug>.md 执行 spec-sync
```

确认 spec / plan / tasks / 代码 / 测试 五方一致。任何 ❌ 必须修复后再合并 PR。

**偏离记录的合法 status 枚举**：

| status | 含义 |
|--------|------|
| `open` | 刚记录，未决策 |
| `pending-decision` | 暂停 task，等 Spec Author 决策 |
| `spec-updated` | 已回流到 spec |
| `plan-updated` | 已回流到 plan（spec 不变） |
| `accepted-as-is` | 决策为接受偏离，不改 spec/plan |
| `rejected` | 决策为按原 spec 实施 |
| `escalated` | 已升级为新 spec 或 spec 重做 |

**禁止行为**：
- 偏离记录留 `open` 状态就提 PR
- spec 已修改但不更新「修订记录」
- 修改 spec 但忘了刷新 `Status` 和 `Updated`
- 划掉的 task 直接删除

---

## 阶段四：测试（Test）

**触发条件**：代码实施完成。

**必须执行的步骤**：

1. 对照 spec 中的**验收标准**和**测试点**
2. 为每个验收标准至少编写一个测试
3. 覆盖 spec 中列出的**边界情况**
4. 测试应放在 `tests/` 目录中，命名与功能对应
5. 如果无法编写自动化测试（如环境限制），应输出手动测试步骤

**禁止行为**：
- 实现了功能但不提及测试
- 测试只覆盖 happy path，忽略边界情况

---

## 阶段五：变更摘要（Summarize）

**触发条件**：实施和测试完成。

**必须输出的内容**：

```markdown
### 变更摘要

**关联 Spec**：[spec 文件路径和 ID]

**改了什么**：
- [具体变更列表，精确到文件和函数]

**为什么这样改**：
- [关键设计决策的简要说明]

**影响范围**：
- [哪些模块/接口/数据受影响]

**风险点**：
- [已知风险或需要关注的地方]

**未完成项**：
- [如有遗留事项，列出]

**建议验证方式**：
- [如何验证这次修改是正确的]

**文档同步**：
- [是否需要更新 spec 状态、README 或 docs]
```

**禁止行为**：
- 完成实施后直接结束，不输出摘要
- 摘要过于笼统（如"修改了相关代码"）

---

## 阶段六：Push 与 MR（Push）

**触发条件**：阶段五 review 通过、所有验收标准已勾选、tests 通过。

**核心原则**：
- 每个改动仓库都用 `/spec-push` 命令执行（含 commit + 安全 rebase + push）
- commit message **必须**符合规范
- push 前**必须**先 rebase 基线分支（避免合并冲突）
- 多仓库 push 顺序：**被依赖优先**（如 proto → 业务代码）

**必须执行的步骤**：

### Step 1: 本地 Commit

每个改动仓库：

```bash
git status                       # 检查改动
git add <files>                  # 不用 git add .
git commit -m "<type>(<scope>): <subject> --story=<STORYID> [#finish]"
```

**Commit Message 规范**：

| 字段 | 必填 | 说明 |
|------|------|------|
| `<type>` | ✅ | feat / fix / refactor / docs / test / chore / perf / style |
| `<scope>` | ✅ | 模块名（小写） |
| `<subject>` | ✅ | 简洁中文描述 |
| `--story=<STORYID>` | ✅ | 与 spec frontmatter Story ID 一致；无 story 用 `--story=0` |
| `#finish` | 可选 | 仅在 Story 的**最后一笔** commit 加（合并 MR 时关闭工蜂 story） |

**例**：

```
feat(palm): 首页面板新增空中录掌关联数据&数据可配置 --story=134389917 #finish
fix(gateway): 修复空中录掌鉴权超时问题 --story=134029304 #finish
```

### Step 2: 安全 Rebase（每个改动仓库）

```bash
git checkout <baseline>
git pull -r origin <baseline>     # 拉取基线最新代码
git checkout {feature|hotfix}/<spec-name>
git rebase <baseline>              # rebase 到基线最新
```

如有冲突：
- 暂停 push，沟通解决
- 解决后**重新跑测试**（rebase 可能引入运行时不一致）
- 测试通过后继续

### Step 3: Push

```bash
# 首次 push
git push -u origin {feature|hotfix}/<spec-name>

# rebase 后再次 push
git push -f origin {feature|hotfix}/<spec-name>
```

### Step 4: 多仓库合并顺序

如有跨仓库依赖（如 proto + 业务代码）：

```
被依赖仓库（如 palm_proto）→ 先 push + 合并 + 等流水线
   ↓
依赖仓库（如 palm_local）→ 更新依赖 → rebase → push + 合并
```

每个仓库的 MR 在描述中相互引用：`palm_proto!123`、`palm_local!456`。

### Step 5: 合并后 sync

所有 MR 合并后，执行 `/spec-sync`：
- spec status 改为 `implemented`
- 验证三件套 + 代码 + 测试一致

> 仓库内不再维护聚合的 `specs/INDEX.md`（已移除，避免 MR 冲突）。Spec 索引以 iWiki 文档为单一发布出口，由专人/工具按需运行 `/spec-index` 覆盖发布，**不在个人开发流程中执行**。

**禁止行为**：

- commit message 不符合规范（缺 type/scope/--story）
- 跳过 rebase 直接 push（可能造成合并冲突）
- 多仓库不按依赖顺序合并
- 在 push 前没有跑通测试
- 把 `#finish` 加到中间 commit（会过早关闭 story）

**详细操作手册**：见 `docs/git-workflow.md`。

---

## 流程例外

以下情况可以简化流程：

1. **纯 bug 修复**（有明确的错误和修复方案）：可以省略 plan，但仍需输出摘要
2. **人明确说"直接做"**：可以合并 plan 和 implement 步骤
3. **纯文档修改**：可以省略测试步骤

即使在例外情况下，变更摘要**不可省略**。
