## 技能文档

### 基本信息
- 技能名: `create-zerus`
- 创建人: @kennyzhxu (kennyzhxu@tencent.com)
- 版本: v0.5.0
- 更新时间: 2026-07-13

### 适用场景

一个 spec 完成 `/spec-implement` + `/spec-push` 后，把改动的服务定向到 feature 分支、拉起一套 zerus 联调环境供测试 / 前端 / 产品验证。

典型触发语句：
- 「拉一套 zerus」
- 「起个 zerus 环境」
- 「create zerus」
- 「联调环境」/「新建 zerus」

### 前置条件

- 已安装 Python 3.9+，能创建 venv
- 首次使用需装依赖，二选一：

  **推荐（一键装，会先弹确认）：**
  ```bash
  cd skills/create-zerus/scripts
  python3 bootstrap.py          # 会打印将执行的步骤 + 磁盘影响，输 y 确认
  # 或者跳过确认：python3 bootstrap.py --yes
  # 只想看会做什么：python3 bootstrap.py --dry-run
  source .venv/bin/activate
  ```

  **手动：**
  ```bash
  cd skills/create-zerus/scripts
  python3 -m venv .venv && source .venv/bin/activate
  pip install -r requirements.txt
  playwright install chromium
  ```
- 有 `https://zerus.woa.com/` 访问权限（能在浏览器完成 SSO 登录）
- 涉及仓库的 feature 分支已 push 到远端（`git push` 完毕）
- 本机可用 `curl --http2`（zerus 网关强制 HTTP/2；macOS 自带 curl 已支持）

### 使用示例

```
# 用户对话中
拉一套 zerus 环境，我要联调 spec 0003 那两个仓库

# skill 会：
# 1. 检查 / 触发登录（scripts/login_and_save.py）
# 2. 汇总变更服务 + 分支，跟用户确认 payload
# 3. 调 zerus API 创建环境
# 4. 输出环境名 / hashName / 关联服务
```

**修改已有环境**（v0.4.0 新增，`references/API.md` §6）：

```python
# Python REPL / 脚本内
from zerus_api import (
    preflight, get_env_detail, update_env,
    build_service_instance, add_service_instance,
    remove_service_instance_by_name, update_service_branch_by_name,
    get_bk_params, extract_params_list,
)

cookie = preflight()
env = get_env_detail(cookie, env_id=6597)

# 加服务
tpl = extract_params_list(get_bk_params(cookie, 706, 1386))
add_service_instance(env, build_service_instance(
    706, 1386, "palmgrpcuiproxy", "feature/xxx", tpl,
))

# 或：删服务 / 改分支 / 改 endTime …
# remove_service_instance_by_name(env, "devicegateway")
# update_service_branch_by_name(env, "wecarddataanalysisapp", "feature/yyy")
# env["endTime"] = "2026-08-01T00:00:00.000Z"

resp = update_env(cookie, 6597, env)
assert resp.get("code") == 0
```

**删除已有环境**（v0.5.0 新增，`references/API.md` §7，⚠️ 不可撤销 + 推测非幂等）：

```python
from zerus_api import preflight, get_env_detail, delete_env

cookie = preflight()
env = get_env_detail(cookie, env_id=6597)      # 先拉现状展示给用户
# skill 端跟用户二次确认：name / hashName / creator / 服务列表
if env.get("prevent"):
    raise SystemExit("[!] env 打了 prevent=true 防误删标记，请先在控制台解除")

resp = delete_env(cookie, env["id"])
assert resp.get("code") == 0, resp.get("message")
```

冒烟自测（不依赖具体 spec，直接跑通全流程）：

```bash
# dry-run，只组装 body 不发请求
python skills/create-zerus/scripts/smoke_test.py --dry-run --sample 2 --branch master

# 真跑，1h 后自动过期
python skills/create-zerus/scripts/smoke_test.py --confirm-create --sample 2 --branch master
```

抓样探测（回填 `references/API.md`）：

```bash
# §1 服务清单 + 前 2 个服务的 §2 详情
python skills/create-zerus/scripts/probe.py

# 单个服务的 §3 params
python skills/create-zerus/scripts/probe.py --service-id 711 --meta-id 1394
```

### 注意事项

