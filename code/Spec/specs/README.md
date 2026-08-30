# Specs 目录

> 📋 **全队 Spec 索引 / 当前迭代需求一览** → 团队 Wiki：<TEAM-WIKI-URL>（含每个 spec 的版本号、Status、Owner、Spec/Plan/Tasks 链接）

## 作用

`specs/` 是所有功能需求的**单一事实来源**。每个 spec 描述一个独立的功能需求或变更，是 AI 进行实现、测试、总结的起点。

AI 在执行任何功能开发任务时，**必须首先查找并阅读相关 spec**，而不是仅凭口头描述或模糊指令。

## 如何新增 Spec

1. 复制 `templates/spec-template.md` 到 `specs/<VERSION>/` 目录（当前迭代版本目录，如 `specs/v1.6.0/`）
2. 按命名规范命名（见下文）
3. 填写模板中的各个章节
4. 不确定的部分标记为 `[TBD]`，AI 会在分析阶段指出

## 命名规范

```
specs/<VERSION>/<STORYID>-<kebab-case-slug>.md
```

> 📂 **版本目录层级**：spec 文件按迭代版本归档到 `<VERSION>/` 子目录（如 `v1.6.0/`）；`templates/`、`README.md` 保留在 `specs/` 根。聚合索引由 `/spec-index` 命令生成并完整覆盖同步到 团队 Wiki（<TEAM-WIKI-URL>），仓库内不再维护 `INDEX.md`。

- `<VERSION>`：迭代版本号（如 `v1.6.0`）
- `STORYID`：纯数字 Story ID（与需求单号关联，便于追溯）
  - 来自Git 平台（GitHub / GitLab / 工蜂） / JIRA / 其他需求管理系统
  - 如该 Spec 无对应 story（纯内部任务），用 `0` 占位
  - **不强制唯一**——一个大需求可拆为多个 spec 共享同一 Story ID
- `<kebab-case-slug>`：小写字母 + 连字符的简短描述
  - **同 Story 下必须唯一**（slug 是真正的区分键）
  - 应有业务区分度（按模块 / 子功能命名）
- **同一版本目录内文件名（`<STORYID>-<slug>.md`）必须唯一**

示例：
- `specs/v1.6.0/0-example-feature.md`（示例 spec，无 story id）
- `specs/v1.6.0/10086-example-user-login.md`（单 spec）
- `specs/v1.6.0/10088-payment-retry.md`（单 spec）

**大需求拆分示例（同 Story 多 spec）**：
- `specs/v1.6.0/10088-gateway-changes.md`（张三：网关层）
- `specs/v1.6.0/10088-controller-impl.md`（李四：控制器层）
- `specs/v1.6.0/10088-example-channel.md`（<owner>：通道层）

## 分支命名规范

实施阶段在所有相关仓库统一使用：`{feature|hotfix}/{spec-name}`，其中 `{spec-name}` 是 spec 文件名去 `.md` 后缀。

| spec | 分支名 |
|------|-------|
| `specs/v1.6.0/10086-example-user-login.md` | `feature/10086-example-user-login` |
| `specs/v1.6.0/10088-gateway-changes.md` | `feature/10088-gateway-changes` |
| `specs/v1.6.0/0-fix-payment-timeout.md`（紧急 bug） | `hotfix/0-fix-payment-timeout` |

> 💡 与 `plans/` 与 `tasks/` 文件名保持**完全一致的 STORYID + slug**，仅各自加 `-plan` / `-tasks` 后缀。
>
> 💡 当一个 Story 拆为多个子 spec 时，建议在每个 spec 的 frontmatter 增加 `Sibling Specs` 字段，列出兄弟 spec 路径，便于追溯。
>
> ⚠️ 设计取舍：早期版本曾使用 `NNNN-<STORYID>-<slug>` 形式，但顺序号 `NNNN` 在多人并行开发时易冲突且难维护，**已废弃**。`<STORYID>-<slug>` 整体已足够唯一定位，无需顺序号。

## Spec 生命周期

| 状态 | 含义 |
|------|------|
| `draft` | 初稿，尚未完整，可能有 TBD 项 |
| `ready` | 已完整，可以开始实施 |
| `in-progress` | 正在实施中 |
| `implemented` | 代码已完成，测试已通过 |
| `deprecated` | 已废弃，不再适用 |

在 spec 文件的 frontmatter 或头部标注当前状态。

## AI 如何使用 Spec

1. **接到任务时**：扫描 `specs/` 查找相关 spec
2. **分析阶段**：逐节阅读 spec，提取功能需求、验收标准、边界情况
3. **发现问题时**：指出 spec 中的模糊、矛盾或缺失部分，不擅自假设
4. **实施阶段**：严格按 spec 中的功能需求和验收标准实现
5. **测试阶段**：根据 spec 中的测试点和验收标准编写测试
6. **完成时**：对照 spec 检查所有验收标准是否满足
