---
name: create-zerus
description: 创建 zerus 联调环境技能。当用户完成一次 spec 的代码改动、并把改动 push 到远端分支后，需要在 zerus 平台拉起一套联调 / 测试环境时使用。触发词：「拉一套 zerus」「起一个 zerus 环境」「create zerus」「联调环境」「新建 zerus」。skill 负责：① 通过 Playwright 弹出登录窗口拿 cookie；② 汇总本次改动涉及的服务与对应远端分支；③ 与用户确认 payload；④ 调用 zerus 后台 API 创建环境；⑤ 输出环境访问信息。
---

# 创建 zerus 联调环境（Create Zerus）

在一次 spec 的代码变更完成、并已 push 到远端 feature 分支后，为本次改动**一键拉起一套 zerus 联调 / 测试环境**，把改动的服务定向到本次的 feature 分支，其余服务走默认基线。

## 何时使用

- 用户说「拉一套 zerus」「起个 zerus 环境」「create zerus」「联调环境」「新建 zerus」。
- 一个 spec 已经 `/spec-implement` + `/spec-push` 完成，代码在远端分支上，需要联调验证。
- 想要一份带链接的联调环境，让测试 / 前端 / 产品直接进去看效果。

## 何时**不**使用

- 只是本地跑单测 / 编译（用 `test-writing` skill 或本地命令）。
- 代码还没 push 到远端（先走 `/spec-push`，本 skill 无法基于未推送的本地 commit 创建环境）。

## 修改已有环境（次要用途）

环境已经存在、只想**加/删服务、切分支、延期**时，走 §6 `update_env`（`references/API.md` §6，PUT 全量覆盖语义）。推荐用 `scripts/zerus_api.py` 里的：

```python
from zerus_api import (
    preflight, get_env_detail, update_env,
    build_service_instance, add_service_instance,
    remove_service_instance_by_name, update_service_branch_by_name,
    get_bk_params, extract_params_list,
)
```

⚠️ **必须先 `get_env_detail` 拉当前 env 再就地 modify 后 PUT**，否则漏带任何 instance 会被服务端当作"删除"。

## 删除已有环境（危险操作）

环境用完想**回收**（联调结束 / 建错了 / 清理过期）时，走 §7 `delete_env`
（`references/API.md` §7，`DELETE /envs/{env_id}` 空 body）。

```python
from zerus_api import preflight, get_env_detail, delete_env

cookie = preflight()
env = get_env_detail(cookie, env_id=6597)      # 先拉现状确认
# ⚠️ 必须跟用户二次确认：展示 name / hashName / creator / 服务列表
# ⚠️ prevent=true 时直接拒绝：if env.get("prevent"): raise ...
resp = delete_env(cookie, env["id"])
assert resp.get("code") == 0
```

⚠️ **删除不可撤销**且**推测非幂等**：skill 端**必须**在调用前跟用户明确二次确认；网络超时时**不自动重试**（避免第一次已删掉却重试拿到"env not found"误导用户）。

---

## 输入

| 参数 | 必选 | 来源 | 说明 |
|------|------|------|------|
| 涉及仓库 & feature 分支 | 是 | 从本会话上下文 / plan / git 现状收集 | 每个改动仓库对应的 `service` + `branch`，见 Step 2 |
| 环境名 (`name`) | 否 | 用户提供或自动生成 | 默认 `spec-<STORYID>-<slug>`（与当前会话 / spec 相关）；用户可覆盖 |
| 过期时间 (`endTime`) | 否 | 用户提供或自动生成 | 默认 **`now() + 7 days`**（UTC，ISO 8601 带 Z）；用户可指定其他有效期 |
| 备注 (`desc`) | 否 | 用户提供 | 建议填 spec 标题 |
| 首建触发 (`autoNew`) | 否 | 用户可覆盖 | **默认 `false`**（不立即首建，联调常规姿态）。仅当用户明确要求"我现在就要出镜像"时改为 `true`。⚠️ **与 Git push 自动触发无关** —— push 触发由 `bk.params[GIT_BRANCH_PROJECT].auto=true` 控制（skill 已强制开启，见 §4 组装规则） |

---

## 前置条件

