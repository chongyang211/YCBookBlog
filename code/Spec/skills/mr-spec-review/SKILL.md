---
name: mr-spec-review
description: MR Spec 评审技能。当用户给出一个Git 平台（GitHub / GitLab / 工蜂） MR 地址（含 intake/design/spec/plan/tasks 任意组合的文档），要求评审他人提交的这些文档（必要时结合实施代码），并把问题作为评论回写到 MR 时使用。评审阶段**按端分流**：先识别 MR 涉及哪些端（前端/后台/识别算法/设备端/移动端SDK/跨端契约），再加载对应端的专业评审依据文档（`review-guides/`，每端含可人工追加的专业注意点专区），用各端准确视角评审、按端分组产出问题。既支持完整 spec/plan/tasks MR，也支持**只含 intake + design 的早期技术方案 MR**（结合当前代码逻辑评审 design 技术方案是否合理、给出整体评估与更优建议）。适用于跨成员 spec 评审、技术方案（design）评审、Code Review 前置的 spec 复核、迭代评审等场景。既可**本地手动执行**（人工确认后投递），也可放进**通用 AI agent / CI 流水线自主执行**（分支 push 触发、自动投递评论、多轮 loop 到无问题）。此外还提供**记忆经验提交功能**：当用户说"提交 spec review 的 memory / 推送评审经验 / 把 memory 改动提交上去"等，触发本 skill 定位工作区内的 CoSpec 仓、检测 `memory/` 是否有待提交改动，有则提交并推送到记忆分支、发起（或更新）合并到 master 的 MR（见 Step 9）。**不适用于**对自己刚完成的实施做规则回顾（那是 `/spec-review` 命令的范畴）。
---

# MR Spec 评审（MR Spec Review）

对**他人提交的 intake / design / spec / plan / tasks MR** 做结构化评审，必要时拉取实施代码联合评审，最终把问题以**总评 + 行内评论**形式回写到Git 平台（GitHub / GitLab / 工蜂） MR 上。

> 📌 **两类典型 MR 都要能评**：
> 1. **完整 spec 类**：含 spec/plan/tasks（可能已实现，需联合代码评审）。
> 2. **早期技术方案类**：只含 **intake + design**（spec 尚未起草）。这类要**结合当前代码逻辑评审 design 技术方案是否合理**，做整体评估——方案本身有没有更优选择、分层/抽象/选型是否贴合现状、细节上哪些要调整。见 Step 4.6。

> ⚠️ **本 skill 是 CoSpec 仓库专用**：所有 spec/plan/tasks 产物都在 CoSpec 仓（`<ORG>/CoSpec`），业务代码在其 `src/<repo>/` 子目录下的各业务仓。skill 运行时可能被放在**任意工作目录**（本地 IDE、或通用 AI agent / CI 容器），因此**第一步必须先在当前工作区定位到 CoSpec 仓**（见 Step 0），再往下走。

## 何时使用

- 用户给出一个 MR 地址（如 `<GIT-HOST>/<ORG>/CoSpec/-/merge_requests/10`），要求"评审这个 spec / 这个 MR / 这个技术方案"。
- 用户说"评一下 XX 的 spec / design"、"给这个 MR 加评论"、"看下 spec 哪里不合理"、"这个技术方案行不行"。
- MR **只含 intake + design**（还没写 spec）→ 走技术方案评审路径（Step 4.6），结合代码评估方案合理性。
- 用户说**"提交 spec review 的 memory / 推送评审经验 / 把 memory 改动提交上去 / 同步记忆库"** → 走**记忆经验提交功能**（Step 9）：定位 CoSpec 仓 → 检测 `memory/` 待提交改动 → 提交并推送到记忆分支 → 发起/更新合并到 master 的 MR。

## 何时**不**使用

- 评审自己刚完成的实施代码（用 `/spec-review` 命令或 `code-review` skill）。
- 评审纯代码 MR、无 spec 文档（用 `code-review` skill）。
- 只想本地分析 spec 完整性、不投递评论（用 `spec-analysis` skill）。

---

## 执行环境与模式

本 skill 有两种运行模式，**核心流程（Step 0~4 评审逻辑）完全一致**，只在"是否要人工确认后投递"和"是否沉淀记忆"上有差异。

| 模式 | 触发场景 | 投递评论 | 记忆沉淀 |
|------|---------|---------|---------|
| **A. 交互式（本地手动）** | 人在 IDE 里手动触发评审 | Step 5 出报告 → Step 6 用户确认 → Step 7 投递 | 询问用户后追加 |
| **B. 自主式（CI / 通用 AI agent）** | 分支 push → CI 触发；或无人值守的 agent 自动跑 | **跳过确认，直接投递总评 + 行内评论**（Step 5/6 降级为日志） | 复评时**自动**沉淀已解决项（仍做去重） |

**模式自动识别**（优先级从高到低）：

1. 用户/调用方**显式指定**模式（如 "CI 模式跑一遍"、"直接投评论别问我"）→ 按指定。
2. 存在 CI 环境变量（`CI=true` / `GITLAB_CI` / 流水线注入的 MR 上下文）→ **模式 B**。
3. 无人交互上下文（batch/headless agent）→ **模式 B**。
4. 默认（本地有人交互）→ **模式 A**。

> ⚠️ 模式 B 下**没有人把关**，因此对"投递质量"要求更高：
> - **必须同时投递「总评」+「行内评论」两部分，缺一不可**：总评（`is_person_note=true`）给整体结论，行内评论（`is_person_note=false`）逐条落到具体文件行。**不允许只投总评、也不允许只投行内**。
> - 只投 🔴/🟠（严重+一般），🟡 轻微问题降级为**总评里一句话汇总**，避免刷屏。
> - 复评时**严格去重**（见 Step 2）：同一问题只在初评投一次，后续轮次不重复。
> - 每条评论必须自解释（作者不在场问不了），带明确修改方向 + 代码位置。

### 🔒 并发安全原则（共享工作区多会话，必守）

做成 CI / 通用 agent 自动化后，**同一工作区可能同时跑多个 MR 的 review 会话，共用同一份 CoSpec 仓和 `src/` 目录**。因此：

1. **CoSpec 仓与 src 业务仓在共享工作区里都视为"只停在基线、只读"的共享资源**，**任何会话都不得把它们切到某个 MR 专属分支**（`git checkout <feature/source_branch>` / `reset` 到非基线 / 无独占路径的 `worktree` 都是会互相踩踏的全局副作用）。
2. **要读某个分支/commit 上的文件** → 一律用Git 平台（GitHub / GitLab / 工蜂） MCP **按 ref 只读拉取**（`get_blob_content` / `get_repository_tree`），不动本地 git 状态。
3. **确需大范围本地 grep 某分支代码** → 只能用**会话独占唯一路径 + detached commit 的 worktree**，用完即删；且仅在**确认无并发**的本地单会话下使用。
4. 停在基线的共享仓被并发 `fetch + reset --hard origin/<baseline>` 是**幂等收敛**的，安全；偶发 `.git/index.lock` 短暂重试即可。
5. **记忆回写同样不碰本地工作副本**：Step 8.2 产生的 `memory/` 变更通过Git 平台（GitHub / GitLab / 工蜂） MCP 写 API 提交到**专用记忆分支**（`chore/mr-review-memory`），不在共享 CoSpec 工作副本上 `commit/push`（详见 EVAL-SPEC §9.5）。

---

## 输入

| 参数 | 必选 | 来源 | 说明 |
|------|------|------|------|
| MR 地址 | 是 | 用户提供 / CI 上下文注入 | 形如 `<GIT-HOST>/<group>/<project>/-/merge_requests/<iid>` |
| 运行模式 | 否 | 自动识别 | A 交互式 / B 自主式，见上节 |
| 评审范围 | 否 | 默认全量 | 可指定"只评 spec"或"包含代码评审" |
| 投递方式 | 否 | 默认 C 模式 | 见 Step 7 |

---

## 执行步骤

### Step 0: 定位 CoSpec 仓库 + 强制同步 src 代码仓（**每次必做**）

> 本 skill 可能运行在任意工作目录（本地 IDE / CI 容器 / 通用 agent），且业务代码仓会随时间落后于远端。因此**每次评审最开始**都要先定位 CoSpec 仓、再把所有业务代码仓强制拉到远端最新，保证后续"代码验证评审"（4.5）看到的是最新现状，而非过时代码。

#### 0.1 在当前工作区定位 CoSpec 仓

按以下顺序探测，找到 CoSpec 仓根目录（记为 `$COSPEC_ROOT`）：

1. **当前目录**：`git remote -v` 若指向 `<group>/CoSpec`（与 MR URL 的 group/project 一致）→ 当前目录即 `$COSPEC_ROOT`。
2. **向上/向下探测**：在当前工作区内查找同时具备 `specs/`、`plans/`、`tasks/`、`skills/mr-spec-review/` 目录，且 `git remote` 指向 CoSpec 的目录。
   ```bash
   # 例：从工作区根向下找
   find . -maxdepth 3 -type d -name CoSpec 2>/dev/null
   # 或校验候选目录
   git -C <候选> remote -v | grep -i CoSpec
   ```
3. **仍找不到** → 停下来报告："当前工作区未定位到 CoSpec 仓，请提供 CoSpec 仓路径或先 clone。"**不要**在非 CoSpec 目录上瞎跑。

> ⚠️ **Knot 部署歧义（重要）**：从 Knot 包运行时，**当前目录是 Knot 打包副本**——它虽含 `skills/mr-spec-review/` 子目录，但**不是 CoSpec 的 git checkout**（`git remote` 不指向 CoSpec），故第 1 步会落空，必须继续探测/clone 真正的 CoSpec 仓。**不能把 Knot 包目录误判成 `$COSPEC_ROOT`**。若运行环境无 CoSpec checkout（评审的 spec 与 memory 都需要它）→ 由流水线/agent 用 `GONGFENG_TOKEN` clone 后再继续。

> 定位成功后，**评审数据（specs/plans/tasks/src/memory）的相对路径都以 `$COSPEC_ROOT` 为基准**（`$COSPEC_ROOT/specs/...`、`$COSPEC_ROOT/src/<repo>/...`）；**唯独执行逻辑（SKILL.md/scripts）来自 Knot 包**，见 Step 0.3。

#### 0.2 同步 src/ 业务代码仓的**基线分支**到远端最新（**默认按端按需 · 全量兜底**）

代码验证（4.5）主要对照**基线（主干）现状**判断方案是否合理（现有抽象、命名、分层、复用点）。因此把相关业务仓的**基线分支**同步到 origin 最新、并**始终停在基线**；缺失的仓自动 clone。

> ⚡ **默认按端按需同步（省时关键）**：一个 MR 往往只涉及 1~3 个端的少数仓，**没必要每轮都全量同步 ~20 个仓**。因此本步**默认延后到"端识别"之后**执行（见 **Step 1.6**）——先从 spec/design 识别涉及端 → 映射到 `src/` 目录 → 只同步这些仓（脚本 `REPOS_FILTER`）。**全量同步仅作兜底**：端无法提前判定、或涉及跨端契约需广扫时才全量。

skill 内置脚本（**推荐**，内含完整"目录↔基线分支↔Git 平台（GitHub / GitLab / 工蜂）地址"映射，已处理并发安全 / 缺失 clone / 密钥不落盘 / 并行同步）：

```bash
# 按需同步（默认）：只同步端识别命中的仓，目录名空格或逗号分隔
COSPEC_ROOT="$COSPEC_ROOT" GONGFENG_TOKEN="$GONGFENG_TOKEN" \
  REPOS_FILTER="<设备管理仓库> proto" \
  bash "$COSPEC_ROOT/skills/mr-spec-review/scripts/prepare-src.sh"

# 全量同步（兜底）：端不明确 / 跨端契约广扫时用，不传 REPOS_FILTER
COSPEC_ROOT="$COSPEC_ROOT" GONGFENG_TOKEN="$GONGFENG_TOKEN" \
  bash "$COSPEC_ROOT/skills/mr-spec-review/scripts/prepare-src.sh"
```

> ⚙️ 脚本已内置**并行同步**（并发度 `PREPARE_SRC_JOBS`，默认 8）：多个仓后台并发拉取，总耗时从「Σ(各仓)」降到「max(各仓)」。

