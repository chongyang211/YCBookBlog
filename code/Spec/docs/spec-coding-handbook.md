# Spec Coding 团队手册

> 一页纸版本，给团队 onboarding 用。  
> 完整规则见 `rules/`，详细工作流见 `rules/10-spec-workflow.md`。

> 📋 **全队 Spec 索引 / 当前迭代需求一览** → iWiki：<https://iwiki.woa.com/p/4022732388>（含版本号、Status、Owner、Spec/Plan/Tasks 链接）

---

## 一、为什么要做 Spec Coding

**问题**：传统「vibe coding」让 AI 直接写代码，不同人/不同 Agent 的执行结果差异大；改动范围漂移；文档与代码不同步。

**解法**：把开发流程切成 6 个核心阶段（外加阶段 6 后的 Sync 收尾），每个阶段产出一个**磁盘文件**，确保:
1. 同一件事不同人理解一致（**Spec**）
2. 同一件事不同人选同一条路（**Plan**）
3. 执行过程可暂停、可交接、可审计（**Tasks**）
4. 提交过程跨仓库一致、可追溯（**Push + Sync**）

---

## 二、三件套 + 六阶段全景图

```
原始需求（PM 需求文档 / IM / 会议）
   │
   ▼ （可选，视个人习惯）
┌ ─ ─ ─ ─ ┐
  Intake       /spec-intake → docs/intake/  （把 PM 需求/零散描述结构化成草稿）
└ ─ ─ ─ ─ ┘
   │
   ▼ （需求大/复杂时，可选）
┌ ─ ─ ─ ─ ┐
  Design       /spec-design → designs/  （技术方案，团队评审 → approved）
└ ─ ─ ─ ─ ┘
   │
   ▼
┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐
│ Draft    │→ │  Plan    │→ │  Tasks   │→ │ Implement│→ │  Test    │→ │  Review  │→ │   Push   │→ Sync
│ 起草草稿 │  │  怎么做  │  │ 步步执行 │  │  写代码  │  │ 写用例   │  │ 自查报告 │  │ commit + │
│          │  │          │  │          │  │ + 切分支 │  │          │  │ + 摘要   │  │ rebase   │
└──────────┘  └──────────┘  └──────────┘  └──────────┘  └──────────┘  └──────────┘  └──────────┘
docs/intake/   plans/         tasks/        src/(本地)    tests/        review 报告    远端 + MR
specs/(draft→ready)
```

| 阶段 | 命令 | 产物 | 模板 |
|------|------|------|------|
| 0 前置 — Intake（可选） | `/spec-intake` | `docs/intake/<VERSION>/<STORYID>-<slug>.md`（原始需求草稿） | `docs/intake/templates/intake-template.md` |
| 0 前 — Design（可选） | `/spec-design` | `designs/<VERSION>/<STORYID>-<slug>-design.md`（draft → approved） | `designs/templates/design-template.md` |
| 0 — Draft | `/spec-draft` | `specs/<VERSION>/<STORYID>-<slug>.md`（status: draft） | `specs/templates/spec-template.md` |
| 1 — Spec Review | （直接审） | （同上，status 改 ready） | — |
| 2 — Plan | `/spec-plan` | `plans/<VERSION>/<STORYID>-<slug>-plan.md` | `plans/templates/plan-template.md` |
| 2.5 — Tasks | `/spec-tasks` | `tasks/<VERSION>/<STORYID>-<slug>-tasks.md` | `tasks/templates/tasks-template.md` |
| 3 — Implement | `/spec-implement` | feature 分支 + `src/` 改动 + tasks 实时勾选 | — |
| 4 — Test | `/spec-test` | `tests/` 测试文件 | — |
| 5 — Review | `/spec-review` | review 报告（含变更摘要） | — |
| 6 — Push（可选） | `/spec-push` | commit + 安全 rebase + `git push -f` + MR 提示（视个人提交习惯，也可自行 git 提交） | `docs/git-workflow.md` |
| 6 后 — Sync | `/spec-sync` | spec 状态 + 三件套一致性同步 | `.gitlab/merge_request_templates/Default.md` |

> 💡 **大需求拆分**：Story ID 可被多个 spec 共享（一个大需求拆给多人，产生多个子 spec）。slug 是同 Story 下的真正区分键，必须有业务区分度。详见 `rules/10-spec-workflow.md`。
>
> 💡 **change-summary 不是独立命令**：变更摘要由 `/spec-review` 与 `/spec-push` 内部调用 `change-summary` skill 自动产出，不需要单独触发。

---

## 三、人 vs Agent 职责（速查表）