1. 已完成 Python 环境准备，**推荐一键装（脚本会先打印步骤 + 磁盘影响，等用户确认）**：
   ```bash
   cd skills/create-zerus/scripts
   python3 bootstrap.py          # 交互确认；--yes 跳过确认；--dry-run 只打印
   source .venv/bin/activate
   ```
   等价手动步骤（bootstrap 内部就是这三条，且幂等）：
   ```bash
   python3 -m venv .venv && source .venv/bin/activate
   pip install -r requirements.txt
   playwright install chromium
   ```
   > ⚠️ **skill 主流程不会自动跑 `bootstrap.py`**（避免未经用户同意就下载几百 MB）；preflight 检测到依赖缺失时，会提示用户手动执行。
2. 本会话涉及的所有仓库均已 push 到远端（远端能通过分支名拉到最新提交）。
3. 用户具备访问 `https://zerus.woa.com/` 的权限。

---

## 执行步骤

### Step 0: 前置健康检查（Preflight）

在跑任何 API 之前，**先做三项检查**，任一失败**立即降级为"手动创建指引"**，不要继续往下走：

1. **依赖**：本机有 `curl` 且 `curl --version` 输出里含 `HTTP2`；`python3 -c "import playwright"`（登录流程需要）。
2. **网络**：`curl -sS -I --http2 --max-time 8 https://zerus.woa.com/` 能拿到 HTTP 状态码（301/302/401 都算通，只要 curl exit code = 0）。
3. **登录态**：`scripts/cookies.json` 存在、非空；打一个 `pageSize=1` 的 §1 请求验证 cookie 未被 302 到登录页。

**推荐直接调 `scripts/zerus_api.py` 的 `preflight()`**，它会依次跑上面 3 步，任一失败抛 `PreflightError`（`code` 见下表）：

| `PreflightError.code` | 含义 | 处理 |
|---|---|---|
| `curl_missing` | 找不到 curl 命令 | 提示装 curl（`brew install curl` / `apt install curl`） |
| `http2_unsupported` | curl 太旧不带 HTTP/2 | 提示升级 curl 到 7.47+ |
| `deps_missing` | 缺 `playwright` / `requests` | 提示用户跑 `python3 scripts/bootstrap.py`（会弹确认再装）；**不要自动装** |
| `network` | DNS/TCP/TLS 打不通 zerus | 提示接内网 / 连 VPN，然后走**手动 fallback** |
| `cookie_missing` | 没有 cookies.json | 提示先跑 `login_and_save.py`；服务器无图形环境时走**手动 fallback** |
| `cookie_expired` | cookie 已过期 | 提示重登；如果用户明确无法登录，走**手动 fallback** |

**手动 fallback 输出模板**（用 `zerus_api.render_manual_fallback()` 生成，或按下面结构手动写）：

```markdown
[!] zerus 自动化流程不可用，请改为手动在浏览器创建环境

原因：<code> —— <message>
建议：<hint>

zerus 控制台：https://zerus.woa.com/
项目：palm (projectId=71)
新建环境入口：控制台 → 左侧「联调环境」→「新建」

建议环境名：<Step 2 已算出的 env_name>
建议过期时间：<endTime，UTC>

需要覆盖的服务与分支：
  #   服务名                            分支
  --- -------------------------------  ----------------------------------------
  1   palmdiagnosis                     feature/134389917-...
  2   palm_local                        feature/134389917-...

操作步骤（在 zerus 页面）：
  1. 打开控制台 → 新建联调环境
  2. 填写环境名 / 过期时间 / 备注
  3. 添加上表中每个服务，按 GIT_BRANCH_PROJECT 覆盖为对应分支；其余未列出的服务保持默认基线
  4. 保存并等待构建触发
```

**降级时必须一次性把服务列表 + 分支交给用户**（哪怕 Step 2 只收集到部分），让用户拿去手动填 zerus 页面。

### Step 1: 获取 / 校验 zerus 登录 cookie

1. 检查 `skills/create-zerus/scripts/cookies.json` 是否存在、且**未过期**。
   - 过期判定：调用 API 时收到 401 / 302 到登录页 / HTML 含"登录" —— 归 Step 5 里做兜底。