> 脚本维护点：新增业务仓 / 基线分支变动时，改 [`scripts/prepare-src.sh`](./scripts/prepare-src.sh) 里的 `REPOS` 映射即可（与 `docs/git-workflow.md` 保持一致）。**识别算法仓 `algorithm-repo`（来自 `<ORG>/<算法组>`）平铺在 `src/algorithm-repo`**；`<ORG>/<算法组>` 原有的 `<业务主仓库>`/`proto` 逻辑已合并进 `<ORG>/<子组>` 的 `src/<业务主仓库>`/`src/proto`，不再单独同步。

**边界与安全**（脚本已内置，理解即可）：

- ✅ 只同步**基线分支**并**停在基线**——评审只读代码，业务仓不应有需保留的本地改动。
- ⚠️ **共享工作区 + 并发会话（关键）**：多个 review 会话共用同一 `src/` 目录时，**所有会话都只把仓库停在同一基线**，`reset --hard origin/<baseline>` 是**幂等**的，并发执行都收敛到同一 commit → 安全。**绝不允许任何会话把共享的 src 仓切到某个 MR 的 feature 分支**（那会破坏其他会话的基线假设，见 Step 3）。
- ⚠️ **模式 A（本地手动，独占工作区）**：若某仓有未提交改动 → **不要静默 `reset --hard`**，先提示用户，同意后再强制。**绝不丢用户工作区改动。**
- ✅ **模式 B（CI / agent）**：工作副本无人依赖，可直接 `fetch + reset --hard`；`.git/index.lock` 等并发锁短暂重试即可。缺失私有仓靠 `GONGFENG_TOKEN` 走 HTTPS clone。
- **同步失败**（网络/权限/无 token）→ 脚本 best-effort 记录并继续；模式 A 提示用户，模式 B 在总评注明"某仓未同步，代码验证可能不完整"。

> 注意：Step 0.2 只把业务仓停在**基线**（保证"现状"最新，且并发安全）；**不**在共享仓上切 feature 分支。若确需看 MR feature 分支上的代码，见 Step 3 的并发安全读法。CoSpec 仓自身 MR 文件的加载见 Step 1.5（MCP 只读）。

#### 0.3 运行时资产解析：logic 与 data 分离（**Knot 部署场景必读**）

> 📌 本 skill 打包提交到 **Knot 平台**独立运行时，运行环境里存在**两份** `mr-spec-review` 副本：Knot **打包副本**（实际执行 SKILL.md 的地方，发布时**冻结**）与 CoSpec 仓的**活副本**（`$COSPEC_ROOT/skills/mr-spec-review/`，memory 持续被闭环更新）。若不显式区分，agent 会误读 Knot 包里**冻结的旧 memory**、且写回目标含糊 → **经验反哺闭环失效**。

**因此运行时严格按「逻辑 vs 数据分离」解析资产：**

| 类别 | 内容 | 来源（运行时） | 可写? |
|------|------|---------------|-------|
| **执行逻辑** | `SKILL.md`、`scripts/prepare-src.sh` | **Knot 打包副本**（当前正在执行的这份），冻结、按发布版本走 | 只读 |
| **经验数据** | `memory/`（含 `eval-corpus/`、`attic.md`）、`review-guides/` | **CoSpec 仓活副本**，经Git 平台（GitHub / GitLab / 工蜂） MCP 读**最新**（见下） | 读写（写走 §专用分支） |

**数据读取来源（唯一真相，按优先级）**：

1. **首选**：经Git 平台（GitHub / GitLab / 工蜂） MCP `get_blob_content` / `get_repository_tree` 按 **记忆分支 `chore/mr-review-memory` 的 ref** 读 `skills/mr-spec-review/memory/**` 与 `review-guides/**`——拿到**跨会话共享的最新**经验（不受 Knot 冻结、也不受本地 master 基线滞后影响）。
2. **兜底**：MCP 不可用时，退回读 `$COSPEC_ROOT/skills/mr-spec-review/`（Step 0.1 已定位）的本地副本，报告注明"经验数据可能非最新"。
3. **冷启动种子**：连 `$COSPEC_ROOT` 都定位不到时，才用 **Knot 打包副本**里的 memory/review-guides 兜底，并**明确告警"使用冻结种子，本轮不写回"**。

**关键纪律**：
- ❌ **绝不**把 SKILL.md 里 `./memory/...`、`./review-guides/...` 的相对路径当成"从 Knot 包读"——那是冻结快照。运行时一律重定向到上面的 CoSpec 活副本来源。
- ❌ **绝不**把经验写回 Knot 打包副本（下次发布即冲掉）；写回目标恒为 CoSpec 仓的记忆分支（见 EVAL-SPEC §9.5）。
- ✅ review-guides（人工维护）默认在 master 上编辑，读取时若记忆分支已 rebase/包含 master 最新则一并读到；否则从 master ref 读 guides、从记忆分支 ref 读 memory。

### Step 1: 解析 MR 并拉取上下文

1. **解析 URL** 得到 `project_id`（如 `<ORG>/CoSpec`）和 `iid`。
2. **调用Git 平台（GitHub / GitLab / 工蜂） MCP**：
   - `search_merge_request(project_id, iid)` → 取 `merge_request_id`（注意不是 iid）、`source_branch`、`source_commit`、`target_branch`、作者、状态。
   - `get_merge_request_changes(project_id, merge_request_id)` → 取**全部 diff**（不要 `diff_file_only`，要拿到行号定位）。
   - `search_merge_request_notes(project_id, merge_request_id)` → 拉历史评论（含他人评审 + 作者修复说明）。
3. **识别文件类型**（智能识别，按文件路径分类）：
   - `docs/intake/<VERSION>/<STORYID>-<slug>.md` → intake（需求接收）
   - `designs/<VERSION>/<STORYID>-<slug>-design.md` → design（技术方案）
   - `specs/<VERSION>/<STORYID>-<slug>.md` → spec
   - `plans/<VERSION>/<STORYID>-<slug>-plan.md` → plan
   - `tasks/<VERSION>/<STORYID>-<slug>-tasks.md` → tasks
   - 其他 `.md` → 一般文档（次要）

4. **判定 MR 类别，决定评审路径**：
   - **含 spec**（可能带 plan/tasks/design）→ 走完整评审（Step 4.1~4.5）。
   - **只含 intake + design（或只有 design），无 spec** → 这是**早期技术方案 MR**，**照常评审**，走 Step 4.6（结合代码验证 design 技术方案合理性 + 整体建议）。**不要**因为"没有 spec"就判定为非 spec 类而拒评。
   - **只有 intake、无 design 也无 spec** → 提示用户"该 MR 仅有 intake，缺技术方案/spec，可评审需求描述的清晰度与边界，但无法评估技术方案"；征询后按 intake 维度轻量评审。
   - **完全无 intake/design/spec/plan/tasks** → 告诉用户该 MR 非本 skill 适用范围（纯代码 MR 走 `code-review`）。

> 评审范围：只评 MR 中**实际存在**的类型，缺失的不强求作者补。**intake/design-only 是完全合法的评审对象**（对应 spec 流程的"0前置"阶段，见 `docs/spec-flow-overview.md`）。

### Step 1.5: 加载 MR 版本的 spec 文件全文（**必做 · 默认只读拉取，不切分支**）

目的：拿到 MR 中修改的 intake/design/spec/plan/tasks **文件全文**（而非残缺的 diff 片段）做上下文完整的评审。

> ⚠️ **并发安全（核心约束）**：CoSpec 仓在共享工作区里可能同时被多个 review 会话使用（尤其做成 CI 自动化后，多个 MR 的流水线并发跑、共用同一目录）。因此**默认不对 CoSpec 仓做任何 `checkout` / `reset` / `worktree`**（这些是全局副作用，会让并发会话互相踩踏），而是用Git 平台（GitHub / GitLab / 工蜂） MCP **按 `source_commit` 只读拉取文件内容**。

#### 方式 A：MCP 只读拉取（**默认 · 并发安全 · 两种模式都用**）

1. 从 Step 1 的 diff 拿到 MR 改动的文件清单（intake/design/spec/plan/tasks）。
2. 对**每个**改动文件，按 MR 的 `source_commit` 拉全文（**多个文件并行发起 `get_blob_content`**，减少串行等待）：
   ```
   get_blob_content(project_id=<CoSpec>, sha=<source_commit>, file_path=<文件路径>)
   ```
   - 用 `source_commit`（而非分支名）保证与 diff 行号、评审基准完全一致，且不受作者随后新 push 影响。
   - 文件很大时可配合 `start_line`/`end_line` 分段取。
   - **design MR 评审时**：即使 MR 只改了 design，也要把**关联 intake**（design frontmatter 的 `关联 Intake` 字段路径）一并只读拉取，理解需求原意后再评方案是否贴合需求。
3. **兄弟 spec / 交叉一致性检查**需要读 MR 之外的文件时，同样只读：
   - `get_repository_tree(project_id, ref_name=<source_commit>, path='specs/<VERSION>/', max_depth=1)` 列出同版本 spec；
   - 再对目标文件 `get_blob_content` 取全文。
   - 若要对比**基线上的现状**（如与已合入 master 的 spec 冲突）→ 用 `ref_name=<target_branch>`（如 `master`）只读拉取，**不切本地分支**。

> ✅ 全程零本地 git 副作用 → 多个会话在同一工作区并发 review 互不干扰。这是**默认且推荐**方式，本地手动 / CI / 通用 agent 都用它。

#### 方式 B：会话独占 worktree（**可选 · 仅单会话本地深挖时**）

仅当你在**本地单会话**、确实需要对 CoSpec 仓做大范围本地 grep / 用编辑器全文浏览，且**确认当前无其他并发 review 会话**时才用。**并发 / CI 模式禁用本方式。**

- 必须用**会话独占的唯一路径**（带 iid + 随机后缀），且用 `source_commit` detached，避免与其他会话/主工作目录冲突：
  ```bash
  # 在 $COSPEC_ROOT 内
  git fetch origin <source_branch>
  WT=../CoSpec-mr<iid>-$RANDOM
  git worktree add "$WT" <source_commit>   # detached，行号最准
  # 之后读文件走 "$WT/<file>"
  ```
- 评审完**必须清理**：`git worktree remove "$WT"`。
- ❌ **绝不**在共享仓上 `git checkout <source_branch>`（会把共享目录切走，破坏其他会话）——旧版本的"直接切分支"方式已废弃。

#### 通用兜底

- MCP 拉取失败（网络 / 权限 / 文件已删）→ 退化为**基于 diff 片段**评审，并在报告里注明"未取到全文，上下文可能残缺"。
- worktree 相关命令失败 → 直接改用方式 A（MCP 只读），不要卡在本地 git 上。
- 用户明确说"只看 diff、别拉全文" → 跳过本步，报告注明"仅基于 MR diff"。

### Step 2: 判定初评/复评 + 历史评审回顾（loop 去重关键步骤）

> 🔁 **背景**：分支每次 push 都会触发 CI 重新评审，作者据评论在本地改完再 push，如此 loop 直到无问题。因此**每一轮评审前都要先判断"这是初评还是第 N 轮复评"**，复评必须基于上一轮结果做增量，**绝不重复投递同一问题**。

#### 2.1 判定初评 vs 复评

1. 从 `search_merge_request_notes` 取历史评论列表。
2. 判据：
   - **无本 skill / 无评审机器人产出的历史评论** → **初评**：全量评审，正常投递。
   - **已有本轮之前投递的评审评论** → **复评（第 N 轮）**：只评增量 + 复检未解决项，跳过已投过的问题。
3. 为支持机器识别自己投过的评论，**每条评论结尾附一个稳定指纹**（模式 B 必加，模式 A 建议加）：
   ```
   <!-- mr-spec-review:{问题指纹} -->
   ```
   `问题指纹` = `文件路径 + 锚点(章节/符号名) + 问题类别`（不要用会漂移的行号）做哈希/短串。复评时据此判定"这个问题上一轮是否已提过"。

#### 2.2 历史评审回顾

1. 梳理历史评论，按 `resolve_state` 分三类：
   - **已解决**（`resolved`）：作者已接受并修复 → 复检是否真的改好；**并进入 Step 8 的记忆沉淀候选**。
   - **未解决**（`unresolved`）：作者尚未处理 → 本轮**不重复新投**，但在总评里提醒"仍有 X 条未解决"。
   - **他人（非本 skill）评论**：关注但不与自己的指纹去重，避免和人工评审撞车。
2. 用 commit 历史对齐评论时间轴：
   - `get_commits_list` 拿 commit 列表；
   - 定位"上一轮评审 commit 之后**新增/修改**了哪些文件/章节" → **本轮重点 = 新增量 + 历史未解决项复检**。

