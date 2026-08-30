# Eval 语料库（回测数据集）

> 本目录存放 `mr-spec-review` 经验闭环的**评测语料**：每一次评审评论的结构化处置标注。
> 它是 [`../EVAL-SPEC.md`](../EVAL-SPEC.md) 中**回测（§5.1）**与**提升效果度量（§6.4）**的 ground-truth 数据源。

## 为什么需要它

规则入库前要"自我评测"、入库后要算准确率/误报率/提升效果——这些都需要一份**可复现、可审计**的历史评审处置记录，而不是每次临时靠 MCP 现拉（现拉慢、且作者可能改了历史评论）。本目录把每轮评审的"评论 → 作者处置"沉淀成结构化语料，供回测与净化扫描离线复算。

## 文件组织

- `records.jsonl`：一行一条记录（JSON Lines）。追加写，不覆盖。
- 数据量大后可按季度分片：`records-2026Q3.jsonl` 等。

## 记录 Schema（每行一个 JSON 对象）

| 字段 | 类型 | 说明 |
|------|------|------|
| `mr` | string | MR 标识，如 `<ORG>/CoSpec!77` |
| `round` | int | 评审轮次（1=初评，2/3…=复评） |
| `date` | string | 该处置确定的日期 `YYYY-MM-DD` |
| `file_anchor` | string | 问题定位的稳定锚点：`文件路径#章节或符号`（**不用行号**，避免漂移） |
| `category` | string | 问题类别（与 Pattern 分类索引对齐，如 `命名/引用`、`抽象/分层`、`协议/兼容`） |
| `pattern_id` | string \| null | 命中的 Pattern 编号（如 `code#2` / `doc#D1`）；未命中已有 Pattern 则 `null` |
| `fingerprint` | string | 评论指纹（与投递评论结尾 `<!-- mr-spec-review:xxx -->` 一致） |
| `label` | string | `TP` / `FP` / `FN` / `unlabeled`（定义见 EVAL-SPEC §3.1） |
| `author_reply` | string | 作者回复摘要（用于判定 TP/FP 的依据，≤100 字） |
| `commit_after` | string \| null | 若 `resolved`，作者修复对应的 commit sha（用于核对"真的改了"，防假 TP） |

### 示例

```json
{"mr":"<ORG>/CoSpec!77","round":2,"date":"2026-07-16","file_anchor":"specs/v1.8.0/xxx.md#FR-6.2","category":"抽象/分层","pattern_id":"code#2","fingerprint":"a1b2c3","label":"TP","author_reply":"已下沉到 domain hook","commit_after":"9f3a1c2"}
{"mr":"<ORG>/CoSpec!77","round":2,"date":"2026-07-16","file_anchor":"specs/v1.8.0/xxx.md#FR-4","category":"性能/连接","pattern_id":"code#6","fingerprint":"d4e5f6","label":"FP","author_reply":"该路径调用频率极低，无需连接池","commit_after":null}
```

## 如何维护（AI 执行）

1. **每轮评审收尾（SKILL.md Step 8.1）后**：把本轮所投评论逐条追加为记录，`label` 初始多为 `unlabeled`（作者还没处置）。
2. **复评轮 / MR merge 时**：根据最新 `resolve_state` + 作者回复，把对应指纹记录的 `label` 更新为 `TP`（须核对 `commit_after` 真的改了对应处）或 `FP`（作者反驳）。
3. **bootstrap 历史**：对存量已合并 MR，用 `search_merge_request_notes` 批量回灌（按指纹/锚点对齐）。

> ⚠️ 只追加与更新 label，**不删除**历史记录——回测与 uplift 对比依赖完整历史。隐私/密钥不入库（遵守 @security_rules），`author_reply` 只存问题相关摘要。