2. 不存在或用户显式要求「重新登录」时，运行：
   ```bash
   python skills/create-zerus/scripts/login_and_save.py
   ```
   - 会弹出 Chromium 窗口打开 `https://zerus.woa.com/`
   - 用户完成 SSO / 二次验证后**脚本自动检测登录成功**（不再需要回终端按回车）
   - 脚本会写出 `cookies.json` 与 `state.json`（权限 `600`，不要 commit）
3. **不要**把 cookie / state 内容打印到对话或日志。

> ⚠️ 服务器 / 容器无图形环境时：让用户在本地机器跑 `login_and_save.py`，再把 `cookies.json` 拷贝到当前目录。

### Step 2: 汇总本次改动涉及的服务与分支

来源优先级（自动 → 半自动 → 询问）：

1. **优先** 从当前 spec 的 plan「涉及仓库」表读取：`specs/<VERSION>/<STORYID>-*.md` 关联的 `plans/<STORYID>-*-plan.md`。
2. **兜底** 扫描 `src/<repo>/` 下每个仓库的 git 状态，取当前分支名 + 最近一次 push 的 remote 分支（`scripts/collect_change_context.py`）。
3. **与 zerus 服务清单对齐**：调 `references/API.md` §1 的 `POST /api/v4/projects/71/services_query`，拿到该项目在 zerus 里已接入的服务列表，用 `service.name` 与 `src/<repo>/` 目录名匹配；未匹配上的仓库需人工确认（未接入 zerus，或名称不一致）。
4. **交叉核对** `docs/git-workflow.md` 里的 baseline 表，区分：
   - **变更仓库**（有 feature 分支、要在 zerus 里用 feature 分支覆盖默认）
   - **未变更仓库**（走 baseline / 线上版本）

对每个变更服务补齐 §3 参数模板（`references/API.md` §3）：
- `GET /api/v4/projects/71/services/{sid}/meta/{mid}/bk_params` → `params[]`
- 用 `GIT_BRANCH_PROJECT.aliasName` 反向校验：该 zerus service 确实对应本地 `src/<name>/` 的仓库；不匹配则**停止**并让用户核对

产出统一的 payload 草稿（skill 内部结构，最终会映射到 §4 body）：

```json
{
  "env_name": "spec-134389917-dashboard-online-palm-statistics",
  "story_id": "134389917",
  "end_time": "2026-07-20T07:17:00.000Z",
  "auto_new": false,
  "changes": [
    {
      "service_id": 711,
      "service_meta_id": 1394,
      "service_name": "palmdiagnosis",
      "repo": "palm/weixin/device_manage",
      "branch": "feature/134389917-dashboard-online-palm-statistics",
      "bk_params": "<从 §3 深拷贝得到的完整 params[]，GIT_BRANCH_PROJECT 已覆盖为上面 branch>"
    }
  ],
  "notes": "spec 0003 联调环境"
}
```

> ⚠️ **未变更仓库不进 `changes[]`**，走默认基线（不出现在 `serviceInstanceList` 里即可）。

### Step 3: 与用户确认 payload

**必须**把 Step 2 的 payload 草稿完整展示给用户，让其确认或修改：

```markdown
即将创建 zerus 环境（请确认）：

- 环境名 (name)：spec-134389917-dashboard-online-palm-statistics
- 过期时间 (endTime)：2026-07-20 15:17 (+08:00)  // 默认 7 天后
- 首建触发 (autoNew)：❎ `base.autoNew=false`（不立即首建）；Git push 自动触发已开启（`GIT_BRANCH_PROJECT.auto=true`）
- 备注 (desc)：spec 0003 联调环境
- 覆盖服务（{N} 个）：
  | # | 服务 (zerus) | 仓库 | feature 分支 | 远端已推送？ |
  |---|-------------|------|--------------|-------------|
  | 1 | palmdiagnosis (id=711/meta=1394) | palm/weixin/device_manage | feature/134389917-... | ✅ |
  | 2 | palm_local    (id=???/meta=???) | palm/palmpay/palm_local   | feature/134389917-... | ✅ |
- 其他服务：走默认 baseline（不进 serviceInstanceList）

确认无误吗？（yes / 修改 …）
```

**必须逐仓库校验分支已推送到远端**（`git ls-remote origin <branch>` 或等价手段）；未推送的**阻断**，提示先走 `/spec-push`。