#### 2.3 去重规则（强制）

- **同指纹已投过** → 本轮**不再投**（即便问题仍在，靠"未解决评论"本身提醒作者）。
- **作者改动后问题变形**（同一处衍生出新问题）→ 视为新问题，可投，但指纹要区分。
- **已解决项** → 不再投；改为进入记忆沉淀流程（Step 8）。

3. **输出评审上下文预热信息**：
   ```
   ## 评审上下文
   - MR: !{iid} 作者 @{author} 状态 {state}
   - 评审轮次：{初评 / 第 N 轮复评}
   - 已有 {N} 条本 skill 评论（{X} 未解决，{Y} 已解决）
   - 上轮评审后新增 {Z} 个 commit
   - 本轮策略：
     * 复检未解决项：[列表]
     * 已解决待沉淀记忆：[列表]（见 Step 8）
     * 重点评审增量：[文件 / 章节]
     * 去重跳过：[已投指纹数]
   ```

### Step 3: 判定 spec 实现状态（是否需要联合代码评审）

> 📌 **intake/design-only MR（无 spec）**：spec 尚未起草，谈不上"已实现"。此时**跳过下面的 Status 判定**，直接进入 Step 4.6 —— 但**仍必做代码验证**：design 是"技术方案"，其合理性只有对照**业务代码基线现状**（现有架构、抽象、复用点、分层）才能评。Step 0.2 已把 src 仓停在基线，直接只读 grep/read 即可。

按以下优先级判定（对应澄清 Q3 选项 D）：

1. **读 spec frontmatter** 的 `Status` 字段：
   - `draft` / `approved` → **未实现**，只评 spec 文档本身。
   - `implementing` / `done` → **已实现**，需联合代码评审。
2. **检查关联仓库**：
   - 从 spec 中提取 `Branch` 字段（如 `feature/134710417-iot-device-behavior-revamp`）。
   - 从"实施备注"/"关键挂载点"提取相关仓库（如 `<前端仓库>`、`<业务主仓库>`）。
   - 若 spec **未明确标注哪个仓库 / 哪个分支** → **停下来问用户**，不要瞎猜。
3. **访问业务代码做验证**（仅当判定"已实现"且仓库明确时）：

   > ⚠️ 与 Step 1.5 区分：Step 1.5 处理的是 **CoSpec 仓**里的 spec 文件；这里访问的是 **业务代码仓**（`src/<仓库>`）。两者是不同仓库。

   **默认基于基线（主干）现状评审**：Step 0.2 已把每个业务仓停在**基线并同步到 origin 最新**。方案合理性（现有抽象、命名、分层、复用点、字段/常量分布）几乎都靠**基线现状**判断——因此**默认直接在停于基线的共享仓上只读 grep / read**即可，无需切分支。

   **各业务代码仓的基线分支对照**（**权威来源 = [`scripts/prepare-src.sh`](./scripts/prepare-src.sh) 的 `REPOS` 映射**；此处为速查，改动以脚本为准）：

   | 目录（相对 `src/`） | Git 平台（GitHub / GitLab / 工蜂）仓 | 基线分支 |
   |------|------|---------|
   | `<业务主仓库>` | <ORG>/<业务主仓库> | `develop` |
   | `<协议仓库>` | <ORG>/<协议仓库> | `master` |
   | `<前端仓库>` | <ORG>/<子组>/<前端仓库> | `develop` |
   | `<设备管理仓库>` | <ORG>/<子组>/<设备管理仓库> | `develop` |
   | `<业务主仓库>` | <ORG>/<子组>/<业务主仓库> | `develop` |
   | `proto` | <ORG>/<子组>/proto | `master` |
   | `infrastructure` | <ORG>/<子组>/infrastructure | `develop` |
   | `algorithm-repo` | <ORG>/<算法组>/algorithm-repo | `master` |
   | `paymax_device` / `<支付仓库>` | <ORG>/… | `develop` |
   | `<激活服务仓库>` / `<IoT 服务仓库>` / `<IoT 服务仓库>` | <ORG>/… | `develop` |
   | `<SDK 仓库>` | <ORG>/<SDK 仓库> | `master` |
   | `<示例仓库>` | <ORG>/<示例仓库> | `master` |

   > ⚠️ **识别算法仓平铺**：`algorithm-repo` 来自 `<ORG>/<算法组>`，直接平铺在 `src/algorithm-repo`。`<ORG>/<算法组>` 原有的 `<业务主仓库>`/`proto` 逻辑**已合并进** `<ORG>/<子组>` 的 `<业务主仓库>`/`proto`（`src/<业务主仓库>`、`src/proto`），原 `src/<旧流水线目录>/` 分组已废弃删除；spec 引用识别侧 <业务主仓库>/proto 时直接用 `src/<业务主仓库>`、`src/proto`。

   > 如遇不在上表中的新仓库 → 优先查 `docs/git-workflow.md` 的《基线分支映射表》确认；仍不明确则**问用户**，不要瞎猜。

   **确需看 MR feature 分支上的实际实现代码时**：遵循 §并发安全原则与 Step 1.5 的并发安全读法（**绝不在共享 `src/<仓库>` 上 `git checkout`**，会破坏并发会话基线）：

   1. **首选只读拉取**（零副作用）：`get_blob_content(project_id=<业务仓 full path>, sha=<feature-branch 或其 commit>, file_path=<文件>)`，配合 `get_repository_tree(ref_name=<feature-branch>, path=...)` 定位文件。
   2. **需大范围 grep**：用会话独占 worktree（同 Step 1.5 方式 B：唯一路径 + feature commit detached，用完 `git worktree remove`），仅本地单会话、确认无并发时用。
   3. 都不可行（网络/权限失败）→ 退化为仅基线 + MR diff 评审，报告注明"未取到 feature 分支实现，代码验证可能不完整"。

   **兜底**：
   - **基线本地有分叉 / 有未提交改动**（模式 A 独占工作区）→ 停下告诉用户，不要自作主张 `stash` / `reset`。
   - **本地无 `src/<仓库>`** → 按 `docs/git-workflow.md` 地址 `git clone`（首选仍是只读 MCP 取文件，避免为一次评审 clone 大仓）。
4. **状态模糊**（如 Status=draft 但作者提了实施 commit）→ 明确问用户："这个 spec 已经实现了吗？要不要结合代码一起评？"

### Step 4: 评审执行（按维度）

> ⭐ **两轮评审模型（强烈推荐 / 默认执行）**  
>
> 单轮"读文档就下评论"很容易漏掉方案层问题。默认按**两轮**推进：
>
> - **第一轮：文档层评审**（4.1 + 4.2）—— 只看 spec/plan/tasks/design 内部一致性、模板合规、命名/风险/边界。产出 15~20 分钟能给出的结论。
> - **第二轮：代码验证评审**（4.5，**核心增量**）—— 结合业务代码仓真实实现，逐条核验 spec 中引用的**每一个**方法、常量、文件、字段号；对照现有抽象/分层评估方案是否合理；对照业界成熟实践评估选型。
>
> 用户明确说"只评文档、不看代码"才跳过第二轮。否则**必做**——因为大多数严重问题（分层错、重造轮子、命名与真实代码不一致、性能反模式）只有翻代码才能发现。
>
> 📌 **若 MR 是 intake/design-only（无 spec）**：第一轮只做 design 文档层自检，**第二轮走 Step 4.6（Design 技术方案评审）**——结合代码评方案合理性 + 整体建议，这是这类 MR 的核心。
>
> 报告时**清楚标注每一条问题来自"第一轮（文档层）"还是"第二轮（代码验证）"**，方便作者定位。

#### 4.0 端识别 + 按端加载评审依据（**每次必做 · 评审前置**）

> 🎯 **目的**：不同端（前端 / 后台 / 识别算法 / 设备端 / 移动端SDK）的关注点差异极大。评审前先判定 MR 涉及哪些端，加载对应的**专业评审依据文档** [`review-guides/`](./review-guides/)，用每个端准确的视角评审——而不是用一套通用维度"一锅烩"。

**① 识别涉及的端**：从 spec/design 的「关键挂载点 / 改动文件清单 / 跨系统影响表 / Branch 字段」提取涉及的 `src/` 仓库，按 [`review-guides/README.md`](./review-guides/README.md) 的**端路由表**映射到端（一个 MR 可命中多个端）：

| 端 | 命中的 `src/` 目录 | 评审依据 |
|----|-------------------|---------|
| **后台服务端** | `<设备管理仓库>`、`<业务主仓库>`、`<业务主仓库>`、`<激活服务仓库>`、`infrastructure` | [`review-guides/backend.md`](./review-guides/backend.md) |
| **识别·算法端** | `algorithm-repo`、`<业务主仓库>`/`proto`（识别侧逻辑，已合并入 <ORG>/<子组>） | [`review-guides/algorithm.md`](./review-guides/algorithm.md) |
| **前端** | `<前端仓库>`、`<支付仓库>/web`、`<SDK 仓库>/web` | [`review-guides/frontend.md`](./review-guides/frontend.md) |
| **设备端** | `<IoT 服务仓库>`、`paymax_device`、`<IoT 服务仓库>` | [`review-guides/device.md`](./review-guides/device.md) |
| **移动端·SDK** | `<SDK 仓库>`（android/ios）、`<支付仓库>/Android` | [`review-guides/mobile-sdk.md`](./review-guides/mobile-sdk.md) |
| **跨端契约**（横切） | 任意端改到 `*.proto`/接口/错误码/字段号，或 `proto`/`<协议仓库>` | [`review-guides/contract.md`](./review-guides/contract.md) |

> 端归属不明确 → 从 Branch/影响表推断；仍不明确则在报告注明"按最相关端评审"，模式 A 可问用户。**权威路由表以 [`review-guides/README.md`](./review-guides/README.md) 为准。**

**② 加载每个命中端的评审依据全文**（含底部「🖊 人工追加评审注意点」专区）：把该 guide 的 Checklist + 人工注意点，与 memory 里的 Pattern **一并**加载进当前评审的检查清单。

- 命中多个端 → 全部加载；涉及跨端字段/接口/错误码 → **额外加载 `contract.md`**。
- 逐条对照扫描，命中即标注来源：`命中 <end>-guide#<小节>`（Checklist）或 `命中 <end>-guide#人工-<编号>`（人工注意点）。

**③ 评审报告按端分组**（见 Step 5）：让作者一眼看清"哪个端有几个问题"。

> ⚠️ **加载优先级与顺序**：memory（历史真问题）+ 命中端 review-guide（专业依据 + 人工注意点）**都要读**，合并成检查清单。二者互补：memory 是"踩过的坑"，guide 是"这个端该看的专业项 + 专家前置的注意点"。

#### 4.1 Spec 文档评审维度（第一轮）

> **前置要求（强制）**：进入本节前，**必须先读完 [`memory/doc-process-patterns.md`](./memory/doc-process-patterns.md)**（文档层 Pattern），**并已在 Step 4.0 加载好命中端的 [`review-guides/*.md`](./review-guides/)**（含其人工注意点专区）。把 doc-patterns + 各端 guide 的文档层要求一并加载到检查清单。命中即标注 `命中 doc-patterns#DN` 或 `命中 <end>-guide#...`。

| 维度 | 关注点 |
|------|------|
| **方案合理性** | 目标 vs 非目标边界是否清晰、选型是否合适、是否有更轻的替代 |
| **逻辑正确性** | FR 间是否自洽、流程图是否闭环、状态机有无遗漏分支 |
| **实现优雅性** | 是否最小入侵、是否复用现有资产、是否避免过度设计 |
| **细节完整性** | 接口契约（URL / 字段 / 错误码）、数据结构、默认值、初始状态 |
| **场景边界** | 异常路径、并发竞态、空值 / 边界值、权限缺失、网络失败 |
| **安全（@security_rules）** | XSS / SQLi / 鉴权 / SSRF / 日志脱敏 / 危险 URL schema / 反序列化 |
| **性能** | 超时设置、N+1、缓存策略、批量、限流、大数据量降级 |
| **兼容性 / 灰度 / 回滚** | 老数据、老 URL、回滚路径、灰度策略 |
| **设备端兼容性（重要！）** | **若涉及 `devicegateway` / `wecarddevicegateway` 接口**：是否破坏旧设备版本？是否需保留老字段？设备 OTA 时间窗口考虑了吗？详见 4.3 |
| **可测试性** | 验收标准是否可验证、测试点是否覆盖关键路径与并发竞态、是否要求单测 |
| **i18n 三语完整性** | 涉及前端时，zh-CN / en-US / ja-JP 三语词条**全部具体**给出，不可留空 / 写"按已有同步" |
| **文档质量** | 注释、TODO 标记、修订记录 |
| **模板合规** | 对照 `specs/templates/spec-template.md` 的必填 frontmatter（Story ID / Status / Sibling Specs / Branch / Created / Updated）是否齐全、`Updated` 是否与修订记录最新日期一致 |
| **状态-修订记录一致性** | `Status: ready` 却在 ready 后新增 FR？→ 应回落 `in-progress` / `draft`；修订记录条目连续、TBD 编号连续无跳号 |
| **跨包 / 跨仓库影响** | spec 是否漏覆盖同名 / 同功能的另一个 package、是否影响调用方 |
| **兄弟 spec 交叉一致** | 同一 Story 的兄弟 spec 之间：**同名协议字段号/类型是否一致**（proto message、错误码、指令名、常量名）；`Sibling Specs` frontmatter 是否列全 |
| **与现有 spec 冲突** | 是否与已有 spec 重叠、命名冲突、约定冲突 |

