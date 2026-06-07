---
title: README
date: 2025-06-23 18:55:17
permalink: /pages/36ec62/
categories:
  - book
  - 通用技术的提升
  - Git从入门精通
tags:
  - 
author: 
  name: 杨充
  link: https://github.com/xugaoyi
---
# Git 从入门到精通 · 实践驱动课程方案

## 一、课程设计理念

**"用中学，错了再纠"**。本课程不做枯燥的概念堆砌，而是以 **30+ 实战任务** 贯穿全程——每学一个知识点，立刻在终端动手验证。从一个人写代码到百人团队协作，从手忙脚乱丢代码到游刃有余玩分支，全程可复现。

---

## 二、课程大纲（9 章，循序渐进）

### 第1章 · Git 是什么 & 为什么诞生 （约1小时）
**目标**：理解版本控制的本质，知道 Git 解决了什么问题。

| 节 | 内容 | 实践 |
|---|---|---|
| 1.1 | 没有版本控制的日子：文件命名灾难（`方案_v1_最终版_真的最终.doc`） | 动手：手动模拟三次"保存版本"，体会痛苦 |
| 1.2 | 集中式 vs 分布式：SVN 为什么被 Git 取代 | 图解：SVN 需要连服务器，Git 本地即完整仓库 |
| 1.3 | Git 的诞生：Linus 用两周写出 Git 的背后故事 | 任务：`git --version` 确认安装 |
| 1.4 | Git 不是 GitHub：分清工具和平台 | 对比图 |

**实践任务**：安装 Git → 配置用户名和邮箱 → 验证配置

```bash
git config --global user.name "你的名字"
git config --global user.email "你的邮箱"
git config --list
```

---

### 第2章 · 核心概念：三个区域 & 四种状态 （约1.5小时）

**目标**：彻底搞懂 Git 的底层模型，之后所有操作都有理论支撑。

| 节 | 内容 | 实践 |
|---|---|---|
| 2.1 | 三个区域：工作区 → 暂存区 → 版本库 | 图解 + 动手创建第一个 repo |
| 2.2 | 四种状态：未跟踪、已修改、已暂存、已提交 | `git status` 反复切换观察 |
| 2.3 | .git 目录里藏着什么：objects、HEAD、refs | `ls .git` 一探究竟 |
| 2.4 | 一次 commit 的底层发生了什么：blob → tree → commit | 用 `git cat-file` 追踪一个对象 |

**实践任务**：创建项目 → 修改文件 → `add` → `commit` → 用 `git log` 和 `git show` 查看全过程。

```bash
mkdir my-git-lab && cd my-git-lab
git init
echo "Hello Git" > README.md
git add README.md
git commit -m "first commit"
git log --oneline
```

---

### 第3章 · 单人工作流：复盘、撤销、回退 （约2小时）
**目标**：一个人写代码时，自由穿梭于任意历史版本。

| 节 | 内容 | 实践 |
|---|---|---|
| 3.1 | 查看历史：`git log` 的花式用法 | `log --oneline --graph --all` |
| 3.2 | 比较差异：`git diff` 的三个场景（工作区/暂存区/版本间） | 修改文件 → diff → add → diff --cached |
| 3.3 | 撤销工作区修改：`git restore` / `git checkout` | 故意写错 → 一键还原 |
| 3.4 | 撤销暂存区：`git restore --staged` | add 错了 → 撤回 |
| 3.5 | 回退版本：`git reset --soft / --mixed / --hard` | 三个模式分别演练 |
| 3.6 | 后悔药：`git reflog` 找回"丢失"的提交 | 模拟一次 hard reset 后抢救 |

**实践任务**：做 5 次提交 → 用 `reset --hard` 回退两次 → 用 `reflog` 找回"丢失"的那两次。

---

### 第4章 · 分支：Git 的灵魂 （约2.5小时）
**目标**：理解分支原理，掌握日常分支操作。

