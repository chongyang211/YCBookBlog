# Command: spec-draft

## 命令名称

`spec-draft`

## 作用

基于原始需求（`docs/intake/xxx.md` 文件 或 对话描述），起草一份符合 `specs/templates/spec-template.md` 格式的 spec 文档。这是工作流的**最起点**，所有后续阶段（plan / tasks / implement / test）都依赖这一步的产出。

> 💡 **可选上游**：如果需求较大、已先走过 `/spec-design` 产出技术方案，本命令应同时读取 `designs/<VERSION>/<STORYID>-<slug>-design.md`（应为 `approved` 状态），把方案中的关键技术决策、spec 拆分建议作为起草输入，使 spec 更准确。

## 输入参数

| 参数 | 必选 | 说明 |
|------|------|------|
| 原始需求来源 | 是 | `docs/intake/xxx.md` 文件路径 或 自然语言描述 |
| 技术方案 | 否 | `designs/<VERSION>/<STORYID>-<slug>-design.md`（如已走 `/spec-design`，应为 approved） |
| 编号 | 否 | Story ID；不指定则从 intake frontmatter 取，或主动询问 |
| Author | 否 | 默认 `[TBD]`，建议指定 |

**触发方式**：

```
请基于 docs/intake/v1.6.0/2026-06-09-payment-retry.md 执行 spec-draft
```

或：

```
请帮我把这个需求写成 spec：用户登录失败时要支持指数退避重试，最多 3 次……
```

或：

```
我有一个需求要做空中支付，docs/intake/ 下有原始描述，请执行 spec-draft 起草成 0003 spec
```

## 执行步骤

0. **前置校验 — Story ID（必填录入点）**

   `/spec-draft` 是 Story ID 的**录入点**，后续所有命令都从 spec 文件名沿用。这一步**强制**确定 Story ID，不允许跳过。

   **来源优先级**：
   1. 用户对话中明确提供（如 "story id 是 10088"）
   2. `docs/intake/xxx.md` frontmatter 的 `Story ID` 字段
   3. 以上都没有 → AI **必须主动询问**：
      > "请提供本需求的 Story ID（纯数字，例 10088）。如果暂无对应 story，回复 `0` 占位。"

   **校验规则**：
   - 必须为纯数字（含 `0`）
   - 不允许跳过此步骤直接进入 Step 1
   - **版本目录**：从 intake/design 路径解析 `<VERSION>`（`docs/intake/<VERSION>/…`）；spec 写入同名版本目录 `specs/<VERSION>/`，目录不存在则先 `mkdir -p` 再写入。如输入路径缺少版本目录段，主动询问归属版本（如 `v1.6.0`）

   ⚠️ **多 spec 共享同一 Story ID 是合法的**（一个大需求拆给多人 → 多个 spec 共用一个 Story ID，靠 slug 区分）。本 Step 不强制 Story ID 唯一。

0.5. **多 spec 检测**（当 Story ID 已存在时）

   - 扫描 `specs/` 目录，看是否已有相同 Story ID 的 spec 文件
   - 如果**有**，AI 必须主动询问用户：
     > "已检测到 Story 10088 下已存在以下 spec：
     >   - specs/v1.6.0/10088-gateway-changes.md（张三）
     >   - specs/v1.6.0/10088-controller-impl.md（李四）
     >
     > 本次是要：
     >   a) 新建一个**子 spec**（slug 区分，比如 `10088-example-channel.md`）
     >   b) 修改已有的某个 spec
     >   c) 这是独立任务，需更换 Story ID"
   - 用户选 a → 继续；slug 必须与已有 spec 不重复，AI 在起草时引用兄弟 spec
   - 用户选 b → 终止本次起草，提示用 `/spec-sync` 走偏离回流流程
   - 用户选 c → 回到 Step 0 重新询问 Story ID

1. **收集原始需求**
   - 优先读取 `docs/intake/xxx.md`
   - 如果用户直接描述，先复述确认理解
   - **如已走过 `/spec-design`**：同时读取 `designs/<VERSION>/<STORYID>-<slug>-design.md`，校验其 `Status` 为 `approved`（否则提示先完成方案评审），把「关键技术决策」「建议的 spec 拆分」纳入起草输入

2. **扫描上下文**
   - 扫描 `specs/<VERSION>/` 目录了解现有 spec
   - 检查是否有现有 spec 重叠 / 冲突 / 可复用
   - 扫相关 `src/` 代码识别现有约束

3. **列出澄清问题**（关键步骤，**不可跳过**）
   - 主动列出 2-5 个最关键的不清楚点
   - **等用户回答后**再进入起草

4. **起草 spec**
   - 调用 `skills/spec-drafting` 技能
   - 按模板章节填写，每个章节标注信息来源（📥 原始需求 / 🤖 AI 推断 / ❓ TBD）
   - 不臆造业务目标、验收标准、ROI、安全合规
   - 如本 spec 是 Story 下的子 spec，必须在 frontmatter `Sibling Specs` 字段列出兄弟 spec 路径

5. **写入文件**
   - 路径：`specs/<VERSION>/<STORYID>-<slug>.md`
     - `STORYID`：Step 0 已确定的 Story ID（**多个 spec 可共享**）
     - `<slug>`：kebab-case 小写描述（**同 Story 内必须唯一**，是真正的 spec 区分键）
     - 例：`specs/v1.6.0/10088-payment-retry.md`
     - 子 spec 例：`specs/v1.6.0/10088-gateway-changes.md`、`specs/v1.6.0/10088-controller-impl.md`
   - frontmatter 必须包含 `Story ID` 字段（与文件名一致）
   - `Status: draft`（**强制**，不可改）
   - `Created` / `Updated` 自动填当前日期

6. **输出起草报告**
   - 三类章节清单：来自原始需求 / AI 推断 / [TBD]
   - 澄清问题列表（已问的、还未答的）
   - 与现有 spec 的关系
   - 建议下一步

## 输出结果

- 一份磁盘文件：`specs/<VERSION>/<STORYID>-<slug>.md`（status: draft）
- 起草报告（chat 中输出）

## 后续动作

按顺序：

1. **人 review draft**：业务方 + 架构师对每个 🤖 AI 推断的章节确认；对 ❓ [TBD] 章节补全
2. **回答澄清问题**：把 step 3 列的问题逐条回答到 spec 中
3. **Status 改为 ready**：所有 [TBD] 解决、所有验收标准确定后
4. **执行 `/spec-plan`**：进入阶段二

## 适用时机

- ✅ 接到口头/邮件/IM 形式的需求
- ✅ `docs/intake/` 中已有原始需求草稿
- ✅ 既有需求需要拆成多个独立 spec
- ❌ 已有 spec 文件，仅需修改局部 → 直接编辑 spec
- ❌ 已有 spec 但需偏离回流 → 走 `/spec-sync`

## 关联规则与技能

- **规则**：`rules/10-spec-workflow.md`（**阶段零**）
- **技能**：`skills/spec-drafting/SKILL.md`
- **模板**：`specs/templates/spec-template.md`

## 与其他命令的衔接

```
原始需求 (docs/intake/)
  ↓
（需求大时可选）/spec-design → designs/<VERSION>/<STORYID>-<slug>-design.md (approved)
  ↓
/spec-draft        ← 你现在用的命令（如有 design 则一并读取）
  ↓
specs/<VERSION>/<STORYID>-<slug>.md (status: draft)
  ↓ [人 review，补 TBD，改 status: ready]
/spec-plan
  ↓
plans/<VERSION>/<STORYID>-<slug>-plan.md
  ↓
/spec-tasks
  ↓
... (后续流程)
```