#### 4.2 Plan / Tasks 评审维度（若存在）

| 维度 | 关注点 |
|------|------|
| **拆分粒度** | 步骤是否原子化、能否独立验证 |
| **依赖关系** | 跨步骤 / 跨仓库依赖是否标清楚、有无环 |
| **改动文件清单** | 是否与 spec FR 对得上、有无漏覆盖（重点：跨 package） |
| **风险点** | 是否标注高风险步骤、回滚预案 |
| **任务勾选项** | tasks 是否可执行（不可执行的"理解需求"类放掉） |

#### 4.3 设备端接口兼容性（**必查项**）

> 📌 命中设备端时，本节与 [`review-guides/device.md`](./review-guides/device.md)（设备端完整依据）+ [`review-guides/contract.md`](./review-guides/contract.md)（跨端契约）配合使用；本节是其中**兼容性**的硬性摘要。

只要 MR 涉及以下任一情况，必须**强制评审**兼容性：

- spec 文档中提到 `devicegateway` / `wecarddevicegateway` / `<设备管理仓库>` 中的接口
- 代码改动涉及 `src/<设备管理仓库>/`、`gateway/devicegateway/`、`gateway/wecarddevicegateway/`
- 字段名、错误码、协议格式变更

**评审点**：
- ✅ 新增字段必须有默认值，老设备不传时行为不变
- ✅ 不修改已有字段语义、不改类型
- ✅ 错误码只能新增、不能复用 / 改义
- ✅ 接口下线 / 改名前必须有兼容期（至少覆盖一个 OTA 周期）
- ❌ **破坏性变更直接标 🔴 严重风险**，要求作者说明灰度方案

#### 4.4 代码评审维度（spec 已实现时）

| 维度 | 关注点 |
|------|------|
| **AC 对齐** | spec 验收标准是否逐条在代码中体现 |
| **spec 偏离** | 与 spec 不一致处是否在「修订记录」回流 |
| **命名规范** | 与现有代码风格一致（参考同模块兄弟文件） |
| **目录 / 分层** | 后端：Gateway / Application / Domain / Repo 不串层；前端：跨 package 引用合理 |
| **复用 vs 重造** | 工具函数 / 现有组件是否已复用 |
| **错误处理** | 是否吞错、是否有日志、是否有降级 |
| **类型安全** | TS 的 `as any` / `any[]`、Go 的 interface{} 滥用 |
| **测试覆盖** | 纯函数有单测、关键分支有用例（参考 `rules/30-testing-rules.md`） |
| **安全 / 性能** | 代码层细化，参考 `skills/code-review/SKILL.md`（Go 后端） |
| **i18n key 实际落地** | 三语 json 文件中是否真的加了所有声明的 key |
| **TODO / `as any` / `// FIXME`** | 是否有遗留、是否有 `TODO(backend-align)` 等显式标记 |
| **死代码 / 注释代码** | 是否有 commented-out 代码、是否有未引用的 import |

#### 4.5 代码验证评审（第二轮 · 核心，spec 未实现也要做）

> 🎯 **目的**：即使 spec 尚未实施，**方案本身**的合理性/抽象度/命名/分层，都必须通过**翻业务代码**来验证——否则容易接受一个"看起来合理、落地会翻车"的方案。
>
> **触发条件**：只要本地可访问业务代码仓（`src/<repo>/` 存在或可 clone），就**必做**这一轮。
>
> **前置要求（强制）**：进入本节前，**必须先读完 [`memory/code-patterns.md`](./memory/code-patterns.md)**，**并已在 Step 4.0 加载好命中端的 [`review-guides/*.md`](./review-guides/)**（后台看 `backend.md`、设备端看 `device.md`、识别看 `algorithm.md` 等，含各自「人工追加评审注意点」专区；涉及跨端契约再加 `contract.md`）。把 code-patterns + 命中端 guide 的代码层 Checklist + 人工注意点全部加载进检查清单，逐个对照 spec/plan 扫描，命中即写评论并标注 `命中 code-patterns#N` 或 `命中 <end>-guide#...`。

> ⚡ **执行提效（减少往返）**：先把 spec/plan 引用到的**所有**代码符号（函数名 / 常量 / 字段 / 文件）**一次性结构化汇总成清单**，再用**单次多 pattern 的批量 grep**（`search_content` 一次传多个符号的正则）跨仓验证，而非逐个符号发一次搜索；多个独立文件的读取也**并行**发起。这样把"逐条串行核对"压成"批量并行核对"。

**必查清单（照单执行，别偷懒）**：

1. **spec/plan 引用的所有代码位置——逐个 grep 验证真实存在**
   - "参考 `xxx.go` 第 XX 行的 `YYY()` 函数"→ 打开该文件 grep `YYY`，验证：函数是否存在？行号是否准确？签名是否与 spec 描述一致？
   - **常见坑**：spec 里的行号往往过时（作者写时是对的，代码已 refactor），或**函数名拼写有误**（大小写、单复数、AppId vs AppAppId）。这些是 🔴 严重错误——dev 照抄会编译不过。

2. **spec 声称"直接复用"的方法/常量——验证是否真的可用**
   - 检查**导出可见性**：spec 说要复用的方法是否 `public/export`？跨包调用是否会撞私有边界？
   - 检查**分层**：被复用的函数所在包，与将要引用它的调用方是否符合项目 DDD 分层（controller → application → domain → repo，反向依赖是违规）？
   - 检查**是否已经有多份副本**：如果一个 helper 在项目里已经被拷贝了 2~3 次，说明它本该被抽公共包——本次实施是**顺手清理的好时机**，应写进评论。

3. **spec 提出"新增"的方法/字段——验证是不是其实已经存在或有更好落点**
   - 举例：spec 说"新增 `IsExampleDevice()` 方法"→ grep 一下 `isExampleDevice`（大小写不敏感），可能私有版本已存在，或有更通用的 `IsDeviceTypeOf(t)`；
   - 举例：spec 说"application 层加设备类型 if 校验"→ grep `checkCreateInstruction*` 之类，可能 domain 层已经有一个专门 hook 函数，本来就是为此设计的**天然落点**（本次实战的 #16 就是这类问题）；
   - **原则**：任何"新增抽象"前，先问"现有代码里有没有干这件事的地方"。

4. **spec 描述的结构体/proto message——逐字段核对真实定义**
   - 字段号、类型、字段列表**逐一比对**真实 proto / struct 定义；
   - 尤其兄弟 spec 之间引用同一 message 时，**每份 spec 都要与 proto 源对齐**（本次实战：后端 spec vs 设备端 spec `InstructionLinkMsg` 字段号错位 → 联调必挂）；
   - 完整字段列表（例如 `NewXxxParams` 有 18 个字段）→ spec 是否漏了几个 → 漏的字段有没有语义相关性（如 `SafeConfig`/`RateLimitAmount` 与安全/限流的关联）。

5. **命名/大小写/单复数**——三份文档（spec/plan/tasks）互相一致 + 与真实代码一致
   - 常见坑：spec 用 `xxx`、plan 用 `Xxx`、真实代码是 `XxxAppId`——三份文档互冲、且都错
   - 常见坑：spec 里同一个东西前后叫两个名字（前文 `ExampleAppId`，后文 `ExampleAppAppId`）
   - **必做**：对 spec 里高频出现的每个类名/常量名/方法名，全仓 grep 一次，取真实拼写

6. **配置/常量/枚举——是否与现有约定一致**
   - `ExpireDuration`、`RateLimit`、`IssueType` 等取值 → 全仓统计现有指令的分布，判断新指令取值是否**贴合业务语义**（本次实战的 #19：所有指令都 86400s，但远程开门这种强时效应特殊化）
   - 命名前缀/后缀是否遵循现有惯例（`camelCase` vs `snake_case`、`Errcode` 前缀等）

7. **依赖/资源代价——评估方案的隐性成本**
   - 每次调用都会 dial 新连接？（本次实战 #18）
   - 每次都全量遍历？
   - 引入新第三方库或 protobuf 依赖？
   - "best-effort 无副作用"是否被真实代码打脸？

8. **降级/失败路径——用代码验证"兜底"是否可靠**
   - spec 说"MQTT 失败短轮询兜底"→ 短轮询周期是多少？（读代码找 `IntervalSec` / 配置文件）
   - spec 说"超时兜底流转"→ 超时脚本扫描间隔多少？(`ExpireDuration` × 兜底延迟 = 用户实际感受)
   - 两者相加是否符合 spec 承诺的 SLA？（本次实战 #19：86400s + best-effort → 半天误开门）

**结合业界成熟实践评估**：

对于每个关键决策，思考"业界主流是怎么做的"：

| 主题 | 参考思路 |
|------|---------|
| 分层/依赖方向 | DDD / Hexagonal（domain 不依赖 application/controller；controller 不横向引用同层其他业务包） |
| 序列化复用 | DRY——`ToDeviceDTO` 和 `ToLinkDTO` 字段相同 → 复用而非并列 |
| RPC 客户端 | 连接池 vs 每次 dial（gRPC 官方推荐长连接 + `grpc.NewClient` 而非 `Dial` + `defer Close`） |
| 全量 vs 单点开关 | 大变更引入前先灰度（feature flag / 按指令名过滤），别一步 all-in |
| 幂等 / 去重 | 涉及物理副作用（开门、扣款）必须 `RequestId + 单指令锁`；MQTT + 轮询双通道尤其要有 |
| 高危操作权限 | RBAC 应有独立细粒度 permission，而非复用通用 permission（远程开门 ≠ 重启设备） |
| 过期时间 | 强时效指令（开门/扣款）不复用通用 24h，应短过期（分钟级）避免延迟消费误触发 |
| 错误码定义 | 分段位（403xx/409xx…）；只新增不复用；跨服务对齐同一码值 |
| Proto 演进 | 字段号只加不改，不同 spec 引用同一 message 必须与 proto 源对齐 |
| 观察者/事件 | 广播/信号 → 注册回调（避免全量广播 + 订阅方过滤的反模式；参考 v1.7.0 mqtt-integration） |

**产出**：把第二轮发现的问题，与第一轮问题**分开列在报告里**（"来自代码验证"标签），便于用户判断优先级 & 决定"是否要重构方案"级别的改动。

#### 4.6 Design 技术方案评审（intake/design-only MR · 核心路径）

> 🎯 **适用场景**：MR 只含 **intake + design**（或只有 design），spec 尚未起草。此时评审对象是**技术方案本身**，目标是在方案落地为 spec/代码之前，尽早发现"看起来合理、落地会翻车"的设计。
>
> **核心要求（用户强调）**：**必须结合当前业务代码逻辑来评审 design 技术方案是否合理**，做**整体评估**并给出建议——不只是挑文档格式问题，而是回答"这个方案好不好、有没有更优解、细节要怎么调"。
>
> **前置要求（强制）**：进入本节前，**必须先读完 [`memory/code-patterns.md`](./memory/code-patterns.md)**（方案/代码层经验），**并已在 Step 4.0 加载好该 design 涉及端的 [`review-guides/*.md`](./review-guides/)**（含人工注意点专区）——design 是"技术方案"，要用对应端的架构约定 / 高频坑 / 人工注意点去评方案是否贴合该端现状。同时把关联 intake 的需求原意加载进上下文（见 Step 1.5）。

**评审必做两件事：**

**① 结合代码验证方案合理性**（复用 4.5 的代码验证手法，聚焦"方案级"而非"引用级"）：