| 任务 | 人 | Agent |
|---|:---:|:---:|
| 业务目标判断 | ✅ | — |
| 把原始需求落到 docs/intake/（可选 /spec-intake） | ✅ | 🤝 整理 |
| 技术方案（/spec-design，需求大时） | ✅ 评审拍板 | ✅ 起草主力 |
| Spec 起草（/spec-draft） | 🤝 审 + 补 [TBD] | ✅ 主力 |
| 验收标准定义 | ✅ | 🤝 起草建议 |
| 安全/合规决策 | ✅ | — |
| Plan 起草 | 🤝 审 | ✅ 主力 |
| Tasks 拆解 | 🤝 校对 | ✅ 主力 |
| 代码实现 | 🤝 监督 | ✅ 主力 |
| 单元测试 | 🤝 抽检 | ✅ 主力 |
| Code Review | ✅ | 🤝 lint |
| 偏离 Spec 决策 | ✅ | ❌ 只能提建议 |
| 重构/引入新依赖 | ✅ 立项 | ❌ |

---

## 四、铁律（必须遵守）

1. **没有 Spec，不写代码。** 原始需求草稿放 `docs/intake/`，不要污染 `specs/`。
2. **Plan 与 Tasks 必须沉淀为磁盘文件。** 不允许只在对话中存在。
3. **执行权 ≠ 定义权。** Agent 不能改 Spec，只能在 tasks 文件的「偏离记录」中提建议。
4. **PR 必须可追溯到 Story ID。** 用 `.gitlab/merge_request_templates/Default.md` 三段式。
5. **变更摘要不可省略。** 即使是 1 行 bugfix。
6. **实时勾选。** 每完成一条 task，立刻 `[x]`，不允许批量补勾。
7. **偏离必须记录。** 沉默偏离视为缺陷。
8. **分支命名严格统一。** 实施阶段在所有相关仓库使用 `{feature|hotfix}/{spec-name}`，多仓库一致。
9. **Push 前必须安全 rebase。** 切基线 → `git pull -r` → 切回 feature → `git rebase 基线` → `git push -f`。详见 `docs/git-workflow.md`。
10. **Commit Message 严格规范。** `<type>(<scope>): <subject> --story=<STORYID> [#finish]`。`#finish` 仅加在 Story 最后一笔。

---

## 五、新人 onboarding 5 分钟流程

> 📘 **完整版手册**：`docs/onboarding-codebuddy.md`（含 0001 端到端实操、CodeBuddy IDE 操作示范、常见错误自救）

### 第 1 步：读三个文件
1. 这份 handbook
2. `rules/10-spec-workflow.md`（最核心）
3. 任意一个完成度高的样板：`specs/<VERSION>/<STORYID>-<slug>.md` + 同名 `plans/...-plan.md` + `tasks/...-tasks.md`（在 iWiki 索引 <https://iwiki.woa.com/p/4022732388> 中找 status 为 `implemented` 的就是好样板）

### 第 2 步：跑通一次完整链路（用示例 spec `0-example-feature`）

```
/spec-plan      specs/v1.6.0/0-example-feature.md
（review plan）
/spec-tasks     specs/v1.6.0/0-example-feature.md
（review tasks）
/spec-implement specs/v1.6.0/0-example-feature.md
/spec-test      specs/v1.6.0/0-example-feature.md
/spec-review    specs/v1.6.0/0-example-feature.md
/spec-push      specs/v1.6.0/0-example-feature.md     ← commit + 安全 rebase + push
（在工蜂创建 MR、合并）
/spec-sync      specs/v1.6.0/0-example-feature.md
```

### 第 3 步：第一个真实任务
- 先和架构师 / Tech Lead 一起确认需求落到 `docs/intake/`
- 让 Agent 起 Spec（`/spec-draft`），自己审 + 补 [TBD] → status 改 `ready`
- 让 Agent 起 Plan（`/spec-plan`），自己审
- 让 Agent 拆 Tasks（`/spec-tasks`），自己审
- 让 Agent 实施（`/spec-implement`），自己看每个 commit
- 写测试（`/spec-test`） → Review（`/spec-review`） → Push（`/spec-push`） → 在工蜂提 MR → 合并 → Sync（`/spec-sync`）

---

## 六、常见误区

| 误区 | 正确做法 |
|------|----------|
| 「Spec 太小不用写 plan」 | 即使是 30 行改动，也要 1-2 段 plan，避免无据可依 |
| 「我看了一遍 plan，可以开干了」 | Plan 必须**写入** `plans/`；对话中的同意不算 |
| 「Tasks 完成后再统一打 ✓」 | 必须实时勾选，否则中断后无法接续 |
| 「Agent 觉得 spec 不合理就改了」 | 严禁。Agent 只能在 tasks 文件提偏离建议，由人决策 |
| 「顺手优化一下相邻代码」 | 严禁。重构是独立 spec，记录到 docs/intake/ 待立项 |
| 「测试只写 happy path」 | 必须覆盖 spec 的「边界情况」章节每一条 |
| 「PR 描述写一句话」 | 必须三段式：关联 Spec / 偏离说明 / 变更摘要 |

