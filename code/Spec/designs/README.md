# Designs — 技术方案目录

本目录存放**技术方案（Technical Design）**文档。技术方案是工作流中的**可选前置环节**，位于阶段零（Draft）之前。

## 什么时候需要技术方案

当需求**较大、较复杂、跨多个模块/仓库，或存在多种技术方案需要评审拍板**时，先做技术方案：

```
原始需求 (docs/intake/<VERSION>/)
  ↓
/spec-design  →  designs/<VERSION>/<STORYID>-<slug>-design.md
  ↓ [团队评审 → status: approved]
/spec-draft   →  基于「intake + design」起草更准确的 spec
  ↓
... (plan / tasks / implement / test / review / push / sync)
```

小需求 / 改动清晰的需求可**跳过本环节**，直接 `/spec-draft`。

## Design vs Plan（别混淆）

| | 技术方案 Design（本目录） | 实施计划 Plan（`plans/`） |
|------|------|------|
| 时机 | spec 之前 | spec ready 之后 |
| 关注 | 架构、技术选型、方案对比、**spec 拆分建议** | 落地到具体文件的执行步骤 |
| 颗粒度 | 方案级（怎么做、为什么） | 执行级（改哪些文件、按什么顺序） |

> 一句话：**Design 决定「用什么方案、拆几个 spec」，Plan 决定「改哪些文件、按什么步骤」。**

## 命名规范

| 文件类型 | 命名格式 | 示例 |
|---------|---------|------|
| Design | `designs/<VERSION>/<STORYID>-<slug>-design.md` | `designs/v1.6.0/10086-example-user-login-design.md` |

> 📂 **版本目录层级**：design 文件按迭代版本归档到 `<VERSION>/` 子目录（如 `v1.6.0/`）；`templates/`、`README.md` 保留在 `designs/` 根。

- `<VERSION>`：迭代版本号（如 `v1.6.0`）
- `STORYID`：纯数字需求单号；无 story 用 `0` 占位
- `<slug>`：kebab-case 小写描述，与后续 spec / plan / tasks 的 slug 对齐
- 模板见 `designs/templates/design-template.md`

## 状态图例

| 状态 | 含义 |
|------|------|
| `draft` | 初稿，待评审 |
| `reviewing` | 评审中 |
| `approved` | 已评审通过，可进入 `/spec-draft` |
| `obsolete` | 已废弃 / 被新方案取代 |