| 维度 | 关注点（对照 `src/` 基线现状） |
|------|------|
| **现状分析是否属实** | design「现状分析」描述的现有架构/模块边界/调用链，逐条 grep 业务代码核对——**是不是真的长这样**？有没有把已废弃的老路径当现状？ |
| **方案是否贴合现有架构** | 提出的新模块/新服务/新链路，是否与现有分层（DDD：controller→application→domain→repo）、现有抽象一致？会不会引入反向依赖、跨层直连？ |
| **复用 vs 重造** | 方案要"新建"的能力，代码里是否已有同等/相近实现（helper、已有服务、已有事件机制）？能复用就别新造。 |
| **落点是否最优** | 方案选的挂载点/落地位置，是不是现有代码里的"天然落点"？有没有更贴合的已有 hook/扩展点？ |
| **跨系统/跨仓库影响是否覆盖全** | design「跨系统影响表」对照真实调用方 grep——有没有漏掉的下游/调用方/同名功能包？ |
| **数据结构/接口方向是否可行** | 高层数据结构/接口方向，是否与现有 proto/struct/接口约定冲突（字段号、命名惯例、错误码分段）？ |
| **性能/资源代价** | 方案隐含的连接方式、遍历方式、第三方依赖，是否有性能反模式（每次 dial、全量扫描、N+1）？ |
| **时效/幂等/安全** | 涉及物理副作用（开门、扣款）→ 幂等/去重/短过期/细粒度权限是否在方案里就想清楚了？ |

**② 整体评估 + 给出建议**（这是 design 评审区别于 spec 评审的关键产出）：

- **方案本身有没有更优选**：针对每个关键决策点，对照「业界成熟实践」（见 4.5 末尾的参考表）与项目现状，评估作者的取舍是否合理；**如有更优/更轻/更贴合现状的替代方案，明确提出并说明理由**（不是含糊地"可以再想想"，而是给出具体的替代思路）。
- **方案对比是否充分**：design 模板要求关键决策「≥2 个备选 + 优缺点」。检查是否真的对比了，还是只写了倾向方案？淘汰理由是否站得住？
- **spec 拆分建议是否合理**：design「建议的 spec 拆分」表——拆分边界/依赖是否清晰？粒度是否合适（过粗难评审、过细增协作成本）？
- **风险与未决问题是否完整**：有没有明显该 open 的风险被漏掉？
- **细节调整清单**：把"方案大方向 OK、但细节要改"的点，逐条列成可执行建议（如"决策点 2 建议改用 X，因为现有代码 Y 已支持"）。

**产出结构（并入 Step 5 报告）**：design 评审的结论要能回答用户三个问题——
1. **方案整体可行吗**（一句话结论：可行 / 需调整后可行 / 建议换方案）；
2. **有没有更优方案**（列出替代方案 + 理由，没有则明确说"当前方案已是较优解"）；
3. **落地前要调整哪些细节**（可执行的逐条清单，带代码依据）。

> ⚠️ design 评审同样遵守**模式 B 必须投总评 + 行内评论**：总评给整体结论（可行性 + 是否有更优方案），行内评论逐条落到 design 的具体决策点/章节行。

> **模式 A（交互式）**：输出结构化报告到对话，**严禁直接调 MCP 写评论**，等 Step 6 用户确认。
> **模式 B（自主式）**：报告降级为**执行日志**（打到 CI 输出 / agent 日志即可），**不等确认**，直接走 Step 7 投递。

报告格式：

```markdown
# Spec Review: !{iid} {标题}

## 评审上下文
- 作者：@{author}
- 状态：{state}
- 评审轮次：{初评 / 第 N 轮复评}
- **涉及端**：{后台 / 前端 / 设备端 / 移动端SDK / 识别算法 / 跨端契约}（已加载对应 review-guides）
- 历史评论：{N} 条（{X} 未解决，{Y} 已解决）
- 本次重点：{增量评审 / 全量初评 / 复检}
- 去重跳过：{已投指纹数}

## 总评
{一句话整体评价：通过 / 需修改后通过 / 不通过}

## 方案整体评估（仅 intake/design-only MR）
- **整体可行性**：{可行 / 需调整后可行 / 建议换方案}
- **是否有更优方案**：{列出替代方案 + 理由；若无则写"当前方案已是较优解"}
- **落地前须调整的细节**：{可执行的逐条清单，带代码依据}

## 发现的问题（按端分组，已剔除上轮已投指纹）

### 【后台端】🔴 严重（必须修改）
1. **[问题简述]**（path: spec.md L77 · 命中 backend-guide#分层）
   - 现状：[一句话]
   - 建议：[一句话]

### 【设备端】🔴 严重
2. ...（命中 device-guide#4.1 兼容性 / 人工-N）

### 【前端】🟠 一般（建议修改）
3. ...

### 【跨端契约】🟠 一般
4. ...

## 亮点（可选）
- ...

## 待用户确认（仅模式 A）
1. 是否投递以上 {总数} 条评论到 MR？
2. 是否需要裁剪（按风险等级 / 按编号）？
```

### Step 6: 与用户澄清确认（**仅模式 A**）

> **模式 B 跳过本步**，直接进入 Step 7。

明确询问用户：

> 以上发现 {N} 条问题（🔴{X} 🟠{Y} 🟡{Z}）。请确认：
> 1. 是否全部投递？或按风险等级裁剪（如只投 🔴+🟠）？
> 2. 是否补充/删除某条？
> 3. 总评是否需要调整？

**评论合并检查（两种模式都必做）**：投递前**主动识别主题重叠的评论**：

- 两条评论指向**同一根本问题**（如 #A 讲"全量推送影响面"、#B 讲"每次 dial gRPC 性能"）→ 合并成 1 条更聚焦
- 两条评论定位**同一行/相邻行**且方向一致 → 合并（避免作者收到"评论轰炸"）
- 若合并会导致单条 > 200 字符 → 保持分开，或用列表结构精简

- **模式 A**：以提议形式征询用户 —— "#A 和 #B 都指向「XXX 主题」，建议合并成 1 条更聚焦。你希望：合并 / 分开投？"
- **模式 B**：**自动合并**同主题/同锚点评论，无需询问。

**模式 A 等用户明确说"投递" / "OK" / "可以发" 才进入 Step 7**。

### Step 7: 投递评论

> **模式 A**：按用户确认范围投递。
> **模式 B**：直接投递 🔴/🟠（🟡 合进总评一句话），每条评论**必带指纹注释**（见 Step 2.1）用于下轮去重。
>
> ⚠️ **两种模式都必须投「总评 + 行内评论」两部分**（模式 B 尤其容易漏投总评）：
> - **总评**（`is_person_note=true`）：1 条，给整体结论（通过/需修改/不通过 或 design 的可行性 + 是否有更优方案），风险等级 = 最严重条目。
> - **行内评论**（`is_person_note=false`）：逐条问题落到具体文件行。
> - **即使只有 1 条行内评论，也要单独发一条总评**；即使全部问题都能行内定位，也不能省略总评。

调用 `create_merge_request_note`，遵守以下**投递规范**：

| 字段 | 默认值 | 说明 |
|------|------|------|
| `path` + `line` + `line_type=new` | 必填 | 行内评论。新增文件用 `new`；修改文件按 diff 判断 |
| `risk` | 🔴=3 / 🟠=2 / 🟡=1 / 总评=2 或 3 | 严重程度；总评风险等级跟随最严重条目 |
| `resolve_state` | `1`（unresolved） | 让作者可标记解决 |
| `notify_enabled` | `true` | **必须通知作者** |
| `is_person_note` | 总评 `true`、行内 `false` | 总评进 comments tab，行内进 conversation tab |

**评论内容规范**（极其重要）：

- ✅ **简明扼要**，开头一句话直接点问题，紧跟一句话给方向。
- ✅ 用 emoji 标风险：🔴 🟠 🟡。
- ✅ 必要时贴**原文一行**（用 backtick 包），不超过 30 字。
- ✅ 引用真实代码位置时**必带文件路径 + 行号**（如 `instruction.go` L379），让作者能一键跳转。
- ❌ **不要长篇大论**、不要复述 spec 原文、不要补充背景知识。
- ❌ 不要写"建议讨论"、"可能需要"等模糊措辞，给出**具体的修改方向**。

**行号定位策略**：

- 优先定位到**问题最具体的那一行**（如 FR-6.2 的某条具体描述）。
- 若问题跨章节 / 章节缺失 → 定位到**最相关的章节标题行**。
- 完全无法定位 → 用**总评**形式（不带 path/line）放 conversation tab。

**投递顺序**：

1. **先发总评**（is_person_note=true，1 条，**必发**）——**总评风险等级 = 最严重条目的等级**（如有 🔴 则总评就是 🔴）；design MR 的总评要含"方案整体可行性 + 是否有更优方案"的结论。
2. 再按文件 + 行号顺序发**行内评论**（is_person_note=false，并行批量发，提升效率）。
3. ⚠️ **自检：投递完确认「总评」和「行内评论」两类都已成功创建**（模式 B 无人把关，尤其要在日志里确认总评已投）。

**指纹（loop 去重必带）**：每条投递的评论**结尾附**稳定指纹注释，供下一轮复评识别自己投过的问题：

```
<!-- mr-spec-review:{指纹串} -->
```

- 指纹串 = `文件锚点(章节/符号，非行号) + 问题类别` 的短哈希，push 后行号漂移也不影响识别。
- **模式 B 强制加**；模式 A 建议加。

**分批投递策略（可选，适用于"两轮评审 + 大量问题"场景）**：

如果第一轮和第二轮加起来 > 15 条，考虑征询用户后**分两批**投：

- **批 1（先投）**：文档层 🔴/🟠 —— 让作者先修文档
- **批 2（作者修完后再投）**：代码验证发现的方案层问题 —— 避免作者一次收到 20+ 条淹没重点

默认还是**一次投完**，除非用户明确要分批。

### Step 8: 收尾汇报 + 复评记忆沉淀

#### 8.1 收尾汇报

- **模式 A**：向用户报告。
- **模式 B**：写 CI/agent 日志，并可在总评末尾追加一行"本轮共投 N 条，其中 X 条为新增、Y 条沿用历史未解决"。

```
✅ 已投递 {N} 条评论到 MR !{iid}（第 {初评/N 轮复评}）
  - 总评 1 条
  - 行内 {N-1} 条（🔴{X} 🟠{Y}）
  - 去重跳过 {重复指纹数} 条
  - 通知已发送给 @{author}

未投递 {M} 条：
  - [被裁剪/降级为总评的问题简述]
```

#### 8.2 复评记忆沉淀（**eval 门禁驱动的自净化闭环**）

> 🎯 目的：作者**接受并解决**过的问题，是被验证过的"真问题模式"。但"作者改了"≠"可泛化的好规则"——因此沉淀不是无脑追加，而是走 **Eval 门禁闭环**：**候选入库前先自我评测（回测准确率/泛化/冲突/锚点），入库后按准确率、误报率、提升效果定期净化淘汰**，让 skill **越用越准而不是越用越噪**。
>
> 📖 **完整机制、指标定义、量化门槛、执行清单见 [`memory/EVAL-SPEC.md`](./memory/EVAL-SPEC.md)**。本节是其在评审流程里的落点摘要。
>
> ⏳ **加载时机（省时关键）**：`EVAL-SPEC.md`（约 450 行）**不在每轮评审前加载**——它只服务于"记忆沉淀/净化"。仅当**本轮为复评/merge 且确有 TP 候选**（作者已 `resolved` 且 commit 真改了）需要走入库门禁时，才**按需读取** `EVAL-SPEC.md`。**初评轮不加载**（初评不沉淀，只暂存候选待复评确认）。
>
> **触发条件**：本轮为复评（Step 2.1 判定）识别出**新变为 `resolved` 的评论**（作者本轮接受并改好）→ 进入沉淀候选；MR **merge 事件**额外触发一次增量净化 + 全量沉淀。

**闭环三步（照 EVAL-SPEC §10 执行）**：

**① 喂语料库（每轮收尾都做，EVAL-SPEC §10.1）**
- 把本轮所投评论 + 指纹 + 当前处置追加到 [`memory/eval-corpus/`](./memory/eval-corpus/)。
- 复评轮更新上一轮记录的 label：`unresolved→resolved`（**且核对 commit 真的改了对应处**）= **TP**；`unresolved→作者反驳/wontfix` = **FP**。