**同时校验 §3 里的 `GIT_BRANCH_PROJECT.aliasName` 与仓库匹配**；不匹配的**阻断**，让用户核对 service 选择。

**同名环境防重（推荐）**：确认前调用 `POST /api/v4/projects/71/envs_query`（`references/API.md` §5）按 `name = <草案 env_name>` 查一次：
- 命中未过期同名（`endTime > now`）→ 让用户选：**复用**（跳过 Step 4，直接进 Step 5 输出该已有环境）/ **换名**（回本步改 `name`）/ **强制新建**（继续 Step 4，接受服务端裁决）
- 查询接口 5xx / 非 `code=0` → **降级为 warning**，不阻塞创建，只在确认提示里加一行"⚠️ 未能完成同名检查"

### Step 4: 调用 zerus API 创建环境

契约完整定义在 `references/API.md` §4。要点：

- `POST https://zerus.woa.com/api/v4/projects/71/envs`（Cookie 请求头即可）
- Body 顶层字段（skill 需覆盖，其他字段按 `references/API.md` §4 模板原样传）：
  - `name` ← 环境名（Step 3 用户确认）
  - `endTime` ← 过期时间，**默认 `now() + 7 days`**（UTC，`YYYY-MM-DDTHH:mm:ss.sssZ`）
  - `type: "vt"` 固定
  - `serviceInstanceList[]` ← 每个变更服务一项
  - `envCustom.additionEnvVars: []`
- 每个 `serviceInstance` 里：
  - `serviceID` / `serviceMetaID` / `name` ← 来自 §1/§2
  - `type: "vt"` 固定
  - `buildInfo.type: "bk"`，`buildInfo.base: {"autoNew": false}`（默认不立即首建；本字段与 push 触发无关，见 `references/API.md` §4）
  - `buildInfo.bk.params` ← §3 拿到的 `params[]` **深拷贝原样带回**，做两处修改：
    * `GIT_BRANCH_PROJECT.defaultValue = <该服务的 feature 分支>`
    * **`GIT_BRANCH_PROJECT.auto = true`**（关键：这是 Git push 自动触发的真正开关；模板里默认 `false`，必须强制翻转）
    * 可选：`BK_CI_BUILD_MSG.defaultValue = "[cospec] create env for <branch>"`
  - `releaseInfo.type: "direct"`，`releaseInfo.base: {"autoNew": false}`（语义同上，与 push 触发无关）
  - `releaseInfo.bk` / `releaseInfo.direct` 结构保留、字段全空

**推荐直接复用 `scripts/zerus_api.py` 里的封装函数**（自带 curl HTTP/2、cookie 装载、body 组装、GIT_BRANCH_PROJECT 覆盖）：

```python
from zerus_api import (
    preflight, PreflightError, render_manual_fallback,     # ← Step 0 用
    load_cookie_header, get_bk_params, extract_params_list,
    build_service_instance, build_create_env_body, create_env,
    query_envs, extract_env_list, iso_utc_after_days, default_env_name,
)
```

处理表：

| 情形 | 处理 |
|------|------|
| `2xx` && `code == 0` | 成功，取 `data.id` / `data.hashName` / `data.name`，进入 Step 5 |
| `2xx` && `code != 0` | 展示 `message`，**不重试**，交回用户；常见：同名冲突、分支不存在 |
| `4xx` | 展示 `message`；打印 body diff（**必须脱敏 cookie**），交回用户 |
| `401` / `302 → 登录页` | Cookie 过期 → 回 Step 1 重新登录后重试**一次**；再失败停止 |
| `5xx` / 超时 | 指数退避重试 ≤ 2 次（2s / 5s）；**重试前先用列表接口查一遍同名环境**（`references/API.md` §5）：若命中 `creator = 当前用户` 且 `endTime` 合理的同名 env，判定为已创建成功，跳过重试进 Step 5；两次重试仍失败 → **降级为 Step 0 的手动 fallback**（把 env_name / endTime / 服务分支表交给用户去 zerus 页面创建） |

⚠️ 幂等未验证：**同名 `name` 是否允许由服务端决定**。Step 3 的同名检查是主要防线；Step 4 的重试防双创是兜底。

### Step 5: 输出环境信息

创建成功后（用户明确要求），**必须**输出：
- 环境名 (`name`)
- 关联的服务（列表）
- 每个服务对应的分支