| 节 | 内容 | 实践 |
|---|---|---|
| 4.1 | 分支的本质：一个指向 commit 的指针 | `cat .git/refs/heads/main` |
| 4.2 | 创建与切换：`branch`、`switch`、`checkout` | 创建 feature 分支 |
| 4.3 | 合并分支：`merge` 的两种模式（fast-forward / 三方合并） | 模拟两次 merge |
| 4.4 | 合并冲突：产生原因 → 手动解决 → 标记完成 | 故意制造冲突 → 解决 |
| 4.5 | 变基：`rebase` 的原理与黄金法则 | 模拟 rebase → 对比 merge 的区别 |
| 4.6 | 交互式 rebase：合并提交、修改历史信息 | `rebase -i` 实战 |
| 4.7 | `reset` vs `revert` vs `rebase`：三者的区别和使用场景 | 对比表格 + 各自演练 |
| 4.8 | 最佳实践：何时 merge，何时 rebase | 项目规范建议 |

**实践任务**：创建一个 feature 分支 → 做 3 个提交 → 切回 main 再做 2 个提交 → merge feature 分支（解决冲突）。

---

### 第5章 · 远程协作：GitHub/GitLab 实战 （约2小时）
**目标**：从单机 Git 走向多人协作。

| 节 | 内容 | 实践 |
|---|---|---|
| 5.1 | 远程仓库概念：`remote`、`origin`、`upstream` | 在 GitHub 创建仓库 |
| 5.2 | 推送与拉取：`push`、`pull`、`fetch` | push 到远程 |
| 5.3 | 克隆仓库：`clone` 的三种协议（HTTPS/SSH/Git） | clone 自己的仓库 |
| 5.4 | `pull` vs `fetch + merge`：为什么建议 fetch 后再 merge | 对比实验 |
| 5.5 | Pull Request（PR）工作流：Fork → Branch → PR → Review → Merge | 一人分饰两角：发起 PR → 审查 → 合并 |
| 5.6 | 多人协作冲突：两个人同时改同一个文件 | 两台终端模拟：push 被拒 → pull → 解决冲突 → 再 push |
| 5.7 | 标签管理：轻量标签 vs 附注标签，发布版本 | `git tag v1.0.0` |

**实践任务**：创建远程仓库 → 本地开发并推 5 个 commit → 用另一台终端 clone → 各自修改 → 模拟 PR 流程。

---

### 第6章 · 高级技巧：cherry-pick、stash、bisect （约2小时）
**目标**：掌握"特种作战"能力，应对复杂场景。

| 节 | 内容 | 实践 |
|---|---|---|
| 6.1 | `stash`：临时保存工作现场 | 改了一半被叫去修 bug → stash → 切分支 → 修完 → 切回来 → pop |
| 6.2 | `cherry-pick`：精准摘取一个提交到另一分支 | 把 hotfix 分支的一个 commit 移植到 main |
| 6.3 | `bisect`：二分法快速定位引入 bug 的提交 | 故意引入一个 bug → `git bisect` 找出 |
| 6.4 | 子模块 `submodule`：管理项目中的第三方依赖 | 场景介绍 + 基本操作 |
| 6.5 | `git blame`：追溯每一行代码是谁写的 | `git blame README.md` |
| 6.6 | `git reflog` 高阶：30 天后悔药，找回一切 | 模拟"灾难"后用 reflog 恢复 |

**实践任务**：模拟"开发中被叫去修紧急 bug"场景：stash → 切分支 → cherry-pick → 切回 → stash pop。

---

### 第7章 · 团队工作流实战 （约2小时）
**目标**：学会企业级 Git 协作模式。