**② 入库门禁（沉淀候选时，EVAL-SPEC §5）**
1. **筛候选**：本轮新 TP → 候选；被拒 FP → **负样本**（不丢，用于淘汰）。
   - ✅ 值得沉淀（TP）："application 层硬编码设备类型判断，未复用 domain hook"（通用反模式）
   - ❌ 不值得（一次性笔误）："第 5 行拼写把 example 写成 exammple"
2. **归一化四元组 + 去硬编码**：写成 `trigger / probe / assertion / fix`，把来源 MR 的具体符号抽象成变量占位（否则规则只对来源 MR 有效）。
3. **过自我评测门禁**（在 eval 语料库上跑）：
   - **回测**：Precision = TP/(TP+FP) ≥ 0.8？（样本 <3 条 → 判"数据不足"）
   - **泛化**：TP 命中的独立 `(MR×文件锚点)` ≥ 2？（仅来源 MR 单点 → 不足）
   - **冲突**：与现有 `active` Pattern + `review-guides` 人工注意点无直接矛盾、非语义重复？
   - **锚点**：probe 引用的代码位置在当前基线 `grep` 真实存在？
4. **判状态并落盘**（EVAL-SPEC §8 格式，含"状态+统计"两行）：
   - 全达标 → `active`（写入对应 `code-patterns.md`/`doc-process-patterns.md`）
   - 数据不足/泛化不够 → `shadow`（试用，投递时降级）
   - 不达标/严重冲突 → `rejected`（记 [`memory/attic.md`](./memory/attic.md) 拒收记录，不进正式库）
   - 语义重复 → **合并**到已有 Pattern（累加来源 MR + 统计，不新增编号）
5. **回灌 FP**：本轮被拒评论对应规则的统计 FP+1、重算 Precision → 触发 ③ 的误报判定。

**③ 净化淘汰（merge 增量 / 每周全量，EVAL-SPEC §7）**
- 对每条 `active`/`shadow` 规则重算滚动窗口统计，依次体检：
  - **误报淘汰**：Precision∈[0.4,0.6) 降 `shadow`；<0.4 或 FP≥3 且 Precision<0.5 → `retired`。
  - **过期淘汰**：probe 锚点在基线 grep 失效 → `stale`（模式 B 尝试自动重锚为语义锚点；无法重锚 → `retired`）。
  - **冲突淘汰**：矛盾对保 Precision 高者，落败者降 `shadow`。
  - **僵尸归档**：连续 10 次评审 0 命中 或 >120 天未命中 → `dormant` → 归档。
- 淘汰项移 `attic.md`（附理由，可恢复）；满足升级条件（distinct≥3 + Precision≥0.85 + uplift 正向）→ 提议升级 `rules/`。

**落盘模式差异**：
- **模式 A**：门禁结论 + 各 metric 展示给用户，认可后写入本地工作副本。写入后可**随时用 Step 9 一键收口**（提交 memory 改动到记忆分支 + 发起/更新到 master 的 MR），无需手工拼 git 命令；也可走平时 spec MR 流程。
- **模式 B（headless/CI）**：按门禁自动判定后，**通过Git 平台（GitHub / GitLab / 工蜂） MCP 写 API 把变更提交到专用记忆分支**（如 `chore/mr-review-memory`），**不改共享工作副本、不切分支**——否则会被 Step 0 的 `reset` 冲掉、且无法反哺其他会话（详见 EVAL-SPEC §9.5）。日志输出净化报告。

**记录闭环结果**（附收尾汇报，格式见 EVAL-SPEC §10.3）：
```
🧪 规则闭环报告
  入库：新增 active {a}、shadow {s}、拒收 {r}、合并 {m}
  淘汰：{Pattern#M → retired（FP {fp}/Precision {p}）}
  过期：{Pattern#K → stale（锚点 `xxx` 失效，{自动重锚/待人工}）}
  休眠：{Pattern#J → dormant（连续 {n} 次 0 命中）}
  升级：{Pattern#Z → 提议写入 rules/}
  当前库存：active {A} · shadow {S} · attic {D}
```

> ⚠️ **纪律**：① 只把 **TP（作者已接受并 resolved + commit 真改了）** 作为沉淀候选；② **FP（被拒/无效）不丢弃**，必须回灌为误报统计驱动淘汰——这是旧流程最大缺口；③ 评审加载时 `active` 正常投递、`shadow` 降级为 `[试验性]`/总评汇总、`stale`/`dormant` 不加载（见 EVAL-SPEC §10.4）。

### Step 9: 记忆经验提交（**手动触发 · 把本地/记忆分支的 memory 改动收口成 MR**）

> 🎯 **用途**：Step 8.2 的自净化闭环会在本地工作副本（模式 A）或记忆分支（模式 B）里**累积** `memory/` 改动，但这些改动最终要**合并回 master** 才能真正反哺团队。本步是一个**独立可单独触发**的收口功能：不做评审，只负责把待提交的 memory 经验改动**提交 → 推送 → 发起/更新合并到 master 的 MR**。

> 🔔 **触发方式（用户显式发起）**：用户说 **"提交 spec review 的 memory" / "推送评审经验" / "把 memory 改动提交上去" / "同步记忆库到 master"** 等 → **直接进入本步，不走 Step 1~8 的评审流程**（没有 MR 地址也能跑）。

#### 9.1 定位 CoSpec 仓（复用 Step 0.1）

- 按 **Step 0.1** 在当前工作区定位 `$COSPEC_ROOT`（`git remote` 指向 `<group>/CoSpec` 的目录）。
- 定位不到 → 停下报告"当前工作区未定位到 CoSpec 仓，请提供路径或先 clone"，**不在非 CoSpec 目录上乱跑**。
- ⚠️ **Knot 部署歧义**：当前目录若是 Knot 打包副本（非 CoSpec 的 git checkout）→ 继续探测真正的 CoSpec 仓；**绝不把 memory 提交到 Knot 打包副本**（下次发布即冲掉，见 Step 0.3）。

#### 9.2 检测是否有待提交的 memory 改动

**扫描范围严格限定在** `$COSPEC_ROOT/skills/mr-spec-review/memory/`（`code-patterns.md`、`doc-process-patterns.md`、`eval-corpus/`、`attic.md`、`README.md`、`EVAL-SPEC.md`）。

- **模式 A（本地手动，有工作副本）**：
  ```bash
  # 只看 memory 目录的改动（含未追踪的新文件）
  git -C "$COSPEC_ROOT" status --porcelain -- skills/mr-spec-review/memory/
  git -C "$COSPEC_ROOT" diff --stat -- skills/mr-spec-review/memory/
  ```
  - **无输出** → 报告"memory 无待提交改动，无需提交"，**结束**（不要空提交、不要开空 MR）。
  - **有输出** → 列出改动清单（哪些 Pattern 文件 / eval-corpus 变了、新增几条），进入 9.3。
- **模式 B（headless / 记忆分支已累积）**：改动在记忆分支上（经 MCP 写 API 累积）→ 通过 `get_repository_tree` / `compare` 比较 `chore/mr-review-memory` 与 `master` 的 `skills/mr-spec-review/memory/` 差异，判定是否有待合并内容。

> ⚠️ **只提交 memory/，不夹带其它改动**：`git add` 时**只加 `skills/mr-spec-review/memory/` 下的文件**，绝不 `git add .`（避免把评审过程中拉取的 src 改动、其它 spec 草稿等误提交）。

#### 9.3 提交并推送到记忆分支

**固定使用专用记忆分支 `chore/mr-review-memory`**（与被评审 MR 的 feature 分支隔离，见 EVAL-SPEC §9.5）。

- **模式 A（本地手动 · 独占工作区）**：
  1. **并发安全前置**：确认当前工作区**不是**多会话共享的评审工作区（共享区禁止在此切分支，见 §并发安全原则）。若是共享/并发区 → 改用模式 B 的 MCP 写 API 路径。
  2. 切到/新建记忆分支（**只在独占工作区做**）。⚠️ **先显式探测远端记忆分支是否存在，再决定基点**，绝不静默 fallback 到 master 用 `-B` 覆盖本地已有指针（否则 fetch 抖动就可能丢未推送的记忆 commit）：
     ```bash
     git -C "$COSPEC_ROOT" fetch origin
     if git -C "$COSPEC_ROOT" ls-remote --exit-code origin chore/mr-review-memory >/dev/null 2>&1; then
       # 远端已有记忆分支 → 必须基于它（保留历史沉淀）
       git -C "$COSPEC_ROOT" checkout -B chore/mr-review-memory origin/chore/mr-review-memory
     else
       # 远端确无该分支 → 才允许基于 master 全新创建
       git -C "$COSPEC_ROOT" checkout -B chore/mr-review-memory origin/master
     fi
     # 把之前停在其它分支时产生的 memory 改动带过来（如原本在 feature 分支产生）——
     # 若改动已在工作区未提交状态，checkout -B 会保留；如已在别的分支提交，则 cherry-pick 对应 commit
     ```
     > ⚠️ 若 `ls-remote` 因网络抖动等原因**报错**（既非明确存在也非明确不存在）→ **停下报告，不要静默降级到 master**，避免误覆盖已有记忆分支。
     > 若 memory 改动此前是在别的分支上**已提交**，用 `git cherry-pick <commit>` 摘到记忆分支；若还在工作区**未提交**，切分支时会自动带过来。
  3. 只暂存 memory 目录并提交（commit message 说明本次沉淀/净化摘要）：
     ```bash
     git -C "$COSPEC_ROOT" add skills/mr-spec-review/memory/
     git -C "$COSPEC_ROOT" commit -m "chore(mr-review-memory): 沉淀评审经验 <一句话摘要>"
     ```
     - commit message 摘要用 Step 8.2 的规则闭环报告要点（新增 active X / shadow Y / 淘汰 Z 等），无报告则据 diff 概括。
  4. **push 前先安全 rebase master（关键 · 保证Git 平台（GitHub / GitLab / 工蜂） MR 可安全 rebase merge）**：commit 完成后**先不要直接 push**，按团队 git 工作流把 master 同步到最新、再让记忆分支 rebase 到 master。⚠️ **进入本步前必须已完成步骤3 的 commit、工作区干净**——否则 `checkout master` 会把未提交改动带到 master 工作区或因冲突阻塞，故先做前置校验：
     ```bash
     # ⓪ 前置：确保工作区干净（步骤3 已 commit），否则停下
     git -C "$COSPEC_ROOT" diff --quiet && git -C "$COSPEC_ROOT" diff --cached --quiet \
       || { echo "工作区仍有未提交改动，请先完成步骤3 commit"; exit 1; }
     # ① 切到 master 同步最新
     git -C "$COSPEC_ROOT" checkout master
     git -C "$COSPEC_ROOT" pull -r origin master
     # ② 切回记忆分支，rebase 到最新 master
     git -C "$COSPEC_ROOT" checkout chore/mr-review-memory
     git -C "$COSPEC_ROOT" rebase master
     ```
     - **rebase 冲突处理**：memory 是纯文档，冲突通常是同一 Pattern 文件被多方追加 → 手工合并保留双方条目（不要丢别人的沉淀），`git add` 后 `git rebase --continue`；实在理不清则 `git rebase --abort` 停下报告，交用户裁决，**绝不 `--skip` 丢改动**。
     - 目的：让记忆分支线性领先于 master，Git 平台（GitHub / GitLab / 工蜂）上的 MR 就能走 **rebase merge**（快进、无 merge commit），避免落后 master 太多导致合并冲突。
  5. **push 到记忆分支**：据步骤2 `ls-remote` 的探测结果决定 push 策略——远端已有记忆分支（rebase 改写了历史）→ `--force-with-lease` 安全强推；远端全新分支 → 普通 `push -u`：
     ```bash
     if git -C "$COSPEC_ROOT" ls-remote --exit-code origin chore/mr-review-memory >/dev/null 2>&1; then
       # 远端已存在 → rebase 改写历史，用 force-with-lease 安全强推
       git -C "$COSPEC_ROOT" push --force-with-lease -u origin chore/mr-review-memory
     else
       # 远端首次创建 → 普通推送，避免对全新分支强推
       git -C "$COSPEC_ROOT" push -u origin chore/mr-review-memory
     fi
     ```
     > ⚠️ 用 `--force-with-lease`（不是 `--force`）：rebase 改写了历史必须强推，但 `--force-with-lease` 会在远端分支已被别人更新时拒绝推送，避免覆盖他人提交。
- **模式 B（headless / CI）**：**不切本地分支**，改动已由 §9.5 的 MCP 写 API（`create_or_update_file` / `batch_modify_files`）提交到 `chore/mr-review-memory`；本步只需确认已推送，直接进 9.4。

