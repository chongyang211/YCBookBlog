# 06｜落地你自己的项目

> 📖 **本篇定位**：把 CoSpec 从"通用范式"落到"你的具体项目"——分为**克隆骨架、清洗业务耦合、按团队定制、CI 校验**四个部分。
> 🔗 **上一篇**：[05 目录与文件契约](./05-architecture.md) · **回到系列首页**：[README](./README.md)

---

## 一、落地方式的三种选择

### 方式 A：一份仓库同时承载"代码 + Spec 元数据"（推荐给小/中项目）

```txt
your-project/
├─ rules/ commands/ skills/ ...    ← CoSpec 骨架
├─ specs/ plans/ tasks/ ...        ← 元数据
├─ src/                             ← 业务代码
└─ tests/                           ← 测试
```

**优点**：一次 clone 全在手；spec 与代码天然同 commit 关联。
**缺点**：CoSpec 骨架的更新会与业务代码提交交织。

### 方式 B：独立 Spec 仓库 + 多个业务仓库（推荐给大团队 / 多微服务）

```txt
your-org/
├─ CoSpec/                          ← 只有元数据
│  ├─ rules/ commands/ skills/
│  ├─ specs/ plans/ tasks/ ...
│  └─ src/                          ← .gitignore，按需 clone 业务仓
│
├─ service-a/                       ← 业务仓
├─ service-b/                       ← 业务仓
└─ service-c/                       ← 业务仓
```

**优点**：CoSpec 骨架变更集中管理；跨仓库同名分支约定天然适配。
**缺点**：多一次 clone；spec 与代码非同仓，追溯需靠 Story ID。

### 方式 C：Monorepo 内的 CoSpec 子目录

```txt
your-monorepo/
├─ .cospec/                         ← 骨架 + 元数据集中
│  ├─ rules/ commands/ skills/
│  └─ specs/ plans/ tasks/ ...
├─ packages/
│  ├─ service-a/
│  ├─ service-b/
└─ tools/
```

**优点**：适合已有 monorepo 的团队；元数据与代码同仓。
**缺点**：目录路径变长；AI 助手加载 rules 需配置。

---

## 二、克隆骨架（3 步）

### Step 1：拷贝骨架文件

从本仓 `code/Spec/` 拷贝以下目录到你的项目：

```txt
需要拷贝：
├─ rules/                    ← 全部（5 份）
├─ commands/                 ← 全部（10 份）
├─ skills/                   ← 至少拷贝 8 个核心 skill：
│  ├─ spec-drafting/
│  ├─ spec-analysis/
│  ├─ codebase-survey/
│  ├─ implementation-planning/
│  ├─ feature-implementation/
│  ├─ test-writing/
│  ├─ change-summary/
│  └─ technical-design/
├─ specs/templates/          ← 模板
├─ plans/templates/          ← 模板
├─ tasks/templates/          ← 模板
├─ designs/templates/        ← 模板
├─ intake/templates/    ← 模板
├─ docs/spec-flow-overview.md
├─ docs/spec-coding-handbook.md
└─ CLAUDE.md（或对应 AI 助手的入口文件）
```

**不需要**拷贝的：

- ❌ `specs/v*/` 的实例文件（是别人的具体需求）
- ❌ `plans/v*/` `tasks/v*/`
- ❌ `intake/v*/`
- ❌ 与团队特定业务耦合的脚本 / skill（见 §3 清洗）

### Step 2：初始化空目录

```bash
mkdir -p specs/v1.0.0 plans/v1.0.0 tasks/v1.0.0 \
         intake/v1.0.0 designs/v1.0.0
```

`v1.0.0` 换成你项目的初始版本号。

### Step 3：AI 助手识别

**Cursor**：把 `CLAUDE.md` 改名为 `.cursorrules`，或在 Cursor 设置里指定读取路径。
**Codebuddy**：项目根放 `CLAUDE.md`（或 `codebuddyRules`）。
**Continue / Cline**：在其配置里加载 `rules/*.md`。
**其他**：查各自文档，找到"项目级 system prompt"的加载方式。

**验证**：在 AI 助手中输入 `/spec-draft`，看它是否识别命令。

---

## 三、清洗业务耦合（关键步骤）

CoSpec 的骨架里可能有一些**与原项目业务耦合**的内容。落到你项目前必须清洗。

### 3.1 常见业务耦合点

