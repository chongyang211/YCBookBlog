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
  name: xugaoyi
  link: https://github.com/xugaoyi
---
# Git 从入门到精通 · 实践驱动课程方案

## 一、课程设计理念

**"用中学，错了再纠"**。本课程不做枯燥的概念堆砌，而是以 **30+ 实战任务** 贯穿全程——每学一个知识点，立刻在终端动手验证。从一个人写代码到百人团队协作，从手忙脚乱丢代码到游刃有余玩分支，全程可复现。

---

## 二、课程大纲（8 章，循序渐进）

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

## 三、学习路线建议

```
第1章（1h）→ 第2章（1.5h）→ 第3章（2h）→ 第4章（2.5h）
    ↓
第5章（2h）→ 第6章（2h）→ 第7章（2h）→ 第8章（1.5h）
```

- **零基础**：按顺序学，每章跟做实践任务。
- **有基础**：从第4章（分支）开始，重点攻克第6章（高级技巧）和第7章（工作流）。
- **查漏补缺**：直接跳第8章，场景速查。

---

## 四、每章配套

每章的详细内容会以独立文章展开，每个知识点配 **可复制的命令 + 截图 + 常见踩坑提醒**。对应文章列表：

| 编号 | 文章 | 对应章节 |
|---|---|---|
| 01 | Git 实用技巧指南 | 第1-2章（基础速览） |
| 02 | 代码提交和推送 | 第2-3、5章 |
| 03 | 代码撤销的操作 | 第3章 |
| 04 | 合并分支的操作 | 第4章 |
| 05 | 选择性移植提交 | 第6章（cherry-pick） |
| 06 | 分支常见的操作 | 第4章 |
| 07 | 删除分支的操作 | 第4、8章 |
| 08 | TAG 标签的操作 | 第5章 |
| 09 | stash 暂存工作现场 | 第6章（待补充） |
| 10 | Git 工作流实战 | 第7章（待补充） |

---

## 五、核心心法（贯穿全程）

1. **Git 只有三个区域**：工作区、暂存区、版本库。一切命令都在这三者之间搬运数据。
2. **分支只是一个指针**：创建分支只是新建了一个可移动的标签，毫秒级完成。
3. **不要害怕犯错**：只要 commit 过，就能用 `reflog` 找回来。Git 是后悔药最多的工具。
4. **先 fetch 再决定 merge 还是 rebase**：永远不要盲目 `git pull`。
5. **Commit 要小要频繁**：一次 commit 只做一件事，方便回退和 cherry-pick。