⚠️ **敏感文件**：`scripts/cookies.json` 和 `scripts/state.json` 含登录态，已在根 `.gitignore` 忽略，**任何情况下都不要提交**，也不要打印内容到对话。

⚠️ **HTTP/2 强制**：zerus 网关只接受 HTTP/2；Python `requests`（HTTP/1.1）会在 TLS 握手阶段被断连（`SSLError: record layer failure`）。所有 API 调用统一走 `zerus_api.curl_json`（子进程调 curl `--http2`）。

⚠️ **同名防重**：Step 3 的 `envs_query` 精确查同名是主要防线；Step 4 5xx 重试前**必须**再查一遍，避免服务端已创建成功但客户端超时导致的双环境。

⚠️ **Git push 自动触发开关（已明确）**：真正的开关是 `buildInfo.bk.params[GIT_BRANCH_PROJECT].auto` —— `true` 开、`false` 关。skill 已在 `override_git_branch` 里强制置为 `true`。`base.autoNew` 只控制"是否立即首建"，与 push 触发正交（详见 `references/API.md` §4）。

⚠️ **未变更仓库不进 `serviceInstanceList`**：只放需要覆盖分支的服务；其余服务由 zerus 走默认 baseline。

⚠️ **网络/依赖异常自动降级**：v0.3.0 起，`smoke_test.py` / `probe.py` 启动时会先跑 `preflight()`（依赖 → 网络 → cookie 三件套），任一失败会打印**手动创建指引**（`zerus_api.render_manual_fallback()`），包含 zerus 网址、建议 env_name、服务分支表，退出码 `10`。skill 主流程也应在 Step 0 复用同一套逻辑。

### 已知问题

- [ ] 访问入口 URL 前端路径未抓（推测 `https://zerus.woa.com/env/{hashName}`），目前只能让用户到 zerus 控制台按名字搜
- [ ] 环境状态轮询 endpoint 未确定（`BUILD_WAIT → BUILDING → RUNNING/FAILED` 的完整枚举待观察）
- [x] ~~删除 / 回收环境接口未抓，环境需靠 `endTime` 自然过期~~ → v0.5.0 增加 §7 `delete_env`（`DELETE /envs/{env_id}` 空 body）
- [ ] 同名环境的服务端行为（覆盖 / 报错 / 允许重名）未做故意重名实测
- [ ] `collect_change_context.py` 里 `DEFAULT_BASELINE_MAP` 只覆盖了 4 个仓库，其余仓库需 `--baseline-map` 传自定义 map 或补齐 `docs/git-workflow.md`
- [x] ~~`_probe_via_curl.py` / `_probe_create_env.py` / `run_automation.py` 三个脚本的可复用工具重复~~ → v0.2.0 已合并到 `scripts/zerus_api.py`
- [x] ~~`autoNew` 语义写反~~ → v0.2.0 改为"待观察"，默认 `false`；v0.6.0 一度误以为 `base` 存在性是 push 触发开关；v0.7.0 用户 UI 抓包 diff 确认**真正开关是 `bk.params[GIT_BRANCH_PROJECT].auto`**，已在 `override_git_branch` 里强制 `true`
- [x] ~~网络/依赖异常时脚本直接 stacktrace，用户拿不到兜底~~ → v0.3.0 增加 `preflight()` + `render_manual_fallback()`，失败时输出手动创建指引
- [x] ~~缺少修改已有环境的能力（改分支 / 加删服务 / 延期）~~ → v0.4.0 增加 §6 `update_env`（PUT 全量覆盖）+ `zerus_api` 里的 `get_env_detail` / `add_service_instance` / `remove_service_instance_by_name` / `update_service_branch_by_name`
- [ ] §6 `update_env` 的响应结构未实测（当前抓包只有 request 侧）
- [x] ~~缺少删除 / 回收环境的能力~~ → v0.5.0 增加 §7 `delete_env`
- [ ] §7 `delete_env` 的响应结构未实测（当前抓包只有 request 侧）；`prevent=true` / 已删除 env 的错误 message 待实测

### 相关技能

- `feature-implementation`: 完成本次 spec 的代码改动，产出 feature 分支
- `change-summary`: 汇总本次 spec 的变更仓库与分支，可作为 Step 2 的输入
- `mr-review-resolve`: 联调环境验证通过后，进入 MR 评审阶段
