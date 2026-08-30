# Command: spec-index

## 命令名称

`spec-index`

## 作用

**生成 Spec 索引并完整覆盖同步到 团队 Wiki**，作为团队唯一的 Spec 一览入口。

- 扫描 `specs/<VERSION>/*.md`，按 Story ID 分组、版本归档，输出一张完整索引表
- 通过 团队 Wiki MCP **每次完整覆盖**目标 团队 Wiki 文档（docid: `4022732388`，URL: <TEAM-WIKI-URL>）
- **不在仓库内落盘任何聚合文件**，从根本上消除 MR 因索引文件冲突的问题

> ⚠️ **带外工具，不属于个人开发流程**：每个人都在自己的 feature 分支上开发，工作树是半成品状态。本命令仅由**专人**（Tech Lead / 迭代负责人 / 定时任务 bot）在 **master 基线**上按需运行，避免不同人在不同分支上各自重生导致 团队 Wiki 文档被反复以半成品覆盖。

> ⚠️ 历史上 `specs/INDEX.md` 是手工维护的聚合表，导致并发 MR 几乎必冲突。本命令出现后，该文件**已被移除**，索引一律由本命令在 master 基线上单点发布到 团队 Wiki。

## 输入参数

| 参数 | 必选 | 说明 |
|------|------|------|
| 范围 | 否 | 默认全量（所有版本）。可指定 `version=v1.7.0` 只重生该版本表，但**同步到 团队 Wiki 时仍是全量覆盖**（避免漏行） |
| `--dry-run` | 否 | 仅在 chat 中打印生成结果，**不**调用 团队 Wiki MCP 同步 |

**触发方式**：

```
请执行 spec-index
```

或：

```
请执行 spec-index --dry-run
```

## 执行步骤

0. **前置校验**
   - 当前工作目录是仓库根（包含 `specs/` 目录）
   - 团队 Wiki MCP（工具名 `团队 Wiki-saveDocument`）可用

1. **扫描所有 spec 文件**
   - 遍历 `specs/v*/` 下的 `*.md`（排除 `templates/` / `README.md` / 任何 `INDEX*.md` 残留）
   - 解析每个文件的 frontmatter / 头部元信息，提取字段：
     - `Story ID`（必有；文件名 `<STORYID>-<slug>.md` 与 frontmatter 字段需一致，否则在报告中标注「不一致」）
     - `Title`（取一级标题 `# xxx`；若 frontmatter 有 `Title` 字段优先）
     - `Status`（`draft` / `ready` / `in-progress` / `implemented` / `deprecated`）
     - `Owner` / `Author`
     - `Updated`（无则用文件 `git log -1 --format=%ad` 最近提交日期兜底）
     - `Version`：从目录路径解析（如 `specs/v1.7.0/xxx.md` → `v1.7.0`）
   - 兼容两种 frontmatter 风格：
     - YAML 风格：`---\nStory ID: 10086\n---`
     - 引用风格：`> **Story ID**: 10086`

2. **派生 Plan / Tasks 链接**
   - 同名规则推导：
     - Plan: `plans/<VERSION>/<STORYID>-<slug>-plan.md`
     - Tasks: `tasks/<VERSION>/<STORYID>-<slug>-tasks.md`
   - 文件存在 → 输出**远程仓库绝对 URL** 链接；不存在 → 输出 `—`
   - **链接基底（必须使用）**：`<GIT-HOST>/<ORG>/CoSpec/blob/<默认分支>/`
     - Spec 链接：`<GIT-HOST>/<ORG>/CoSpec/blob/<默认分支>/specs/<VERSION>/<STORYID>-<slug>.md`
     - Plan 链接：`<GIT-HOST>/<ORG>/CoSpec/blob/<默认分支>/plans/<VERSION>/<STORYID>-<slug>-plan.md`
     - Tasks 链接：`<GIT-HOST>/<ORG>/CoSpec/blob/<默认分支>/tasks/<VERSION>/<STORYID>-<slug>-tasks.md`
   - **不要使用相对路径**：团队 Wiki 渲染后相对路径无法跳转，必须用 `<GIT-HOST>/...` 绝对 URL，让团队成员从 团队 Wiki 一键跳到Git 平台（GitHub / GitLab / 工蜂）仓库查看源文件。
   - 如未来 master 分支不再作为对外基线，仅在此处统一调整基底即可。

3. **排序规则**
   - 主排序：**Version 倒序**（最新迭代在最上）
   - 次排序：同版本内按 **Story ID 升序**
   - 同 Story ID 的多个 spec **保持连续**，slug 字母序

4. **生成 Markdown 内容（完整覆盖体）**

   严格按下方"输出格式"章节的模板组装。Updated 时间为当前日期。

5. **同步到 团队 Wiki**
   - 调用 `团队 Wiki-saveDocument`：
     - `docid`: `4022732388`
     - `title`: `Spec INDEX`
     - `is_html`: `false`
     - `body`: 第 4 步生成的完整 Markdown
   - **不做增量合并**：每次都是完整覆盖（符合用户明确要求）
   - `--dry-run` 模式跳过本步骤

6. **输出执行报告**（chat 中）
   - 扫描到的 spec 总数 / 按 Version 分布
   - 按 Status 统计（draft / ready / in-progress / implemented / deprecated）
   - 异常清单：frontmatter 与文件名 Story ID 不一致、缺 Status、缺 Owner 等
   - 团队 Wiki 同步结果（成功 / 失败 + 链接）

## 输出格式（团队 Wiki 文档正文模板）

> 以下是同步到 团队 Wiki 的**完整正文**，每次执行都重新生成并覆盖。