| 位置                                          | 耦合点                            | 清洗方式                                                                 |
| --------------------------------------------- | --------------------------------- | ------------------------------------------------------------------------ |
| `skills/*/scripts/*.sh`                       | 硬编码的 Git 仓库列表 / token 名 | 全部删除或改为**你自己的仓库映射**                                       |
| `rules/*.md`                                  | 特定业务术语 / 团队名             | 全文搜替换成你自己的                                                     |
| `docs/git-workflow.md`（若存在）              | Git 平台域名 / 基线分支           | 改为你团队约定                                                            |
| `CLAUDE.md`                                   | 项目名 / 团队 Wiki 链接           | 改为你自己的                                                              |
| 其他任何写死 URL / 路径的地方                 | 团队专属 URL                      | 用占位符 `<GIT-HOST>` `<TEAM-WIKI>` 替换                                 |

### 3.2 检查耦合的一次性命令

在拷贝完骨架后，跑一遍这些搜索：

```bash
# 找出所有 URL
grep -rE 'https?://[a-zA-Z0-9]' rules/ commands/ skills/ docs/

# 找出所有 Git 域名
grep -rE 'git@|\.git\b' rules/ commands/ skills/ docs/

# 找出中文商业术语（列出你的团队/产品名）
grep -r '<你的旧团队名>' rules/ commands/ skills/ docs/
```

**逐条评估**：这些引用是通用能力还是业务耦合？业务耦合的一律清洗。

### 3.3 清洗后的通用占位符建议

在骨架文档里保留占位符而非具体值：

```markdown
- Git 平台：`<GIT-HOST>`（如 gitlab.your-company.com / github.com）
- 组织：`<ORG>`
- 团队 Wiki：`<TEAM-WIKI-URL>`
- 需求管理系统：`<REQ-SYSTEM>`（如 Jira / 禅道 / Trello）
- 主基线分支：`<MAIN-BRANCH>`（如 main / master / develop）
```

---

## 四、按团队定制

### 4.1 定制 Story ID 来源

如果你的团队用 Jira：

- Story ID = Jira Issue Number（去掉前缀）
- 如 `PROJ-12345` → Story ID = `12345`

如果用禅道 / 飞书任务 / 其他：

- 取该系统的 Issue 唯一 ID
- 保持纯数字（若有前缀，去掉）

如果**没有需求管理系统**：

- 用日期 + 序号：如 `20260115001`
- 或从 1 开始自增（但要在 CoSpec 内维护"当前最大 ID"，避免冲突）

**修改点**：在 `rules/10-spec-workflow.md` 和 `commands/spec-draft.md` 里的"Story ID 说明"章节改写为你团队约定。

### 4.2 定制版本号策略

CoSpec 的 `<VERSION>/` 目录代表迭代版本。可选策略：

| 策略                 | 例子              | 适合场景                          |
| -------------------- | ----------------- | --------------------------------- |
| SemVer               | `v1.0.0`          | 产品发布节奏明确                  |
| 迭代号               | `sprint-24`       | 敏捷 Sprint                       |
| 季度                 | `2026-Q1`         | 长周期项目                        |
| 月份                 | `2026-01`         | 月度迭代                          |
| 单一版本             | `v1/`             | 不需要归档，全部放一个目录        |

选一种告知团队，写在 `CLAUDE.md` 里。

### 4.3 定制评审角色

CoSpec 默认"其他技术同事评审 spec"。可以细化为：

```markdown
## Spec 评审规则

- 常规 spec：任一其他技术同事评审即可
- 涉及数据模型：**必须** DBA 评审
- 涉及安全：**必须** 安全组评审
- 大需求（Design 阶段）：架构师 + Tech Lead + 相关方评审

评审通过标志：在 spec 的 "修订记录" 追加一行 review 通过说明。
```

写在 `rules/10-spec-workflow.md` 的"阶段 1 卡口"章节。

### 4.4 定制 Commit / Branch 命名

如果你团队已有 commit 规范（如 Conventional Commits 但格式略不同），可以调整：

```markdown
## 我们团队的 commit 规范

<type>(<scope>): <subject>

Story: <STORYID>

其中 type 使用：feat / fix / chore / docs / test / refactor
scope 使用：模块名（如 auth / payment / ui）
最后一笔 commit 在正文加 [#finish]
```

写在 `docs/git-workflow.md`。

### 4.5 定制技术栈相关规则

如果你的项目是纯 Python：

```markdown
## Python 项目特定规则

- 命名：snake_case（函数 / 变量）、PascalCase（类）
- 类型标注：Python 3.10+ 强制加类型标注
- 测试框架：pytest
- 测试命名：test_<模块>_<场景>
```