---

## 七、目录速查

| 目录 / 文件 | 作用 |
|----|----|
| `CLAUDE.md` | Claude Code 入口，自动加载规则与导航 |
| `README.md` | 仓库门面，含首次使用流程 |
| `rules/` | 5 条核心规则（00 原则 / 10 工作流 / 20 编码 / 30 测试 / 40 文档） |
| `skills/` | 8 个 SKILL（technical-design / spec-drafting / spec-analysis / codebase-survey / implementation-planning / feature-implementation / test-writing / change-summary） |
| `commands/` | 10 个斜杠命令（intake / design / draft / plan / tasks / implement / test / review / push / sync） |
| `specs/` | Spec 文件 + 模板（聚合索引发布到 iWiki：<https://iwiki.woa.com/p/4022732388>） |
| `designs/` | 技术方案文件 + 模板（可选前置环节，需求大时用） |
| `plans/` | Plan 文件 + 模板 |
| `tasks/` | Tasks 文件 + 模板 |
| `docs/` | 架构 / 部署 / 接口文档 |
| `docs/intake/` | 原始需求草稿区（**不是** Spec） |
| `docs/git-workflow.md` | Git 流程操作手册（基线分支映射、commit 规范、安全 rebase） |
| `docs/onboarding-codebuddy.md` | 新人完整使用手册 |
| `tests/` | 测试代码（按 spec 验收标准对齐） |
| `.gitlab/` | MR 模板（工蜂） |
| `.codebuddy/` | CodeBuddy IDE 协作配置（commands/rules 通过软链共享根目录同一份内容） |
| `src/` | 业务代码仓库（**已 gitignore**，团队自行 clone 子仓库到 `src/<repo>/`） |

---

## 八、流程例外（什么情况可以简化）

| 场景 | 可省略 | 不可省略 |
|------|--------|----------|
| 纯 bugfix（< 50 行） | Plan、Tasks | Spec、Test、Review、Push、Summary |
| 纯文档 / 注释修改 | Plan、Tasks、Test | Push、Summary |
| 紧急 hotfix（用 `hotfix/` 前缀） | Plan、Tasks | Spec（事后补）、Push、Summary |
| 配置项调整 | Plan、Tasks、Test（视情况） | Push、Summary |

**变更摘要、commit message 规范、push 前安全 rebase 在任何情况下都不可省略。**

---

## 九、推广路径

1. **第 1 周**：Tech Lead + 1 位志愿者用一个真实小需求跑完整命令链路，全员旁观
2. **第 2-3 周**：每个新 Spec 必须三件套；老 Spec 不强制回填；MR 必须用 `.gitlab/merge_request_templates/Default.md`
3. **第 4 周后**：CI 加卡口（MR 必须含 `--story=` commit、关联 spec/plan/tasks 文件、跑通测试）
4. **3 个月后**：复盘《Spec Coding 手册 v2》，沉淀团队特化经验

---

## 十、Q&A

**Q: 什么时候需要先做技术方案（/spec-design）？和 Plan 有什么区别？**  
A: 需求大、跨多模块/多仓库、或有多种技术方案要评审时，先做技术方案，团队评审通过后再写 spec，避免 spec 因技术方向未定而反复返工。小需求直接 `/spec-draft`。**Design 在 spec 之前，决定「用什么方案、拆几个 spec」；Plan 在 spec 之后，决定「改哪些文件、按什么步骤」。** 颗粒度和时机都不同。

**Q: AI 已经在执行过程中"心算"出 plan 了，为什么还要写文件？**  
A: 因为下一个人/Agent 看不到那个心算 plan。文件 = 共享内存。

**Q: Tasks 太琐碎，写起来像填表？**  
A: 那就是它的目的。可勾选 = 可暂停。中断后任何人都能接续。

**Q: Spec 已经写得很详细了，还需要 Plan 吗？**  
A: Spec 写「做什么」，Plan 写「在哪个文件、按什么顺序、为什么这样选」。两者关注点不同。

**Q: 如果业务一直在变，Spec 跟不上怎么办？**  
A: 那就是个信号：要么把 Spec 拆得更小，要么把变更改成新增 Spec。Spec 漂移就是流程崩塌的开始。

---

> 本手册是活文档，欢迎在 `docs/spec-coding-handbook.md` 提 PR 改进。