#### 9.4 发起 / 更新合并到 master 的 MR

**推荐"一条长期存在的记忆 MR 持续累积"**（EVAL-SPEC §9.5 模式 B）：

1. 先查是否已有 `chore/mr-review-memory → master` 的开启中 MR：
   ```
   search_merge_request(project_id=<CoSpec>, source_branch=chore/mr-review-memory, target_branch=master, state=opened)
   ```
2. **已存在** → 本次 push 的 commit 自动进入该 MR，**无需新建**；在对话/日志里给出该 MR 链接，提示"记忆改动已追加，待人工 review + merge"。
3. **不存在** → 新建 MR：
   ```
   create_merge_request(
     project_id=<CoSpec>,
     source_branch="chore/mr-review-memory",
     target_branch="master",
     title="chore: mr-spec-review 记忆经验沉淀",
     description=<本次沉淀/净化摘要 + 规则闭环报告>
   )
   ```
   - 目标分支固定 `master`（CoSpec 仓基线）。
   - description 附 Step 8.2 的规则闭环报告，便于人工 review 时快速判断。

> ✅ **人工闸门**：MR **不自动 merge**——给规则库演进留一道人工把关（review 后再 merge）。除非用户明确说"直接合并"，才在权限允许下调 `merge_merge_request`。

#### 9.5 收尾汇报

```
✅ 记忆经验已提交
  - CoSpec: $COSPEC_ROOT
  - 分支: chore/mr-review-memory（已 push）
  - 改动: memory/code-patterns.md(+X) · doc-process-patterns.md(+Y) · eval-corpus(+Z)
  - MR: <链接>（新建 / 已存在并追加 commit）
  - 后续: 人工 review + merge 到 master 即完成反哺
```

- 无改动时：`ℹ️ memory 无待提交改动，未创建提交/ MR`。

