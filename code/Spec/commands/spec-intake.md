# Command: spec-intake

## 命令名称

`spec-intake`

## 作用

把**产品经理输出的需求文档**（或对话中的自然语言描述）快速整理成一份符合 `docs/intake/templates/intake-template.md` 格式的 **intake 草稿**。intake 是「原始需求草稿区」，用于辅助人理解本次需求的内容与边界，作为 `/spec-draft` 的标准输入。

> 💡 **可选命令**：是否使用视个人习惯而定。
> - 需求来自 PM 文档、内容较多/零散时，用它先结构化成 intake，再起草 spec；
> - 需求很简单、你习惯直接起草的，可**跳过本命令**，直接用 `/spec-draft` 描述需求。
>
> ⚠️ **intake 不是 spec**：它不进入 plan/tasks/implement 链路，只是给人理解、给 `/spec-draft` 当输入的草稿。AI 不在 intake 阶段臆造业务目标、验收标准等，未知项统一进「已知未知项」清单。

## 输入参数

| 参数 | 必选 | 说明 |
|------|------|------|
| 需求来源 | 是 | PM 需求文档路径（`.md`/`.docx` 等）或自然语言描述 |
| Story ID | 否 | 需求单号（如 `10088`）；尚未分配填 `0` 占位，分配后再重命名 |
| 版本 | 否 | 归档版本目录（如 `v1.6.0`）；缺失时 AI 主动询问 |
| slug | 否 | kebab-case 短描述，不指定则 AI 依需求标题生成 |
| 优先级 | 否 | 高 / 中 / 低，默认 `[TBD]` |

**触发方式**：

```
请基于 docs/产品需求-空中支付.md 执行 spec-intake，版本 v1.6.0
```

或：

```
帮我把这个需求整理成 intake：用户登录失败时要支持指数退避重试，最多 3 次……
```

或：

```
这是 PM 的需求文档（已粘贴在下），story id 10088，请执行 spec-intake 生成草稿
```

## 执行步骤

0. **前置校验 — 确认迭代版本目录（强制，不可跳过）**

   intake 落在哪个 `<VERSION>/` 目录，决定了后续 design / spec / plan / tasks **全程复用同一版本目录**——它是整条链路的归档锚点，因此**必须在生成文件前与用户确认清楚**，不允许 AI 擅自假定。

   - **必须主动向用户确认归属版本**：即使触发时已给版本，也要复述确认（如："本次 intake 将归档到 `v1.7.0`，后续 spec/plan/tasks 也会放在该版本目录下，确认吗？"）。
   - 用户未指定版本时**必须询问**，并列出当前可选版本目录供选择（如 `v1.7.0` / `v1.8.0` / `v1.9.0` / `v2.0.0`），**不要默认沿用上一个版本**。
   - 确认后：目标目录 `docs/intake/<VERSION>/` 不存在则先 `mkdir -p` 再写入。
   - Story ID **非必填**：有则用于文件名；没有则用 `0` 占位，并提示后续分配后重命名。

1. **收集需求来源**
   - 优先读取 PM 需求文档（按路径读取文件内容）。
   - 如用户直接描述，先**复述确认理解**，与原话有出入处明确标注。
   - 原始描述**尽量复述原文，不做二次加工**（写入「原始需求描述」章节）。

2. **提炼初步理解与已知信息**
   - 用 2-3 句话归纳「接收人的初步理解」，与原话有出入时标注。
   - 填「已知信息」表：目标用户/调用方、所属模块、预期上线时间、关联现有功能——**仅填需求中明确有的，缺失留 `[TBD]`**。

3. **列出已知未知项**（关键步骤，不可跳过）
   - 把需求中**没说清**的点列成清单（业务目标衡量指标 / 验收标准 / 性能安全要求 / 边界情况等）。
   - 这些会作为 `/spec-draft` 阶段 AI 主动提问的种子，**不在 intake 阶段强行补全**。

4. **生成 intake 草稿**
   - 按 `docs/intake/templates/intake-template.md` 模板章节填写。
   - 区分信息来源：📥 来自需求文档原文 / 🤖 AI 归纳 / ❓ [TBD]。

5. **写入文件**
   - 路径：`docs/intake/<VERSION>/<STORYID>-<slug>.md`
     - `STORYID`：已分配则用之，未分配用 `0` 占位
     - `<slug>`：kebab-case 小写短描述
     - 例：`docs/intake/v1.6.0/10088-payment-retry.md`、占位例 `docs/intake/v1.6.0/0-payment-retry.md`
   - frontmatter/头部包含：来源、日期、Story ID、优先级、目标 Spec（如已分配编号）。
   - `日期` 自动填当前日期。

6. **输出 intake 报告**
   - 来源与归档路径
   - 初步理解摘要
   - 已知未知项清单（待 `/spec-draft` 澄清）
   - 建议下一步（执行 `/spec-draft`）

## 输出结果

- 一份磁盘文件：`docs/intake/<VERSION>/<STORYID>-<slug>.md`
- intake 报告（chat 中输出）

## 后续动作

按顺序：

1. **人快速过目 intake**：确认理解无偏差，补充明显遗漏的已知信息
2. **（需求大时可选）执行 `/spec-design`**：先产出技术方案评审
3. **执行 `/spec-draft`**：以本 intake 为输入起草正式 spec

## 适用时机

- ✅ 接到 PM 输出的需求文档，需要先结构化理解
- ✅ 需求零散（来自多个 IM/邮件/会议），需要先归拢成一份草稿
- ✅ 团队规范要求 intake 留痕
- ❌ 需求极简单、习惯直接起草 → 跳过，直接 `/spec-draft`
- ❌ 已有 intake 文件，仅需修改局部 → 直接编辑该文件

## 关联规则与技能

- **规则**：`rules/10-spec-workflow.md`（**阶段零前置**）
- **模板**：`docs/intake/templates/intake-template.md`
- **下游命令**：`commands/spec-draft.md`

## 与其他命令的衔接

```
PM 需求文档 / 对话描述
  ↓
/spec-intake        ← 你现在用的命令（可选）
  ↓
docs/intake/<VERSION>/<STORYID>-<slug>.md（原始需求草稿）
  ↓ [人快速过目]
（需求大时可选）/spec-design → designs/<VERSION>/<STORYID>-<slug>-design.md (approved)
  ↓
/spec-draft
  ↓
specs/<VERSION>/<STORYID>-<slug>.md (status: draft)
  ↓
... (后续流程)
```