写在 `rules/20-coding-rules.md` 附加章节。

---

## 五、CI 校验（把铁律变成机器可执行）

铁律的 10 条约束，很多可以用 CI 机器校验。以下是通用样例（用 GitHub Actions，可移植到 GitLab CI / Jenkins）。

### 5.1 校验 Commit 消息带 Story ID

```yaml
# .github/workflows/spec-check.yml
name: CoSpec Check
on: [pull_request]

jobs:
  commit-message:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          fetch-depth: 0
      - name: Check --story= in all commits
        run: |
          BASE_SHA=$(git merge-base origin/${{ github.base_ref }} HEAD)
          MISSING=$(git log --format='%H %s' $BASE_SHA..HEAD | grep -v -- '--story=' || true)
          if [ -n "$MISSING" ]; then
            echo "以下 commit 缺少 --story=<ID>："
            echo "$MISSING"
            exit 1
          fi
```

### 5.2 校验 MR 描述引用 Spec

```yaml
  mr-description:
    runs-on: ubuntu-latest
    steps:
      - name: Check spec reference
        env:
          PR_BODY: ${{ github.event.pull_request.body }}
        run: |
          if ! echo "$PR_BODY" | grep -qE 'specs/v[0-9.]+/[0-9]+-'; then
            echo "PR 描述必须引用 spec 路径（specs/v*.*.*/12345-...md）"
            exit 1
          fi
```

### 5.3 校验 Tasks 全部勾选（严格模式，可选）

```yaml
  tasks-completeness:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Find matching tasks file
        run: |
          STORY_ID=$(echo "${{ github.head_ref }}" | grep -oE '[0-9]+' | head -1)
          if [ -z "$STORY_ID" ]; then
            echo "分支名不含 Story ID，跳过校验"
            exit 0
          fi
          TASKS=$(find tasks -name "${STORY_ID}-*-tasks.md" | head -1)
          if [ -z "$TASKS" ]; then
            echo "未找到对应 tasks 文件，跳过"
            exit 0
          fi
          UNCHECKED=$(grep -c '\[ \]' "$TASKS" || true)
          if [ "$UNCHECKED" -gt "0" ]; then
            echo "$TASKS 尚有 $UNCHECKED 个未勾选任务"
            exit 1
          fi
```

### 5.4 校验 Spec 状态

```yaml
  spec-status:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Find spec for this branch
        run: |
          STORY_ID=$(echo "${{ github.head_ref }}" | grep -oE '[0-9]+' | head -1)
          SPEC=$(find specs -name "${STORY_ID}-*.md" | head -1)
          if [ -z "$SPEC" ]; then
            echo "未找到对应 spec，禁止合并"
            exit 1
          fi
          STATUS=$(grep -oE 'Status:.*' "$SPEC" | head -1)
          if echo "$STATUS" | grep -q 'draft\|deprecated'; then
            echo "Spec 状态是 draft/deprecated，禁止合并：$STATUS"
            exit 1
          fi
```

### 5.5 校验分支命名

```yaml
  branch-name:
    runs-on: ubuntu-latest
    steps:
      - name: Check branch naming
        run: |
          BR="${{ github.head_ref }}"
          if ! echo "$BR" | grep -qE '^(feature|hotfix)/[0-9]+-[a-z0-9-]+$'; then
            echo "分支名不合规：$BR"
            echo "应为 feature/<STORYID>-<slug> 或 hotfix/<STORYID>-<slug>"
            exit 1
          fi
```

### 5.6 Pre-commit Hook 建议（本地保护）

在 `.git/hooks/commit-msg`（或用 husky / pre-commit 工具）：

```bash
#!/bin/bash
# 校验 commit 消息带 --story=

MSG_FILE="$1"
if ! grep -q -- '--story=' "$MSG_FILE"; then
    echo "❌ Commit 消息必须包含 --story=<ID>"
    echo "示例: feat(auth): add oauth --story=12345"
    exit 1
fi
```

---

## 六、团队推广路径

### 6.1 第 1 周：Pilot

- 挑 1-2 位主动的同事作为 pilot
- 用一个真实中等需求走完全流程
- 收集反馈：卡在哪、觉得繁琐的地方、觉得有价值的地方

### 6.2 第 2-3 周：小范围

- 扩展到 3-5 人
- 每周复盘一次
- 根据反馈调整 rules 和 templates（**这些应该是"活文档"**）

### 6.3 第 4 周起：全团队

- 全员切换
- CI 校验开启
- **旧 spec 不追溯改造**（新需求走新流程，避免大爆炸迁移）