**安全与纪律（本步必守）**：
- 🔒 **token env-only**：push / MCP 写 API 用的 `GONGFENG_TOKEN` 只从环境注入、掩码，绝不写进任何仓库文件或 remote URL（遵守 @security_rules）。
- 🔒 **只碰 memory/**：提交范围严格限定 `skills/mr-spec-review/memory/`，不夹带评审过程产生的 src / 其它改动。
- 🔒 **并发安全**：共享/并发工作区**不切本地分支**，走 MCP 写 API 路径；仅独占工作区才允许本地 `checkout -B` 记忆分支。
- 🔄 **push 前必 rebase master**（模式 A）：commit 后先 `master` `pull -r` 同步最新 → 记忆分支 `rebase master` → 再 `push --force-with-lease`，保证Git 平台（GitHub / GitLab / 工蜂） MR 能安全 rebase merge；rebase 冲突手工合并保留双方沉淀，理不清则 `--abort` 交用户裁决，绝不 `--skip` 丢改动。
- 🚦 **不自动 merge**：默认保留人工 review + merge 闸门，除非用户显式要求直接合并。


> 集成方式：**不在 CoSpec 仓内放 CI 配置**。由一条**外部流水线**监听 CoSpec 仓的 **MR push 事件**与 **MR 合并事件**（webhook），收到事件后取 MR 信息 → 调用 AI agent → 触发本 skill（模式 B）。因评审全程走Git 平台（GitHub / GitLab / 工蜂） MCP 只读（不切分支），**多个 MR 可安全并发**。

```
                    ┌─ push 事件 ──→ 取 MR 信息 ─→ 调 AI agent ─→ 跑 skill（评审 + 直接投评论）
CoSpec MR 事件 ──→ ┤                                                    ↑ 作者按评论改完再 push，loop
  (webhook)         └─ merge 事件 ─→ 取 MR 信息 ─→ 调 AI agent ─→ 跑 skill（Step 8.2 沉淀已解决项到记忆库）
```

### 1. 事件与职责

| 事件 | 触发目的 | skill 行为 |
|------|---------|-----------|
| **MR push**（分支有新提交） | 驱动评审 loop | 初评/复评（Step 2 指纹去重）→ 直接投递 🔴/🟠 评论 → 作者改完再 push，直到无问题 |
| **MR merge**（合并完成） | 收尾沉淀经验 + 净化规则库 | 走 Step 8.2 eval 门禁闭环：把已 `resolved` 的 TP 过入库门禁沉淀、被拒 FP 回灌统计、并跑一次规则净化淘汰（EVAL-SPEC §7），让规则库越用越准 |

### 2. 外部流水线要做的事（伪代码）

流水线自身在 CoSpec 仓**之外**（独立的事件网关 / CI 项目），大致逻辑：

```bash
# 1) 从 webhook payload 解析 MR 上下文
PROJECT_PATH=<payload.project.path_with_namespace>   # 如 <ORG>/CoSpec
MR_IID=<payload.object_attributes.iid>
EVENT=<payload.object_attributes.action>             # open/update(push) 或 merge
MR_URL="<GIT-HOST>/${PROJECT_PATH}/-/merge_requests/${MR_IID}"

# 2) 准备工作区：确保有一份 CoSpec 仓 + src 业务仓（skill 的 Step 0 会再校验/同步）
#    src 业务仓的同步交给 skill 内置脚本，无需流水线重复实现

# 3) 调用 AI agent 触发 skill（占位命令，替换成你们实际的 agent CLI）
codebuddy run \
  --skill mr-spec-review \
  --non-interactive \
  --input "评审这个 MR：${MR_URL}（事件：${EVENT}，模式 B）"
```

> **模式识别**：调用方无人交互 → skill 自动判定为**模式 B**（见「执行环境与模式」自动识别第 2/3 条），跳过确认直接投递。

### 3. 业务代码环境准备（由 skill 负责，非流水线）

skill 在 Step 0.2 运行内置脚本 [`scripts/prepare-src.sh`](./scripts/prepare-src.sh)：

- 含**完整业务仓清单**（`目录 ↔ 基线分支 ↔ Git 平台（GitHub / GitLab / 工蜂）地址`）；识别算法仓 `algorithm-repo`（来自 <ORG>/<算法组>）平铺在 `src/algorithm-repo`；原 `<业务主仓库>`/`proto` 逻辑已合并进 <ORG>/<子组> 的 `src/<业务主仓库>`/`src/proto`。
- 缺失的仓自动 HTTPS clone（需 `GONGFENG_TOKEN`）；已存在的 fetch 后**停在基线 `reset --hard`**（幂等、并发安全）。
- best-effort：单仓失败不中断，agent 在总评注明该仓代码验证可能不完整。

### 4. 密钥与权限（env-only，遵守 @security_rules）

| 变量 | 用途 | 配置位置 |
|------|------|---------|
| `GONGFENG_TOKEN` | clone 私有业务仓 + 通过Git 平台（GitHub / GitLab / 工蜂） MCP **投递评论** + **回写记忆到专用分支/MR**（需对 CoSpec 仓有 write 权限） | 流水线/agent 运行环境的 secret，**掩码存储** |
| AI 模型凭证 | agent 调用大模型 | 同上，掩码 |

- ❌ 绝不把 token 写进任何仓库文件；只从运行环境的 masked secret 注入。脚本用 `http.extraHeader` 注入，不写进 remote URL / `.git/config`。
- MCP 配置里Git 平台（GitHub / GitLab / 工蜂） server 的鉴权同样读环境变量，不落盘明文。

### 5. loop 行为与可选门禁

- **loop**：作者收到评论 → 本地改 → 再 push → push 事件再次触发评审。靠 **Step 2 指纹去重**只评增量、不重复投；直到无 🔴/🟠。
- **记忆闭环**：MR 合并事件触发 **Step 8.2**（eval 门禁闭环）：TP 过门禁沉淀、FP 回灌统计、并净化淘汰过期/冲突/僵尸规则（EVAL-SPEC §7）。
- **记忆持久化（headless 关键）**：agent 产生的记忆变更**不改共享工作副本**，而是通过Git 平台（GitHub / GitLab / 工蜂） MCP 写 API（`get_blob_content` 读最新 → `create_or_update_file`/`batch_modify_files` 提交）落到**专用记忆分支** `chore/mr-review-memory`；**推荐做成一条长期存在的 memory-curation MR 自动累积 commit**，人工侧**只需定期 review + merge 这条 MR**，无需登录 agent 手动提交。写入时机分离：每次 push 只 append `eval-corpus`，merge 事件才改 `memory/*.md`（降并发）。详见 EVAL-SPEC §9.5。
- **可选门禁**：若希望"有 🔴 就卡住合并"，让 agent 发现 🔴 时返回**非 0 退出码**，配合Git 平台（GitHub / GitLab / 工蜂）"流水线通过才可合并"规则。
- **防抖**：作者连续 push 时，让流水线**取消同一 MR 尚未跑完的旧评审任务**，避免浪费算力。

---

## 常见坑与对策

| 坑 | 对策 |
|----|------|
| **skill 跑在非 CoSpec 目录** | Step 0.1 先在工作区定位 `$COSPEC_ROOT`；定位不到就停下报告，别在错目录乱跑 |
| **评审不分端、用一套通用维度一锅烩** | Step 4.0 先识别端（按 `review-guides/README.md` 路由表）→ 加载对应端 guide（含人工注意点）→ 按端评审、按端分组报告 |
| **漏加载某端的专业依据或人工注意点** | Step 4.0 命中的每个端都要读完对应 `review-guides/*.md` 全文（含 `HUMAN-CURATED` 专区）；涉及跨端字段/接口再加 `contract.md` |
| **intake/design-only MR 被误判为"非 spec 类"拒评** | Step 1.4 明确：只含 intake+design 也照常评审，走 Step 4.6 结合代码评方案 |
| **design 只挑文档格式、不评方案好坏** | Step 4.6 必做"结合代码验证合理性 + 整体建议（更优方案/细节调整）"，不能只看模板合规 |
| **模式 B 只投行内漏投总评（或反之）** | Step 7 强制两类都投：先发总评（is_person_note=true）再发行内，投递后自检两类都成功 |
| **业务代码仓过时导致误判** | 每次评审跑 `scripts/prepare-src.sh`（默认按端按需同步命中仓，见 Step 1.6；全量为兜底）：相关 `src/` 仓 `fetch + reset --hard origin/<baseline>` 停在基线 |
| **强制同步覆盖本地改动** | 模式 A（独占工作区）先查有无未提交改动，有则先问用户；模式 B 才直接 reset --hard |
| **复评重复投同一问题** | Step 2 判初评/复评 + 评论带指纹注释；同指纹已投则本轮跳过 |
| **CI 模式还在等人确认** | 模式 B 跳过 Step 6，直接投递 🔴/🟠；🟡 合进总评 |
| **记忆库越沉越膨胀/重复** | Step 8.2 走 eval 门禁：入库前回测+泛化+冲突+锚点校验（EVAL-SPEC §5），入库后按准确率/误报率/活性净化淘汰（§7），`dormant` 不加载；重复的合并不新增 |
| **把误报沉淀进记忆** | 只沉淀 TP（作者已接受 resolved + commit 真改了）；被拒 FP 不沉淀，且**回灌为误报统计**驱动坏规则淘汰 |
| **规则过期误导（代码已重构）** | probe 用语义锚点而非写死符号；净化扫描 grep 校验锚点，失效即 `stale` 暂停投递、尝试重锚（EVAL-SPEC §7.3） |
| **规则互相矛盾** | 入库冲突关（§5.3）+ 净化冲突体检（§7.4）：保 Precision 高者、汰落败者 |
| `merge_request_id` 与 `iid` 混淆 | MCP 工具大多要 `merge_request_id`（不是 iid），先 `search_merge_request` 拿真正的 id |
| 新增文件的行号 | diff 中 `+` 开头的行号即新文件行号，`line_type` 必须用 `"new"` |
| 行号定位漂移 | 用最新 `head_commit_sha`，每次评审前重拉 `get_merge_request_changes` |
| spec 仓库 vs 代码仓库混淆 | spec 在 CoSpec 仓；代码在业务仓（<前端仓库>、<业务主仓库> 等）。MR 只在 spec 仓 |
| 跨仓库代码 clone 失败 | 不要硬撑，直接告诉用户哪个仓库 clone 失败、让用户处理 |
| 业务代码仓基线落后于 origin | Step 0.2 跑 `scripts/prepare-src.sh` 把 src 仓停在基线并同步；完整基线映射以该脚本为准 |
| 基线分支搞错 | 不要脑补 master/develop：以 `scripts/prepare-src.sh` 的 `REPOS` 映射为准（如 <ORG> 系均为 `develop`、`algorithm-repo` 是 `master`）；不在表内的查 `docs/git-workflow.md` 或问用户 |
| 本地 spec 仓忘记加载 MR 版本 | Step 1.5 必做，但**不再切分支**：用 `get_blob_content` 按 `source_commit` 只读拉全文，避免误读到 master 版本 |
| **共享工作区多会话切分支互踩** | 并发/CI 下 CoSpec 与 src 仓都只停基线、只读；读某分支文件走 MCP `get_blob_content`；大范围 grep 用会话独占 worktree（唯一路径+detached） |
| 本地分支落后于 MR head | 无需本地对齐——`get_blob_content` 直接按 `source_commit` 拉，天然与 diff 一致 |
| 历史评论重复 | Step 2 必拉历史评论，不要重发同一问题 |
| 评论太长 | 强制自检：单条评论 body ≤ 200 字符为佳，超过则砍 |
| 作者还在改 | 看最后 commit 时间，若 < 30 分钟先和用户确认是否继续（模式 B 可放宽：以 CI 触发的 commit 为准） |
| 只看 diff 不翻代码 | 第二轮代码验证（4.5）必做；spec 引用的每个代码位置都要真开文件/只读拉取确认 |
| 忽视兄弟 spec | 同 Story 拆分的多个 spec 之间 proto/常量/错误码必须与源对齐；`Sibling Specs` 未列即报 🔴 |
| 名字大小写自动脑补 | `xxx` vs `Xxx` vs `XxxAppAppId` 必须 grep 验证真实拼写，不能靠"看起来对" |
| 主题重叠评论轰炸 | 投递前主动识别重叠条目提议合并（模式 B 自动合并），避免作者收到 3 条讲同一件事的评论 |
| 收尾漏清理 | 默认只读方式无需清理；仅用了会话独占 worktree 时 → `git worktree remove <唯一路径>` |

---

## 输出风险分级标准

| 级别 | 适用情形 | risk |
|------|---------|------|
| 🔴 严重 | 安全漏洞 / 破坏性变更 / 设备端不兼容 / 编译可能 break / 逻辑错误导致功能不可用 | 3 |
| 🟠 一般 | 边界遗漏 / 性能隐患 / 与现状字段不一致 / 缺少灰度方案 | 2 |
| 🟡 轻微 | i18n 留白 / 命名小问题 / 风格不一致 / 缺测试要求 | 1 |

---

## 关联资产

- **依赖**：Git 平台（GitHub / GitLab / 工蜂） MCP
  - 评审上下文：`search_merge_request` / `get_merge_request_changes` / `search_merge_request_notes` / `get_commits_list`
  - **只读取文件（并发安全，替代切分支）**：`get_blob_content`（按 ref 取文件全文）/ `get_repository_tree`（按 ref 列目录）
  - 投递：`create_merge_request_note`
  - **记忆提交（Step 9）**：`create_or_update_file` / `batch_modify_files`（写记忆分支）/ `search_merge_request`（查记忆 MR）/ `create_merge_request`（发起记忆 MR）/ `compare`（比对记忆分支与 master）
- **关联规则**：
  - `rules/10-spec-workflow.md`（spec 工作流）
  - `rules/20-coding-rules.md`（代码规范）
  - `rules/30-testing-rules.md`（测试覆盖）
  - 项目安全规则（@security_rules）
- **分端评审依据**（Step 4.0 按端加载）：
  - [`review-guides/README.md`](./review-guides/README.md)（端路由表 + 人工追加规范）
  - [`review-guides/backend.md`](./review-guides/backend.md) / [`algorithm.md`](./review-guides/algorithm.md) / [`frontend.md`](./review-guides/frontend.md) / [`device.md`](./review-guides/device.md) / [`mobile-sdk.md`](./review-guides/mobile-sdk.md) / [`contract.md`](./review-guides/contract.md)
  - 每份 guide 底部含「🖊 人工追加评审注意点」专区，供各端专家维护
- **关联文档**：
  - `docs/git-workflow.md`（仓库地址 / 基线分支对照）
  - `specs/templates/spec-template.md`（spec 模板，对照检查完整性）
- **经验闭环（记忆库自净化）**：
  - [`memory/EVAL-SPEC.md`](./memory/EVAL-SPEC.md)（经验沉淀与反哺的 Eval Spec：提取/校验/沉淀门禁 + 净化淘汰 + 评测指标）
  - [`memory/README.md`](./memory/README.md)（记忆库使用说明 + Pattern 生命周期格式）
  - [`memory/code-patterns.md`](./memory/code-patterns.md) / [`memory/doc-process-patterns.md`](./memory/doc-process-patterns.md)（Pattern 库）
  - [`memory/eval-corpus/`](./memory/eval-corpus/)（回测数据集）/ [`memory/attic.md`](./memory/attic.md)（淘汰归档区）
- **关联 Skill**：
  - `spec-analysis`（本地分析单个 spec，不投递评论）
  - `code-review`（纯代码评审，Go 后端）
  - `change-summary`（评审报告中可复用其分级表）

## 与现有 `/spec-review` 命令的区别

| 维度 | `/spec-review` 命令 | `mr-spec-review` skill |
|------|---------------------|------------------------|
| 输入 | 本地 spec 路径 | Git 平台（GitHub / GitLab / 工蜂） MR 地址 |
| 对象 | **自己的实现** + 自己的 spec | **他人的 spec/plan/tasks**（可选含代码） |
| 输出 | 对话报告 | 对话报告 + **投递到 MR 评论** |
| 时机 | 实施完成后自评 | 团队互评 / 评审他人提交 |

---

## 经验记忆库（Pattern Memory）

> 长期沉淀的评审经验存放在独立的 `memory/` 目录下，与本 SKILL.md 解耦，便于随每次评审持续增长。

**目录结构**：

```
skills/mr-spec-review/
├── SKILL.md                        # 本文件（评审流程 & 规范）
├── review-guides/                  # ⭐ 分端评审依据（Step 4.0 按端加载；人工维护为主）
│   ├── README.md                   # 端路由表 + 人工追加规范
│   ├── backend.md                  # 后台服务端（Go/DDD）
│   ├── algorithm.md                # 识别·算法端
│   ├── frontend.md                 # 前端（Vue）
│   ├── device.md                   # 设备端（C++/Android 固件）
│   ├── mobile-sdk.md               # 移动端·接入 SDK
│   └── contract.md                 # 跨端协议契约（横切）
└── memory/                         # 历史真问题模式（eval 门禁驱动的自净化闭环）
    ├── README.md                   # 记忆库使用说明与维护规范
    ├── EVAL-SPEC.md                # 🧪 经验闭环评测规格：提取/校验/沉淀 + 净化淘汰机制与指标
    ├── code-patterns.md            # ⭐ 首要：业务/技术/代码层经验
    ├── doc-process-patterns.md     # 次要：文档/流程层经验
    ├── eval-corpus/                # 回测数据集：历史评论的结构化处置标注（TP/FP/FN）
    └── attic.md                    # 归档区：淘汰/拒收/休眠的 Pattern（可审计、可恢复）
```

> 📌 **review-guides vs memory**：`review-guides/` 是**各端稳定的评审基准 + 专家人工注意点**（"这个端该看什么"，人工维护）；`memory/` 是**评审 loop 中沉淀的历史真问题**（"踩过什么坑"，自动沉淀）。评审时两者都加载，互补。

**优先级与主题分层**：

| 文件 | 优先级 | 主题 | 何时读 |
|------|-------|------|--------|
| [`memory/code-patterns.md`](./memory/code-patterns.md) | ⭐ **首要** | 直接影响方案最终落地的**准确性与合理性**：代码引用、抽象、分层、协议一致、性能反模式、时效/安全/幂等等 | Step 4.5 代码验证评审前必读 |
| [`memory/doc-process-patterns.md`](./memory/doc-process-patterns.md) | 次要 | 影响协作效率与追溯性：模板合规、Status 一致、兄弟 spec 引用、配套注册等 | Step 4.1 文档层评审前必读 |

**AI 执行时的强制流程**：

1. 进入 Step 4.0，先按 `review-guides/README.md` 路由表**识别涉及端**，**读完每个命中端的 `review-guides/*.md` 全文**（含「🖊 人工追加评审注意点」专区；涉及跨端契约再加 `contract.md`）
2. 进入 Step 4.1 前，**必须读 `memory/doc-process-patterns.md` 全文**
3. 进入 Step 4.5 前，**必须读 `memory/code-patterns.md` 全文**
4. 逐条扫描当前 MR，命中即在评审报告中标注来源：`命中 code-patterns#N` / `命中 doc-patterns#DN` / `命中 <end>-guide#<小节或人工-编号>`；报告按端分组
5. 评审完成后走记忆沉淀 eval 门禁闭环 —— **执行落点与三步骤见 Step 8.2**，完整机制/量化阈值见 [`memory/EVAL-SPEC.md`](./memory/EVAL-SPEC.md)。要点：只沉淀 TP（过回测/泛化/冲突/锚点门禁才 `active`）、FP 不丢弃回灌统计驱动淘汰、模式 A 展示 metric 待确认 / 模式 B 自动写入。

**设计要点**：主题分层（code 首要 / doc 次要）+ 独立文件（避免淹没主流程）+ 可回溯（每 Pattern 带来源 MR + 统计）+ 可裁剪（跨项目复用 SKILL.md 主体）+ 自净化（eval 门禁只进好规则、自动清坏规则）。**维护规则见 [`memory/README.md`](./memory/README.md)，评测机制/量化门槛见 [`memory/EVAL-SPEC.md`](./memory/EVAL-SPEC.md)**。

---

## 评审铁律（速记 · 完整坑→对策见上方《常见坑与对策》，正文各 Step 为准）

1. **先定位再评**：Step 0 定位 `$COSPEC_ROOT` + 按端同步命中 src 基线（未定位到就停下报告，别在错目录跑）。
2. **先分端 + 先读记忆再评**：Step 4.0 按路由表识别端并读完命中端 `review-guides/*.md`（含人工注意点）；4.1 前读 `doc-process-patterns.md`、4.5 前读 `code-patterns.md`；命中标注来源、报告按端分组。
3. **必做两轮**：文档层(4.1~4.4) + 代码验证(4.5)；intake/design-only 走 4.6 结合代码评方案（可行性 / 更优方案 / 细节调整），不得只挑格式。设备端接口兼容性(4.3)为硬性必查。
4. **不臆测**：spec 引用的每处代码 / 行号 / 字段 / 命名必真开文件 grep 核对；仓库 / 分支不明问用户，不脑补。
5. **并发安全**：遵循 §并发安全原则——共享 CoSpec/src 仓只停基线、只读；读某 ref 走 MCP `get_blob_content`，大范围 grep 用会话独占 worktree；模式 A 同步前不静默 `reset --hard` 覆盖用户改动。
6. **复评去重**：Step 2 判轮次 + 指纹去重，不重复投同一问题；历史评审回顾在前、增量评审为主。
7. **投递齐全**：两模式都必投「总评 + 行内评论」两类（先总评后行内、投完自检）；评论 < 200 字、带路径行号 + 去重指纹、🔴 给可执行修复方向；投递前合并同主题评论（模式 B 自动合并）。
8. **模式纪律**：模式 A 未经确认不投评论 / 不写记忆并先展示报告供裁剪；模式 B 自动投递 + 自动沉淀。运行模式按 §执行环境与模式 自动识别。
9. **记忆自净化**：只沉淀 TP 且过入库门禁（EVAL-SPEC §5：回测/泛化/冲突/锚点四关）；FP 不丢弃、回灌统计驱动淘汰（§7）；过期/冲突/僵尸规则不以正常置信度投递。见 [`memory/EVAL-SPEC.md`](./memory/EVAL-SPEC.md)。
10. **收尾**：默认只读无需清理；用过独占 worktree 则 `git worktree remove <唯一路径>`；最后汇报未投递条目。
11. **记忆提交按需触发**：用户说"提交 spec review 的 memory"等 → 走 Step 9，只提交 `memory/` 改动到 `chore/mr-review-memory` 分支；模式 A commit 后 **push 前先 rebase master**（`master` pull -r → 记忆分支 rebase → `push --force-with-lease`）保证Git 平台（GitHub / GitLab / 工蜂） MR 可安全 rebase merge，再发起/更新到 master 的 MR；无改动不空提交、默认不自动 merge（留人工闸门）；token env-only、共享区不切分支走 MCP 写 API。