| 节 | 内容 | 实践 |
|---|---|---|
| 7.1 | Git Flow：`main / develop / feature / release / hotfix` | 完整模拟一次 Git Flow 发布流程 |
| 7.2 | GitHub Flow：简化版，`main + feature + PR` | 实践 Pull Request 全流程 |
| 7.3 | 保护分支：禁止直接 push main，必须 PR 审查 | GitHub 设置 → 本地违规 push → 被拒绝 |
| 7.4 | Code Review 技巧：如何写好 PR 描述、如何审查代码 | 真实案例讲解 |
| 7.5 | CI/CD 集成：Git 提交如何触发自动化流水线 | GitHub Actions 示例 |
| 7.6 | `.gitignore` 艺术：忽略规则、全局忽略、已跟踪文件处理 | 配置项目 .gitignore |
| 7.7 | Commit 规范：Conventional Commits（`feat:`/`fix:`/`docs:`） | 为之前的提交重写 message |

**实践任务**：完整模拟 Git Flow：创建 develop → 从 develop 开 feature 分支 → 开发 → 提 PR → 合并回 develop → 发 release → 合并到 main → 打 tag。

---

### 第8章 · 故障排除 & 常见问题 （约1.5小时）
**目标**：遇到 Git 报错不再慌，能独立排错。

| 节 | 内容 | 实践 |
|---|---|---|
| 8.1 | `detached HEAD` 是什么、怎么处理 | 故意进入 → 脱离 |
| 8.2 | `merge conflict` 的 5 种场景及解决策略 | 逐场景演练 |
| 8.3 | 误删分支的恢复 | 模拟删除 → `reflog` 恢复 |
| 8.4 | 误提交大文件 → 清理历史 | `git filter-branch` / `BFG` |
| 8.5 | `git pull` 报"unrelated histories" | 场景 + 解决 |
| 8.6 | SSH 密钥配置失败排查 | 从生成密钥到添加 GitHub |
| 8.7 | Git 思维速查表：20+ 场景 → 对应命令 | 速查卡片 |

---

### 第9章 · 常见实操速查 （本章集成已有文章）
**目标**：将分散的常用操作按"遇到什么场景 → 用什么命令"组织成速查地图。遇到问题不用翻遍文档，直接对号入座。

#### 9.1 撤销类操作（来自 [03.代码撤销的操作](other/03.代码撤销的操作.md)）

| 场景 | 命令 | 关键说明 |
|---|---|---|
| 改乱了文件，还没 `add` | `git checkout -- <file>` | 一键还原到上次 commit 的状态 |
| `add` 错了，想撤回暂存区 | `git reset HEAD <file>` | 文件回到已修改状态，不丢内容 |
| `commit` 了但没 `push`，想撤销 | `git reset --soft HEAD^` | 修改回到暂存区，commit 消失 |
| `commit` 想彻底丢弃 | `git reset --hard HEAD^` | ⚠️ 修改全部丢失，慎用 |
| 已经 `push` 了，想撤销远程 | `git revert <commit-hash>` | 生成新 commit 反向抵消，不改历史 |
| 提交信息写错了 | `git commit --amend -m "新信息"` | 仅改 message，不涉及代码 |
| 漏了文件想补进上次提交 | `git add <file> && git commit --amend` | 追加文件到上一次 commit |
| 正在 merge 中想放弃 | `git merge --abort` | 回到 merge 前的干净状态 |
| 正在 rebase 中想放弃 | `git rebase --abort` | 回到 rebase 前的干净状态 |
| stash 了想恢复 | `git stash pop` | 恢复并删除 stash；`apply` 不删除 |
| 回到某个历史版本看看 | `git checkout <commit-hash>` | 进入 detached HEAD，看完切回来 |
| 强制推送（万不得已） | `git push --force` | ⚠️ 覆盖远程历史，团队协作禁用 |

#### 9.2 合并分支操作（来自 [04.合并分支的操作](other/04.合并分支的操作.md)）

| 场景 | 命令 | 关键说明 |
|---|---|---|
| 标准合并（feature → develop） | `git checkout develop && git merge feature` | 先切到目标分支再 merge |
| 推荐：非快进合并 | `git merge --no-ff feature` | 保留功能分支完整历史 |
| 合并前先同步 | `git pull origin develop` | 减少冲突，务必先拉最新 |
| 查看分支差异 | `git diff develop..feature` | 合并前先看清楚改了啥 |
| 解决冲突三步走 | `git status` → 手动修 → `git add` → `git commit` | 修完冲突记得 add + commit |
| 合并后推送 | `git push origin develop` | 推送合并结果 |
| 合并后清理 feature 分支 | `git branch -d feature && git push origin --delete feature` | 本地 + 远程一并删 |
| 撤销已推送的合并 | `git revert -m 1 <merge-hash>` | 生成反向提交，安全撤销 |
| 撤销未推送的合并 | `git reset --hard HEAD~1` | 直接回退一步 |