```markdown
# Spec INDEX

> 本文档由 `/spec-index` 命令自动生成，**请勿手工编辑**。
> 仓库侧已移除 `specs/INDEX.md`，单一事实来源是各 spec 文件本身的 frontmatter。
> 重新生成方式：在仓库根目录执行 `/spec-index`。

**最后更新**：YYYY-MM-DD HH:MM

## 总览

| 版本 | Spec 总数 | draft | ready | in-progress | implemented | deprecated |
|------|----------|-------|-------|-------------|-------------|------------|
| v1.7.0 | 6 | 3 | 3 | 0 | 0 | 0 |
| v1.6.0 | 3 | 1 | 2 | 0 | 0 | 0 |
| ... | | | | | | |

## Spec 列表

> 同一 Story ID 下的多个子 spec 会连续显示。
> Spec / Plan / Tasks 列均为**远程仓库绝对 URL**（基底 `<GIT-HOST>/<ORG>/CoSpec/blob/<默认分支>/`），点击可直接跳转到Git 平台（GitHub / GitLab / 工蜂）查看源文件。

### v1.7.0

| 版本 | Story ID | 标题 | Status | Owner | Spec | Plan | Tasks | Updated |
|------|----------|------|--------|-------|------|------|-------|---------|
| v1.7.0 | 134747897 | O4 端 — OTA 升级过程分阶段实时上报 | ready | yc | [spec](<GIT-HOST>/<ORG>/CoSpec/blob/<默认分支>/specs/v1.7.0/10087-example-ota-upgrade.md) | — | — | 2026-06-22 |
| v1.7.0 | 134754755 | O1 设备端「上传日志」功能（设置入口） | ready | <owner> | [spec](<GIT-HOST>/<ORG>/CoSpec/blob/<默认分支>/specs/v1.7.0/134754755-o1-device-log-upload.md) | — | — | 2026-06-22 |
| ... | | | | | | | | |

### v1.6.0

| 版本 | Story ID | 标题 | Status | Owner | Spec | Plan | Tasks | Updated |
|------|----------|------|--------|-------|------|------|-------|---------|
| v1.6.0 | 0 | Todo 列表管理 API（示例） | ready | Human | [spec](<GIT-HOST>/<ORG>/CoSpec/blob/<默认分支>/specs/v1.6.0/0-example-feature.md) | — | — | 2026-03-26 |
| ... | | | | | | | | |

## 状态图例

| 状态 | 含义 |
|------|------|
| `draft` | 初稿，可能含 TBD，不允许进入实施 |
| `ready` | 已评审，可以开始 plan / 实施 |
| `in-progress` | 正在实施中 |
| `implemented` | 代码已完成、测试已通过、文档已同步 |
| `deprecated` | 已废弃，仅保留作为历史记录 |

## 命名规范

| 文件类型 | 命名格式 |
|---------|---------|
| Intake | `docs/intake/<VERSION>/<STORYID>-<slug>.md` |
| Design（可选） | `designs/<VERSION>/<STORYID>-<slug>-design.md` |
| Spec | `specs/<VERSION>/<STORYID>-<slug>.md` |
| Plan | `plans/<VERSION>/<STORYID>-<slug>-plan.md` |
| Tasks | `tasks/<VERSION>/<STORYID>-<slug>-tasks.md` |

详见仓库 `rules/10-spec-workflow.md`。
```

## 适用时机

> ⚠️ **这是带外工具，不是工作流的一部分**。每个人都在自己的 feature 分支上开发，工作树是部分完成的；如果人人都跑，团队 Wiki 文档会被反复以半成品状态覆盖，反而把索引搞乱。

- ✅ 由**专人**（如 Tech Lead / 迭代负责人 / 定时任务）在 master 基线上按需运行：迭代评审前、周会前、需求看板需要刷新时
- ✅ 也可放进**定时任务**（如每天一次 / 每次 master 有合并后）由 bot 账号统一执行
- ❌ **不要**在个人 feature 分支上、PR 还没合并时运行
- ❌ 改了 spec 正文但没动 frontmatter / Status / Owner / Updated 之类元信息 → 不需要跑（索引不会变）

## 设计要点（为什么这么做）

1. **MR 零冲突**：仓库内不再有任何聚合文件，开发者改 spec frontmatter 即可，PR diff 永远不含索引。
2. **完整覆盖而非增量**：团队 Wiki 文档每次整篇重写，避免"半同步"留下旧条目；同名 spec 状态前后顺序一致。
3. **单一事实来源**：spec frontmatter 是 source of truth，团队 Wiki 是 derived view；本命令是两者之间的唯一同步通道。
4. **带外执行**：仅在 master 基线上由专人/工具运行，避免不同分支上的"半成品"互相覆盖。
5. **链接到Git 平台（GitHub / GitLab / 工蜂）绝对 URL**：团队 Wiki 渲染时相对路径无法跳转，索引表里的 Spec / Plan / Tasks 一律使用 `<GIT-HOST>/<ORG>/CoSpec/blob/<默认分支>/...` 绝对地址，让团队成员从 团队 Wiki 一键跳到Git 平台（GitHub / GitLab / 工蜂）查看源文件。
6. **列表带版本号列**：方便筛选/搜索/复制粘贴到其他文档时保留迭代上下文，不必依赖分组标题。

## 关联规则与技能

- **规则**：`rules/10-spec-workflow.md`、`rules/40-documentation-rules.md`
- **依赖 MCP**：`团队 Wiki-saveDocument`（覆盖式写入）
- **执行约定**：由专人或定时任务在 master 基线上运行，不进入个人开发流程
