# Git 工作流（实施 / Push / Rebase）

> 适用范围：本工作流涉及的全部相关仓库（清单见下方基线分支映射表）  
> 与 Spec Coding 的关系：阶段三（implement）的前置；新增阶段六（push）  
> 详细规则见 `rules/10-spec-workflow.md`

---

## 1. 基线分支映射表

每个仓库都有一个**基线分支**（baseline）—— 团队约定的稳定主干，feature 从它拉、最终也合回它。

> 🤖 **机器可读的权威来源**：`skills/mr-spec-review/scripts/prepare-src.sh` 的 `REPOS` 映射。自动化评审据它同步 `src/` 业务仓；本表与该脚本保持一致，改动请同步两处。

| 本地目录（`src/`） | Git 地址 | 基线分支 |
|------|---------|---------|
| `<迁移工具仓库>` | <GIT-HOST>/<ORG>/<迁移工具仓库> | `[TBD]` |
| `<业务主仓库>` | <GIT-HOST>/<ORG>/<业务主仓库> | `develop` |
| `<协议仓库>` | <GIT-HOST>/<ORG>/<协议仓库> | `master` |
| `<前端仓库>` | <GIT-HOST>/<ORG>/<子组>/<前端仓库> | `develop` |
| `<设备管理仓库>` | <GIT-HOST>/<ORG>/<子组>/<设备管理仓库> | `develop` |
| `<业务主仓库>` | <GIT-HOST>/<ORG>/<子组>/<业务主仓库> | `develop` |
| `proto` | <GIT-HOST>/<ORG>/<子组>/proto | `master` |
| `infrastructure` | <GIT-HOST>/<ORG>/<子组>/infrastructure | `develop` |
| `algorithm-repo` | <GIT-HOST>/<ORG>/<算法组>/algorithm-repo | `master` |
| `paymax_device` | <GIT-HOST>/<ORG>/O4/paymax_device | `develop` |
| `<支付仓库>` | <GIT-HOST>/<ORG>/O1/<支付仓库> | `develop` |
| `<激活服务仓库>` | <GIT-HOST>/<ORG>/O1/<激活服务仓库> | `develop` |
| `<IoT 服务仓库>` | <GIT-HOST>/<ORG>/O1/<IoT 服务仓库> | `develop` |
| `<IoT 服务仓库>` | <GIT-HOST>/<ORG>/<IoT 服务仓库> | `develop` |
| `<SDK 仓库>` | <GIT-HOST>/<ORG>/<SDK 仓库> | `master` |
| `<示例仓库>` | <GIT-HOST>/<ORG>/<示例仓库> | `master` |

> ⚠️ **识别侧仓库归并**：`<ORG>/<算法组>` 原有的 `<业务主仓库>` / `proto` 逻辑**已合并进** `<ORG>/<子组>` 的 `<业务主仓库>` / `proto`（`src/<业务主仓库>`、`src/proto`），原 `src/<旧流水线目录>/` 分组已废弃删除。识别算法仓 `algorithm-repo`（`<ORG>/<算法组>/algorithm-repo`）平铺在 `src/algorithm-repo`。
>
> ⚠️ `[TBD]` 的仓库需要团队补全。补全规则：基线分支 = 团队约定、所有 feature 都基于它拉、合并目标也是它。补全后请同步更新 `prepare-src.sh` 的 `REPOS`。

---

## 2. 三阶段 Git 流程

### 阶段 A：拉 feature 分支（实施前置，对应 `/spec-implement` Step 0.5）

每个**需要改动**的仓库（plan 「涉及仓库」表中标 ✅ 的）都执行：

```bash
# 1. 切到基线分支
git checkout <baseline>

# 2. 同步最新代码（rebase 模式，避免 merge commit 污染）
git pull -r origin <baseline>

# 3. 从基线分支拉出 feature 分支
git checkout -b {feature|hotfix}/<spec-name>
```

**例**（spec 0003）：

```bash
# <业务主仓库>
cd src/<业务主仓库>
git checkout develop
git pull -r origin develop
git checkout -b feature/10086-example-user-login

# <协议仓库>
cd src/<协议仓库>
git checkout master
git pull -r origin master
git checkout -b feature/10086-example-user-login

# <迁移工具仓库>（如改动）
cd src/<迁移工具仓库>
git checkout <baseline>
git pull -r origin <baseline>
git checkout -b feature/10086-example-user-login
```

> 💡 多仓库使用**同一分支名**，便于追溯关联（详见 `rules/10-spec-workflow.md` 分支命名规范）

---

### 阶段 B：实施期 Commit（对应 `/spec-implement` 实施过程）

实施过程中可以产生**多个 commit**，每个 commit 都遵循下面的 message 规范。但只有**最后一笔**带 `#finish`（合并后会关闭对应 story）。

#### Commit Message 规范

```
<type>(<scope>): <subject> --story=<STORYID> [#finish]
```

**字段**：

| 字段 | 必填 | 说明 |
|------|------|------|
| `<type>` | ✅ | 改动类型，见下表 |
| `<scope>` | ✅ | 模块名（小写），如 `auth` / `dashboard` / `gateway` / `proto` |
| `<subject>` | ✅ | 简洁的中文描述（&、/ 等符号可用） |
| `--story=<STORYID>` | ✅ | 关联Git 平台（GitHub / GitLab / 工蜂）需求单号（与 spec frontmatter Story ID 一致；无 story 用 `--story=0`） |
| `#finish` | 可选 | 合并 MR 时关闭关联 story；只在**本 story 的最后一笔 commit** 上加 |