**合并策略对比：**

| 策略 | 命令 | 适用场景 | 效果 |
|---|---|---|---|
| 标准合并 | `git merge feature` | 小型功能 | 快但历史不清 |
| `--no-ff` | `git merge --no-ff feature` | **推荐日常** | 保留完整分支历史 |
| rebase | `git rebase develop` | 保持线性历史 | 整洁但可能丢分支信息 |
| squash | `git merge --squash feature` | 简化历史 | 所有提交压成一个 |

#### 9.3 cherry-pick 精准移植（来自 [05.选择性移植提交](other/05.选择性移植提交.md)）

| 场景 | 命令 | 关键说明 |
|---|---|---|
| 移植单个提交 | `git cherry-pick <hash>` | 把别的分支的某个 commit "摘"到当前分支 |
| 移植连续多个 | `git cherry-pick <hashA>..<hashB>` | 范围移植 |
| 移植不连续多个 | `git cherry-pick <hash1> <hash2>` | 空格分隔多个 hash |
| 编辑提交信息 | `git cherry-pick -e <hash>` | 移植时顺便改 message |
| 只取改动不自动提交 | `git cherry-pick -n <hash>` | 取过来先放着，手动改完再 commit |
| 遇到冲突怎么办 | 手动解决 → `git cherry-pick --continue` | 同 merge 冲突处理 |
| 放弃移植 | `git cherry-pick --abort` | 回到操作前状态 |

> **什么时候用 cherry-pick 而不是 merge？** 单个 bug 修复想同步到多个分支（如 hotfix → main + develop），用 cherry-pick。整个功能完整合并用 merge。

#### 9.4 分支重命名（来自 [06.分支常见的操作](other/06.分支常见的操作.md)）

| 场景 | 命令 |
|---|---|
| 重命名本地分支 | `git branch -m <旧名> <新名>` |
| 删除远程旧分支 | `git push origin --delete <旧名>` |
| 推送新分支到远程 | `git push origin <新名>` |
| 更新本地追踪关系 | `git branch --set-upstream-to=origin/<新名> <新名>` |

#### 9.5 删除分支（来自 [07.删除分支的操作](other/07.删除分支的操作.md)）

| 场景 | 命令 | 关键说明 |
|---|---|---|
| 删除本地分支 | `git branch -d <分支名>` | 已合并的才能删 |
| 强制删除本地分支 | `git branch -D <分支名>` | ⚠️ 未合并也能删，内容丢失 |
| 删除远程分支 | `git push origin --delete <分支名>` | 等效 `git push origin :<分支名>` |
| 同步远程删除（其他人执行） | `git fetch --prune` | 清理本地已不存在的远程分支引用 |

#### 9.6 标签操作（来自 [08.TAG标签的操作](other/08.TAG标签的操作.md)）

| 场景 | 命令 | 关键说明 |
|---|---|---|
| 创建轻量标签 | `git tag v1.0.0` | 只标记 commit，无额外信息 |
| 创建附注标签（推荐） | `git tag -a v1.0.0 -m "发布 v1.0.0"` | 含作者、日期、message |
| 查看所有标签 | `git tag` | `git tag -l "v1.*"` 按模式过滤 |
| 查看标签详情 | `git show v1.0.0` | 显示标签指向的 commit 信息 |
| 推送单个标签 | `git push origin v1.0.0` | push 默认不推送标签 |
| 推送所有标签 | `git push origin --tags` | 一次性全推 |
| 删除本地标签 | `git tag -d v1.0.0` | |
| 删除远程标签 | `git push origin :refs/tags/v1.0.0` | 冒号语法 |
| 切换到标签 | `git checkout v1.0.0` | 进入 detached HEAD |
| 基于标签建分支 | `git checkout -b hotfix-v1 v1.0.0` | 从标签处开新分支修复 |

