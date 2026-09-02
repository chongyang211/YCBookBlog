# 04｜使用手册

> 📖 **本篇定位**：**上手即用**——三种规模场景 × 完整命令示例 × 常见问题手册。
> 🔗 **上一篇**：[03 原理与思想](./03-principles.md) · **下一篇**：[05 目录与文件契约](./05-architecture.md)

---

## 一、开始之前

### 1.1 你需要的准备

1. **一个 AI 编程助手**（Cursor / Codebuddy / Continue / Cline 等任一支持自定义规则和命令的工具）
2. **一个 Git 仓库**（可以是空的，或已有代码）
3. **CoSpec 骨架文件**（本仓库的 `rules/` / `commands/` / `skills/` / `docs/` 等）—— 见 [06 落地你自己的项目](./06-adaptation.md) 的克隆步骤

### 1.2 你不需要的东西

- ❌ 不需要特定编程语言
- ❌ 不需要特定项目框架
- ❌ 不需要特定 Git 平台（GitHub / GitLab / Gitee / 自建都可）
- ❌ 不需要特定操作系统

### 1.3 五分钟检查清单

在开始使用 CoSpec 前，确认：

- [ ] AI 助手已加载 `rules/*.md`（作为常驻规则）
- [ ] AI 助手已识别 `commands/*.md`（作为 slash command）
- [ ] `specs/` / `plans/` / `tasks/` / `intake/` / `designs/` 目录已创建（或首次 `/spec-*` 命令会自动创建）
- [ ] 你选定了本项目的**版本号**（如 `v1.0.0`），后续所有产物按版本归档

---

## 二、三种规模场景

CoSpec 是**分级使用**的——按需求规模选择不同深度。

### 2.1 场景 A：极简需求（改 5-10 行）

**判定标准**：

- 单文件 / 单函数改动
- 无新增依赖
- 无跨模块影响

**推荐流程**：

```txt
1. /spec-draft          → 起草 spec（简版，只填必要章节）
2. 人审                  → status: ready
3. /spec-implement       → 直接改代码（skip plan / tasks）
4. /spec-sync            → status: implemented
```

**示例对话**：

```
你：/spec-draft 帮我为"给登录接口加请求日志"起草一份简单的 spec，Story ID = 100

AI：好的，我先做几个澄清：
1. 日志级别用 info 还是 debug？
2. 是否需要脱敏（如密码、token）？
3. 日志写入哪里？（stdout / 独立文件 / 现有 logger）
...

你：info；密码必脱敏；写现有 logger

AI：[起草 specs/v1.0.0/100-login-request-log.md，Status: draft]
    请找一位同事 review 后把 Status 改为 ready，我再进入实现阶段。

（同事 review → 你手动改 status: ready）

你：/spec-implement specs/v1.0.0/100-login-request-log.md

AI：[切分支 feature/100-login-request-log，改代码，运行测试，commit]
    实现完成。已 commit：
    - feat(auth): add request logging with password masking --story=100 [#finish]

你：/spec-sync

AI：[更新 spec status: implemented，勾选所有验收标准]
```

**总耗时**：约 20-30 分钟（含人审）。

---

### 2.2 场景 B：中等需求（新增/修改 3-8 个文件）

**判定标准**：

- 涉及多个文件
- 有新增数据结构或 API
- 无跨仓库影响

**推荐流程**：

```txt
1. /spec-draft           → 起草 spec
2. 人审                   → status: ready
3. /spec-plan            → 起草实施计划
4. /spec-tasks           → 拆任务清单
5. /spec-implement       → 实施（tasks 实时勾选）
6. /spec-test            → 写测试并运行
7. /spec-sync            → 归档
```

**示例：给用户系统加 OAuth 登录**