### 6.4 3 个月后：审视回顾

- 度量：需求平均周期、返工率、AI 幻觉合并事故数
- 若指标改善明显 → 固化规则
- 若某些规则形同虚设 → 讨论是否精简

---

## 七、避坑清单

### 7.1 不要一次性上线全部铁律

**误区**：一天之内推 10 铁律 + 8 阶段 + 4 目录 → 团队爆炸。

**做法**：**分阶段推**：

- 第一阶段：只推"spec 磁盘化 + AI 不改 spec + 人审"
- 第二阶段：加"tasks 实时勾选 + 偏离记录"
- 第三阶段：加 commit / branch / CI 校验

### 7.2 不要把 CoSpec 神圣化

**信号**：团队开始为了"符合 CoSpec"而写 spec，而不是"为了协作"而写 spec。

**对策**：定期问"这条规则实际帮了我们什么"，帮不上的**大胆删掉**。

### 7.3 不要跳过 Pilot

**信号**：直接给全员看文档要求执行。

**对策**：一定要跑 1-2 个真实需求验证流程，找出 rules 里的写实性问题。

### 7.4 不要指望 AI 完全自动

**信号**：期待 AI 从头到尾自动跑完，不用人管。

**对策**：**CoSpec 的核心价值就是设人工卡口**——spec 评审、方案评审、review 都是人工。AI 只是降低人的工作量，不是替代人。

### 7.5 不要低估"落盘习惯"的心理成本

**信号**：团队反弹"以前一句话的事，现在要写 spec"。

**对策**：

- 强调"5 行 spec 也是 spec，模板 90% 空着也没关系"
- 展示"半年后回溯的省时价值"
- Pilot 结果做成 demo（"看看这个 spec，是不是比翻 IM 记录快"）

---

## 八、快速自检清单

落地 CoSpec 后，团队应能对以下问题回答"是"：

- [ ] 所有正在做的需求都有对应的 spec 文件
- [ ] 所有 spec 文件都通过人工评审
- [ ] 所有 commit 消息都带 `--story=<ID>`
- [ ] 所有 MR 描述都引用 spec 路径
- [ ] 所有 tasks 文件都实时勾选
- [ ] 所有偏离都记录在 tasks 的"偏离记录"章节
- [ ] AI 助手能识别 slash command（`/spec-draft` 等）
- [ ] CI 上有 CoSpec 校验（至少 commit 消息）
- [ ] 团队有共享的 `<VERSION>` 约定
- [ ] 半年后随便挑一个已实施 spec，能一键回溯所有产物

**如果 ≥ 8 条为"是"** → CoSpec 落地成功。

---

## 九、进一步扩展

### 9.1 扩展方向 1：自动化程度更高

- 用 AI 助手自动跑 `/spec-sync`（合并 MR 时触发）
- 用 CI 自动更新 spec status
- 用机器人生成 Story 索引页

### 9.2 扩展方向 2：与需求管理系统双向同步

- 从 Jira 拉需求 → 自动生成 intake
- Spec `Status: implemented` → 自动关闭 Jira Issue
- 需求变更 → 自动通知相关 spec 走"偏离回流"

### 9.3 扩展方向 3：多语言项目适配

如果项目是多语言（Python + Go + TypeScript），在 `rules/20-coding-rules.md` 里按语言分节，让 AI 判断当前编辑的是哪种语言并加载对应规则。

### 9.4 扩展方向 4：跨团队 CoSpec

多个团队用 CoSpec，可以共享一份 "CoSpec 骨架"仓（作为 git submodule），各自项目引用。骨架升级 → 各项目 `git submodule update` 一次生效。

---

## 十、总结

**CoSpec 的落地本质是三件事**：

1. **磁盘化**：把需求 / 计划 / 任务从对话搬到磁盘
2. **仪式化**：用命令链把关键步骤标准化
3. **机器化**：用 CI 把最容易忘的约束校验起来

**记住**：CoSpec 不是要束缚人，是要**让 AI 变成可管理的协作者**。

当你团队的每次 AI 协作都能被回溯、被评审、被复现——CoSpec 就成功了。

---

## 十一、回到系列

- [01 全景总览](./01-overview.md)
- [02 工作流详解](./02-workflow.md)
- [03 原理与思想](./03-principles.md)
- [04 使用手册](./04-how-to-use.md)
- [05 目录与文件契约](./05-architecture.md)
- **06 落地你自己的项目**（当前篇）
- [07 完整案例：短链接服务](./07-case.md)