推荐模板：

```markdown
✅ zerus 环境已创建

- 环境名 (name)：{data.name}
- 环境 ID：{data.id}                                   // 数字，用于后续查询 / 删除
- hashName：{data.hashName}                            // URL 短标识
- 访问入口：见 zerus 控制台按环境名搜索（响应未直接返回 accessUrl，前端路由待定）
- 过期时间：{data.endTime}（UTC）
- 服务实例状态：{data.serviceInstanceList[0].status}  // 通常是 BUILD_WAIT，说明已触发构建
- 关联服务与分支：
  | 服务 | 仓库 | feature 分支 |
  |------|------|--------------|
  | palmdiagnosis | palm/weixin/device_manage | feature/134389917-... |
  | palm_local    | palm/palmpay/palm_local   | feature/134389917-... |
- 未覆盖服务：走默认 baseline

后续建议：
- [ ] 打开访问入口做联调 smoke test
- [ ] 把入口链接同步到 spec 「实施记录」/ MR 描述
- [ ] 联调时如遇报错 / trace 异常，直接说人话（「刚才那笔请求出错了」/「trace_id=xxx 挂了」），`analyze-log` skill 会自动锁定到本环境覆盖的服务范围查日志
- [ ] 联调完成后记得回收环境（避免长期占用资源）
```

如果响应里没有直接的 `access_url` 字段，提示用户到 zerus 控制台按环境名/ID 打开即可。

> **与 `analyze-log` skill 联动**：本 Step 5 输出的 `serviceInstanceList[].name` 会作为 `analyze-log` 的一级上下文输入，用于将日志查询范围锁定到本次改动服务（不必全项目撒网）。skill 联动细节见 `skills/analyze-log/SKILL.md` §会话上下文感知。

---

## 目录结构

```
skills/create-zerus/
├── SKILL.md                     # 本文件（AI 主入口）
├── README.md                    # 人类维护文档
├── scripts/
│   ├── login_and_save.py        # 弹浏览器让用户登录，产出 cookies.json / state.json
│   ├── zerus_api.py             # 底层：cookie / curl HTTP/2 / §1~§5 请求 / body 组装
│   ├── probe.py                 # §1/§2/§3 单点只读抓样，用于回填 API.md
│   ├── smoke_test.py            # 端到端冒烟：随机挑服务真建环境（默认 1h 过期）
│   ├── collect_change_context.py# 扫 src/* 产出 payload 草稿（Step 2 兜底）
│   ├── cookies.json             # ⚠️ 敏感，不要提交（gitignored）
│   ├── state.json               # ⚠️ 敏感，不要提交（gitignored）
│   └── requirements.txt
├── references/
│   └── API.md                   # zerus 后台 API 契约（§1~§5，AI 按需加载）
└── samples/                     # 契约参考样本（真实抓包 + 代表性响应）
    ├── services_query.json                  # §1 全量服务清单（也作为 smoke_test 缓存）
    ├── service_detail_example.json          # §2 单服务详情
    ├── bk_params_example.json               # §3 params 模板
    ├── create_env_request_example.json      # §4 用户 F12 抓包 request
    ├── create_env_response_example.json     # §4 实测响应
    ├── list_envs_request_example.json      # §5 抓包 request
    ├── list_envs_response_example.json      # §5 实测响应
    ├── update_env_request_example.json      # §6 抓包 request（PUT，含新增/保留 instance 示例）
    ├── delete_env_request_example.md        # §7 抓包 request（DELETE，空 body；脱敏说明）
    └── _scratch/                            # 探测 / 冒烟脚本的即时产物（gitignored）
```

---

## 与其他 skill / 命令的关系

- **前置**：`/spec-push`（`docs/git-workflow.md` 阶段 C）—— 必须先把所有改动仓库 push 到远端。
- **前置资料**：`plans/<STORYID>-*-plan.md` 的「涉及仓库」表 —— Step 2 首选来源。
- **对照**：`docs/git-workflow.md` baseline 表 —— 判断哪些是变更仓库、哪些走默认。
- **后置**（可选）：环境更新 / 回收 —— 目前**不在本 skill 范围**，如需请另起 skill。

---

## 注意事项

### ❌ 不要做