```
你：intake/v1.0.0/234-oauth-login.md 已经放好原始需求，
    请执行 /spec-draft 起草 spec，Story ID = 234

AI：读取 intake... 做 light 代码侦察...
    澄清 5 个问题：
    1. 支持哪些 provider？(Google / GitHub / OIDC 提供商 / ...)
    2. 是否需要账号合并？(已有邮箱账号 + 新的 OAuth 账号)
    3. Token 存储位置？(数据库 / 独立表 / Session)
    4. 失败降级策略？(回退到密码登录 / 显示错误)
    5. 是否影响移动端？

你：Google + GitHub；需要账号合并（按邮箱）；独立表；回退密码；只影响 Web

AI：[起草 specs/v1.0.0/234-oauth-login.md，Status: draft]

（同事 review → status: ready）

你：/spec-plan specs/v1.0.0/234-oauth-login.md

AI：[读 spec，deep 代码侦察，起草 plan]
    Plan 已起草，改动范围：
    - 新增：oauth_provider 表、OAuthService 类、/api/oauth/* 路由
    - 修改：User 表加 oauth_id 字段、LoginController 加分支
    - 共计新增 4 个文件、修改 3 个文件
    Phase 拆分：数据层 → 服务层 → 路由层 → 前端整合
    请确认。

你：确认

AI：[写入 plans/v1.0.0/234-oauth-login-plan.md]

你：/spec-tasks

AI：[基于 plan 拆成 4 个 Phase 共 15 个任务，写入 tasks 文件]

你：/spec-implement

AI：[切分支 feature/234-oauth-login，开始 Phase 1]
    [完成任务 1.1 → 更新 tasks: [ ] → [x] → commit]
    [完成任务 1.2 → 更新 tasks → commit]
    ...

你：/spec-test

AI：[对照验收标准写测试，运行]

你：/spec-review

AI：[对照验收标准 + plan 检查改动范围，输出 review 报告]

你：/spec-push

AI：[本地校验 → rebase → push → 生成 MR 描述]

（MR 合并）

你：/spec-sync

AI：[更新 spec status: implemented，回写偏离记录（若有）]
```

**总耗时**：约 2-4 小时（含测试和 review）。

---

### 2.3 场景 C：大型需求（跨模块 / 跨仓库 / 需评审技术方案）

**判定标准**：

- 涉及 ≥ 2 个仓库或 ≥ 5 个模块
- 有多种技术方案可选，需评审
- 影响核心数据模型 / 关键 API
- 有性能 / 安全 / 兼容性等硬约束

**推荐流程**：

```txt
1. /spec-intake          → 落原始需求到 intake/
2. /spec-design          → 起草技术方案（含 spec 拆分建议）
3. 团队评审               → design status: approved
4. /spec-draft × N       → 按 design 建议起草多个子 spec
5. 每个子 spec 人审        → status: ready
6. /spec-plan × N        → 各 spec 独立起 plan
7. /spec-tasks × N       → 各自的 tasks
8. /spec-implement       → 分批实施（跨仓库同名分支）
9. /spec-test /spec-review
10. /spec-push /spec-sync
```

**关键差异**：

- **多 spec 共享一个 Story ID**：一个大需求拆多个 spec，共用 Story ID，靠 slug 区分
- **跨仓库同名分支**：所有涉及的仓库都建 `feature/12345-<slug>` 分支
- **每个子 spec 独立走完流程**：不合并 plan / tasks

**示例：大改造"账户体系统一"**

```
Design 输出：
- 建议拆 4 个子 spec：
  1. 234-account-schema-migration  (数据层)
  2. 234-account-service-refactor  (服务层)
  3. 234-account-api-v2            (API 层)
  4. 234-account-client-migration  (客户端)

四个子 spec 分给不同人 / 不同时间实施，
但共享 Story ID = 234，MR 全部带 --story=234。
```

**总耗时**：数天至数周（一个大需求切多个 spec，各自并行）。

---

## 三、10 个最常用的日常操作

### 3.1 起草一个新 spec

```
/spec-draft 帮我起草一个 spec：<自然语言描述>
或
/spec-draft intake/v1.0.0/<file>.md
```