**type 取值**（Conventional Commits 风格）：

| type | 用途 |
|------|------|
| `feat` | 新功能 |
| `fix` | bug 修复 |
| `refactor` | 重构（不改外部行为） |
| `perf` | 性能优化 |
| `docs` | 文档修改 |
| `test` | 测试代码 |
| `chore` | 杂项（构建、依赖、配置等） |
| `style` | 格式调整（不改逻辑） |

**示例**：

```
feat(auth): 首页面板新增示例业务关联数据&数据可配置 --story=10087 #finish
fix(gateway): 修复示例业务鉴权超时问题 --story=10086 #finish
refactor(controller): 抽取 lic 配额预扣公共方法 --story=0
docs(spec): 补充偏离回流流程说明 --story=0
```

**多 spec 拆分时的 `#finish` 使用**：

| 场景 | `#finish` 使用 |
|------|---------------|
| 单 spec 单 commit | 加 `#finish` |
| 单 spec 多 commit | 仅最后一笔加 `#finish` |
| 多 spec 共享 Story | 整个 Story 的最后一个 spec 的最后一笔加 `#finish`，其他不加 |

> ⚠️ 不确定时，宁可不加 `#finish` —— 后续可在Git 平台（GitHub / GitLab / 工蜂） UI 手动关闭 story；多加了反而会过早关闭。

---

### 阶段 C：Push 与安全 Rebase（验收通过后，对应新增的 `/spec-push` 命令）

**触发条件**：阶段五 review 通过 + 所有 task 已勾选 + spec 状态可改为 `implemented`。

**每个改动仓库都执行**：

```bash
# 1. 检查 working tree 干净
git status

# 2. 切回基线分支同步最新
git checkout <baseline>
git pull -r origin <baseline>

# 3. 切回 feature 分支 rebase 基线分支
git checkout {feature|hotfix}/<spec-name>
git rebase <baseline>

# 4. 解决冲突（如有）：
#    - 与冲突方沟通
#    - rebase 后必须重新跑测试
#    - 测试通过再继续

# 5. 强制 push（首次 push 用 -u；rebase 后用 -f）
git push -u origin {feature|hotfix}/<spec-name>      # 首次
git push -f origin {feature|hotfix}/<spec-name>       # rebase 后

# 6. 在Git 平台（GitHub / GitLab / 工蜂） UI 创建 MR
#    源分支 → 基线分支
#    描述自动套用 .gitlab/merge_request_templates/Default.md
```

#### 为什么要 rebase 而非 merge

| 选择 | 优点 | 缺点 |
|------|------|------|
| **rebase**（本规范） | 历史线性、合并时无冲突 | 需 force push（团队成员不能在 feature 分支并行 commit） |
| merge | 保留分支历史、不需 force push | 历史复杂、合并时可能再次冲突 |

**取舍依据**：本团队偏好线性历史 + 合并安全；feature 分支默认单人持有，多人协作通过拆 spec 实现。

#### 多仓库 Push 顺序

如有跨仓库依赖，按**被依赖优先**的顺序 push & 合并：

```
proto 仓库（如 <协议仓库>）→ 业务代码（如 <业务主仓库>）
```

每个仓库的 MR 在描述中互相引用：

```
跨仓库 MR:
  - <协议仓库>!123（先合）
  - <业务主仓库>!456（后合）
```

> ⚠️ 在 <协议仓库> MR 合并、stub 流水线生成完成后，<业务主仓库> 才能 `go get -u` 更新依赖、再 rebase + push。

---

## 3. 常见错误与自救

| 现象 | 原因 | 自救 |
|------|------|------|
| `git pull` 出现 merge commit | 没用 `-r`（rebase 模式） | 撤销：`git reset --hard origin/<baseline>`；下次记得 `git pull -r` |
| rebase 时大量冲突 | 基线分支跑得太前 | 提前 rebase（实施期间也定期同步）；冲突解决后必须**重新跑测试** |
| `push -f` 后队友报"分支被覆盖" | feature 分支非单人持有 | 沟通确认；如多人协作同一 spec，应拆为多个子 spec（每人独立分支） |
| 忘记加 `--story=` | 不符合规范 | `git commit --amend` 修正最后一笔；已 push 的 → rebase 改 message 后 force push |
| `#finish` 加错位置 | 过早关闭 story | Git 平台（GitHub / GitLab / 工蜂） UI 重新打开 story；下次注意只在最后一笔加 |
| 多仓库 rebase 时间差导致 stub 缺失 | proto 仓库没先合 | 严格按"被依赖优先"顺序合并；用 `replace` 指令本地联调，提交前移除 |

---

## 4. 自动化命令（建议团队封装）

可以把上述流程封装成 git alias 或脚本，减少重复劳动：

```bash
# .gitconfig 示例
[alias]
    sync-base = "!f() { git checkout $1 && git pull -r origin $1; }; f"
    safe-push = "!f() { base=$1; branch=$(git branch --show-current); git checkout $base && git pull -r origin $base && git checkout $branch && git rebase $base && git push -f; }; f"

# 使用
git sync-base develop                          # 同步基线
git safe-push develop                          # 安全 rebase + push
```

> 团队成员各自配置；也可统一沉淀到仓库的 `scripts/` 下作为 shell 脚本。

---

## 5. 相关文档

- `rules/10-spec-workflow.md` — 阶段三 / 阶段六完整规则
- `commands/spec-implement.md` — 实施命令（含 Step 0.5 拉分支）
- `commands/spec-push.md` — Push 命令（含安全 rebase）
- `.gitlab/merge_request_templates/Default.md` — MR 模板