> **标签 vs 分支**：标签是静态的"里程碑"，不会随提交移动；分支是动态的"线"，会不断前进。发布版本用标签，日常开发用分支。

#### 9.7 高频场景组合拳

| 日常场景 | 完整操作序列 |
|---|---|
| **早上来第一件事** | `git checkout develop && git pull origin develop` |
| **开始一个新功能** | `git checkout -b feature/xxx develop` |
| **开发中每天同步** | `git checkout feature/xxx && git merge develop` |
| **功能完成提 PR** | `git push origin feature/xxx` → GitHub 提 PR → 审查 → merge |
| **hotfix 紧急修复** | `git checkout -b hotfix/xxx main` → 修 bug → `git push` → PR 合并到 main + develop |
| **发布新版本** | 合并到 main → `git tag -a v1.2.0 -m "v1.2.0"` → `git push origin main --tags` |
| **合并完删分支** | `git branch -d feature/xxx && git push origin --delete feature/xxx` |
| **改了一半被叫去修 bug** | `git stash` → 切分支修 → 修完提交 → 切回来 → `git stash pop` |
| **提交错了想反悔** | 未 push：`git reset --soft HEAD^`；已 push：`git revert <hash>` |

---

## 三、学习路线建议

```
第1章（1h）→ 第2章（1.5h）→ 第3章（2h）→ 第4章（2.5h）
    ↓
第5章（2h）→ 第6章（2h）→ 第7章（2h）→ 第8章（1.5h）
    ↓
第9章（速查，随时翻阅）
```

- **零基础**：按顺序学，每章跟做实践任务。
- **有基础**：从第4章（分支）开始，重点攻克第6章（高级技巧）和第7章（工作流）。
- **日常查漏**：直接翻第9章速查表，按场景找命令。
- **遇到报错**：跳第8章，场景速查。

---

## 四、每章配套

每章的详细内容以独立文章展开，每个知识点配 **可复制的命令 + 截图 + 常见踩坑提醒**。对应文章列表：

| 编号 | 文章 | 对应章节 |
|---|---|---|
| 01 | 版本控制诞生 | 第1章：Git 是什么 & 为什么诞生 |
| 02 | 单人工作流 | 第2章：log/diff/restore/reset/reflog |
| 03 | 分支的本质 | 第3章：merge/rebase/冲突/交互变基 |
| 04 | 远程协作指南 | 第4章：push/pull/fetch/PR/标签 |
| 05 | 特种作战手册 | 第5章：stash/cherry-pick/bisect/blame |
| 06 | 团队工作流 | 第6章：Git Flow/GitHub Flow/保护分支/CI |
| 07 | 故障排除手册 | 第7章：detached HEAD/误删/大文件/SSH |
| 08 | 场景速查地图 | 第8章：按场景速查 + 一天工作流 + 卡片 |
| 09 | 常见操作实践 | 第9章：提交/撤销/合并/cherry-pick/分支/标签 |

> **第8章** 将全部命令按"场景 → 命令 → 说明"整合为一张速查地图；**第9章** 将日常高频操作串联成可执行的操作清单，适合开发时随手翻阅。

---

## 五、核心心法（贯穿全程）

1. **Git 只有三个区域**：工作区、暂存区、版本库。一切命令都在这三者之间搬运数据。
2. **分支只是一个指针**：创建分支只是新建了一个可移动的标签，毫秒级完成。
3. **不要害怕犯错**：只要 commit 过，就能用 `reflog` 找回来。Git 是后悔药最多的工具。
4. **先 fetch 再决定 merge 还是 rebase**：永远不要盲目 `git pull`。
5. **Commit 要小要频繁**：一次 commit 只做一件事，方便回退和 cherry-pick。