### 3.2 检查一个 spec 是否可以进入实施

```
帮我用 skills/spec-analysis 分析 specs/v1.0.0/234-xxx.md，
看看有没有 TBD 或不完整的地方。
```

### 3.3 从一份 spec 生成实施计划

```
/spec-plan specs/v1.0.0/234-xxx.md
```

### 3.4 从一份计划生成任务清单

```
/spec-tasks plans/v1.0.0/234-xxx-plan.md
```

### 3.5 开始实施

```
/spec-implement tasks/v1.0.0/234-xxx-tasks.md
```

### 3.6 实施过程中遇到与 spec 冲突

**不要** 让 AI 自己改 spec。**应该**：

```
你：AI，你现在停下。这个冲突不是我预期的，让我看看。
    请把冲突点记录到 tasks 的"偏离记录"章节。

AI：[记录偏离，等待人决策]

（你评估后决定：改代码 / 改 spec / 改 plan）
```

### 3.7 换会话继续之前的 spec

```
帮我继续 tasks/v1.0.0/234-xxx-tasks.md 的实施，
从第一个未勾选的任务开始。
```

AI 会自动读 tasks 找到断点。

### 3.8 一个 spec 完全实施完毕

```
/spec-sync specs/v1.0.0/234-xxx.md
```

### 3.9 一份旧 spec 需要废弃

手动改 `Status: deprecated` 并在"修订记录"注明原因。

### 3.10 想快速看某个 Story 的全景

```bash
# 找齐一个 Story 的所有产物
find . -name "234-*" | sort

# 找齐一个 Story 的所有 commit
git log --all --grep '--story=234'
```

---

## 四、常见问题手册（FAQ）

### Q1：Spec 到底该写多详细？

**判定标准**：

- 一个不了解这个需求的同事读完 spec 后，能不能开始实施？
- 验收标准是不是每条都可以打勾验证？
- 有没有留 `[TBD]`？

**过度详细的信号**：spec 里出现具体函数名、变量名——那是 Plan 的事。

**过于简略的信号**：验收标准写"实现良好"、"性能足够"这种模糊描述。

### Q2：一定要走完所有阶段吗？

**不**。见 §2.1 场景 A 的极简流程。

**核心不可省的**：
- Spec 起草 + 人审（必须）
- Story ID 串联（必须）
- Sync 更新状态（必须）

**可省的**：intake / design / plan / tasks / push（视规模）

### Q3：AI 起草的 spec 里 TBD 太多怎么办？

**这是好事**——说明 AI 老实。

**处理**：

1. 逐条 TBD 找业务方 / 架构师问答
2. 答案回填 spec
3. 全部 TBD 清空后 status: ready

**不要** 让 AI 帮你填 TBD——那是幻觉源头。

### Q4：AI 起草的 spec 我觉得写得不对，可以直接改吗？

**当然可以**。人有一切权利改 spec。改完记得：

- 更新 `Updated` 日期
- 在"修订记录"里追加一行说明

### Q5：Plan 和 Tasks 应该 AI 起草还是人写？

**AI 起草，人审**。理由：

- AI 更擅长枚举文件、拆分步骤
- 人更擅长判断优先级、识别遗漏

### Q6：Tasks 我做完一个才勾选一个太麻烦

**这是最容易破功的地方**——请坚持。

**为什么必须实时**：如果你今天做完 5 个任务才回来批量勾选，AI 明天接手时读到"1-5 未勾选"就会想重做。**Tasks 的价值就是"实时状态"**。

**AI 助手支持时**：让 AI 每完成一个任务自动改 `[ ] → [x]`（这是 AI 的职责，不是你的）。

### Q7：偏离记录到底怎么写？

**格式建议**：