- 不要**打印 / 回传 cookie 或 state 内容**到对话、日志、报告。
- 不要**跳过 Step 3 的用户确认**直接调 API 创建（哪怕 payload 看起来完全正确）。
- 不要**用未 push 的本地分支**创建环境（远端拉不到，一定失败）。
- 不要在 cookie 过期时**无限重试**；最多重登录 1 次。
- 不要**硬编码 API endpoint 与字段名**；集中在 `scripts/zerus_api.py` 里维护。
- 不要**用 Python `requests` 直接打 zerus**：网关强制 HTTP/2，`requests` 会在 TLS 握手被断连；用 `zerus_api.curl_json`（走 curl 子进程）。

### ✅ 应该做

- 每次调 API 前，**校验涉及分支在远端存在**。
- 涉及仓库列表**从 plan 读取**为主，git 现状为辅；两者冲突时先跟用户对齐。
- payload 展示时**打表格 + 变更条目数**，让用户一眼看清覆盖范围。
- 环境创建后**立即把访问入口发给用户**，不要藏在日志里。
- API 契约有变化时，只改 `scripts/zerus_api.py` 与 `references/API.md`；SKILL.md 只维护流程与决策。

---

## 未决事项（等用户输入 / 待抓包）

- [x] ~~zerus **创建环境** API 的 method / path / 请求体字段~~ → 已确定：`POST /api/v4/projects/71/envs`，契约见 `references/API.md` §4
- [x] ~~环境**默认 TTL**~~ → 已确定：`endTime = now + 7d`（用户指令）
- [x] ~~`autoNew` 默认值~~ → 已确定：构建 & 发布两侧均传 `{autoNew: false}`（不立即首建；与 push 触发无关）
- [x] ~~**创建环境响应结构**~~ → 已抓到：`data.{id, name, hashName, type, creator, endTime, status, serviceInstanceList[].status=BUILD_WAIT, ...}`，见 `references/API.md` §4 响应段
- [x] ~~**环境列表 / 查询 API**~~ → `POST /api/v4/projects/71/envs_query?page=&pageSize=`，见 `references/API.md` §5
- [x] ~~**列表接口响应结构**~~ → `data.{page, pageSize, count, list: [env, ...]}`，单条 env 与 §4 响应结构一致，见 `references/API.md` §5 响应段
- [x] ~~**列表接口 name 过滤是否精确匹配**~~ → 实测**精确匹配**（完整字符串命中 1 条，`count: 1`）
- [ ] **访问入口 URL 前端路径** —— API 响应里无 `accessUrl`，需要抓一次 zerus 前端"打开环境"的路由（推测 `https://zerus.woa.com/env/{hashName}`，未验证）
- [x] ~~**Git push 自动触发的真正开关**~~ → 已明确（用户 UI 两次抓包 diff，唯一差异字段）：**`bk.params[GIT_BRANCH_PROJECT].auto`** —— `true` 开、`false` 关。`base.autoNew` 只控制"是否立即首建"，与 push 触发正交。skill 已在 `override_git_branch` 里强制 `auto=true`
- [ ] **环境状态查询 / 轮询 API** —— 服务实例创建时 `status: "BUILD_WAIT"`，说明有状态机；轮询 endpoint 待抓包（推测就是 §5 或另一个 detail 接口）
- [ ] `serviceInstance.status` 的完整状态枚举（`BUILDING` / `RELEASING` / `RUNNING` / `FAILED` 等？观察一次生命周期即可）
- [ ] 是否需要传递 **commit sha** 而非仅分支名（当前抓包只用分支名，暂不需要）
- [ ] **已存在同名环境**的服务端行为（覆盖 / 报错 / 允许重名）—— 需要一次故意重名的实测
- [ ] `type: "vt"` 之外是否还有其他环境类型（`prod` / `staging` …）
- [x] ~~**修改环境**接口~~ → 已确定：`PUT /api/v4/projects/71/envs/{envID}`（全量覆盖 PUT），见 `references/API.md` §6
- [x] ~~**删除 / 回收环境**接口~~ → 已确定：`DELETE /api/v4/projects/71/envs/{envID}`（空 body），见 `references/API.md` §7
- [ ] **修改环境 / 删除环境**接口的**响应结构** —— 抓包只覆盖了 request 侧，服务端返回体待实测回填