```markdown
## 偏离记录

| 日期 | 偏离点 | 原因 | 建议 spec 修订 |
|------|--------|------|----------------|
| 2026-01-20 | Plan 只声明改 5 个文件，实际改了 7 个（多了 utils/logger.ts） | 实施时发现日志工具缺少 masking，必须加 | 无需修 spec，加到 plan 的"意外新增文件"章节 |
| 2026-01-21 | FR-3 要求"5 秒内响应"，实测 8 秒 | 后端瓶颈超预期 | **建议 spec 修订**：非功能需求改为"10 秒内"，或本 spec 拆 FR-3 到下一 spec |
```

**关键**：偏离本身不可怕，**要点是让偏离显性可见**。

### Q8：多个人同时改同一个 spec 会冲突吗？

**会**——用 Git 常规冲突处理即可。

**减少冲突的实践**：

- 大需求拆多个 spec（多 spec 共享 Story ID）
- 每个 spec 一位主编，其他人以评审身份留 comment
- 在多人协作前先在 IM / 群内明确"谁改哪块"

### Q9：CI 上怎么校验？

推荐 CI 加以下校验：

```bash
# 1. Commit 消息必须带 --story=
git log --format='%s' origin/main..HEAD | grep -q '\-\-story=' || exit 1

# 2. MR 描述必须引用 spec 路径
grep -qE 'specs/v[0-9.]+/[0-9]+-' <MR-description> || exit 1

# 3. 若 tasks 未全部勾选，禁止合并（可选严格模式）
grep -c '\[ \]' tasks/**/<spec>-tasks.md
```

具体实现见 [06 落地你自己的项目](./06-adaptation.md) 的 CI 章节。

### Q10：一份 spec 卡在 draft 太久怎么办？

**信号**：spec 起草超过 3 天还没能 `Status: ready`。

**原因通常有三**：

1. TBD 太多，等业务方回答 → **升级到会议同步**
2. 涉及方案分歧 → **应该回到阶段 0 前，走 `/spec-design` 评审方案**
3. 需求本身不成熟 → **spec 撤回，改回 intake 阶段继续讨论**

**不要** 让 spec 悬着——半成品 spec 是最大的负担。

---

## 五、AI 助手集成建议

### 5.1 Cursor / Cline / Codebuddy 等支持规则文件的助手

在项目根加 `.cursorrules` / `CLAUDE.md` / `codebuddyRules` 等（视工具）：

```
项目使用 CoSpec 范式。请遵循 rules/ 下所有规则文件。
所有需求 → 磁盘 spec；未经人审的 spec 不允许进入实施。
AI 只有执行权，无定义权；遇到 spec 不清晰必须问，不能编造。
命令定义在 commands/ 下，可用 slash command 触发。
```

### 5.2 支持 slash command 的助手

把 `commands/*.md` 注册为 slash command。当用户输入 `/spec-draft` 时，AI 自动加载对应命令文件的执行步骤。

### 5.3 不支持 slash command 的助手

改用触发词：

```
你：请按 commands/spec-draft.md 的流程为我起草...
```

### 5.4 一份好用的 `CLAUDE.md`

见项目根目录的 `CLAUDE.md`——建议按需修改后放在 AI 助手识别的位置。

---

## 六、上手起步的 15 分钟

**如果你只有 15 分钟，就做这些**：

1. **5 分钟**：读 [01 全景总览](./01-overview.md)
2. **5 分钟**：跑一遍场景 A（极简需求）示例
3. **5 分钟**：熟悉命令清单、目录结构

**接下来的 1 小时**：

- 挑一个真实的小需求
- 走完 draft → 人审 → implement → sync
- 感受一遍完整流程

**接下来的 1 周**：

- 至少走 3 个中等需求
- 逐步引入 design / plan / tasks
- 团队约定 CI 校验规则

**接下来的 1 个月**：

- 团队全员采用 CoSpec
- 收集使用反馈，定期调整规则
- 优化 AI 助手集成

---

## 七、下一步

- 想理解目录 / 文件契约每一处的细节 → [05 目录与文件契约](./05-architecture.md)
- 想把 CoSpec 用到自己的项目上 → [06 落地你自己的项目](./06-adaptation.md)
