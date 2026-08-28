"""zerus.woa.com HTTP API 底层工具。

集中封装：
- cookie 加载（从 login_and_save.py 写出的 cookies.json）
- 通过 curl --http2 发请求（zerus 网关强制 HTTP/2，Python requests 会被断连）
- API.md §1~§5 的请求/组装函数：services_query / service_detail / bk_params /
  create_env / envs_query
- 通用工具：组装 §4 body、覆盖 GIT_BRANCH_PROJECT、生成默认 env_name / endTime

所有面向 zerus 的脚本（probe.py / smoke_test.py / 未来的正式入口）都应该
只依赖本模块，不要重复实现 curl 调用或 body 组装。

安全：
- 不打印 cookie 值，只打印数量
- 不把 cookie 写入日志 / 响应文件

契约见 references/API.md。
"""
from __future__ import annotations

import copy
import getpass
import json
import subprocess
import sys
import time
from datetime import datetime, timedelta, timezone
from pathlib import Path

# ============================================================
# 路径 / 常量
# ============================================================

SCRIPTS_DIR = Path(__file__).resolve().parent
SKILL_DIR = SCRIPTS_DIR.parent
SAMPLES_DIR = SKILL_DIR / "samples"

# 默认放在 scripts/ 下（跟 login_and_save.py 产物同目录）
DEFAULT_COOKIES_FILE = SCRIPTS_DIR / "cookies.json"

BASE_URL = "https://zerus.woa.com"
DEFAULT_PROJECT_ID = 71
DEFAULT_TIMEOUT_SEC = 60
PREFLIGHT_TIMEOUT_SEC = 8


# ============================================================
# Preflight：依赖 / 连通性 / cookie 三件套检查
# ============================================================

class PreflightError(RuntimeError):
    """预检失败。code 说明失败类别，供上层决定如何降级：

    - "curl_missing"     : 没装 curl
    - "http2_unsupported": curl 不支持 --http2
    - "network"          : DNS/TCP/TLS 打不通 zerus.woa.com
    - "cookie_missing"   : 还没跑 login_and_save.py
    - "cookie_expired"   : 有 cookie 但被 302 到登录页
    """

    def __init__(self, code: str, message: str, hint: str = ""):
        super().__init__(message)
        self.code = code
        self.message = message
        self.hint = hint


def _check_curl_http2() -> None:
    """确认本机有 curl 且 curl 编译时带了 HTTP/2 支持。"""
    try:
        r = subprocess.run(
            ["curl", "--version"], capture_output=True, text=True, timeout=5, check=False,
        )
    except FileNotFoundError:
        raise PreflightError(
            "curl_missing",
            "本机找不到 curl 命令",
            hint="macOS 自带 curl；Linux 请 `apt/yum install curl`",
        )
    except subprocess.TimeoutExpired:
        raise PreflightError("curl_missing", "curl --version 执行超时", hint="")

    if r.returncode != 0:
        raise PreflightError(
            "curl_missing",
            f"curl --version 返回非零 (rc={r.returncode})",
            hint=r.stderr[:200],
        )
    # 输出里第 2 行通常是 "Features: ... HTTP2 ..."
    if "HTTP2" not in r.stdout:
        raise PreflightError(
            "http2_unsupported",
            "本机 curl 不支持 HTTP/2（Features 里没有 HTTP2）",
            hint="macOS 自带 curl 已支持；Linux 请升级到 curl 7.47+ 并带 --with-nghttp2",
        )


def _check_python_deps() -> None:
    """确认 playwright / requests 可导入。缺就抛 deps_missing，让用户去跑 bootstrap.py。"""
    missing: list[str] = []
    for mod in ("playwright", "requests"):
        try:
            __import__(mod)
        except ImportError:
            missing.append(mod)
    if missing:
        raise PreflightError(
            "deps_missing",
            f"Python 依赖缺失：{', '.join(missing)}",
            hint=(
                "首次使用请跑：`python3 scripts/bootstrap.py`"
                "（会先弹确认再装 venv + pip 包 + chromium）"
            ),
        )


def _check_network(base_url: str = BASE_URL, timeout: int = PREFLIGHT_TIMEOUT_SEC) -> None:
    """打一个短超时的 HEAD 请求，只判断是否能建 TCP+TLS 到 zerus。

    不判 HTTP 状态码（哪怕 302/401 都算网络通），只要 curl 能拿到响应就算通。
    """
    cmd = [
        "curl", "-sS", "-I", "--http2", "--max-time", str(timeout),
        "-o", "/dev/null",
        "-w", "%{http_code}",
        base_url + "/",
    ]
    try:
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout + 2, check=False)
    except subprocess.TimeoutExpired:
        raise PreflightError(
            "network",
            f"{timeout}s 内无法建连 {base_url}",
            hint="确认是否在公司内网 / 已连 VPN；或 DNS 是否能解析 zerus.woa.com",
        )
    if r.returncode != 0:
        # curl exit code 6=DNS / 7=Connect / 28=Timeout / 35=TLS
        raise PreflightError(
            "network",
            f"curl 无法访问 {base_url}（rc={r.returncode}）",
            hint=(r.stderr or "").strip()[:300] or "常见原因：未连内网 VPN、DNS 无法解析、TLS 拦截",
        )
    code = (r.stdout or "").strip()
    if not code or not code.isdigit():
        raise PreflightError(
            "network",
            f"访问 {base_url} 得到非预期响应",
            hint=f"HTTP status='{code[:50]}'",
        )


def _check_cookie(cookies_file: Path | None = None) -> str:
    """尝试加载 cookie。文件缺失 → cookie_missing。"""
    path = cookies_file or DEFAULT_COOKIES_FILE
    if not path.exists():
        raise PreflightError(
            "cookie_missing",
            f"未找到登录凭证 {path.name}",
            hint="请先在本机跑：python skills/create-zerus/scripts/login_and_save.py",
        )
    try:
        with path.open("r", encoding="utf-8") as f:
            cookies = json.load(f)
    except (json.JSONDecodeError, OSError) as e:
        raise PreflightError(
            "cookie_missing",
            f"{path.name} 解析失败：{e.__class__.__name__}",
            hint="重新跑一次 login_and_save.py 覆盖旧文件",
        )
    parts = [f"{c['name']}={c['value']}" for c in cookies if c.get("name")]
    if not parts:
        raise PreflightError(
            "cookie_missing",
            f"{path.name} 里没有可用 cookie",
            hint="重新跑一次 login_and_save.py",
        )
    return "; ".join(parts)


def preflight(
    cookies_file: Path | None = None,
    *,
    check_cookie: bool = True,
    probe_login_state: bool = True,
) -> str:
    """完整预检：依赖 → 网络 → cookie → (可选) 用 §1 tiny 请求探一下 cookie 是否过期。

    Returns:
        通过时返回 cookie header 字符串（供后续 API 调用直接用）；
        `check_cookie=False` 时返回空串（仅做依赖 & 网络自检）。

    Raises:
        PreflightError: 任一环节失败，上层可 render_manual_fallback() 给用户手动创建指引。
    """
    _check_curl_http2()
    _check_python_deps()
    _check_network()
    if not check_cookie:
        return ""
    cookie = _check_cookie(cookies_file)
    if probe_login_state:
        # 打一个极小的 §1（pageSize=1）探测 cookie 是否过期；不吞 LoginExpired
        url = f"{BASE_URL}/api/v4/projects/{DEFAULT_PROJECT_ID}/services_query?page=1&pageSize=1"
        try:
            status, text = curl_json(
                url, cookie, method="POST", body='{"name":""}',
                timeout=PREFLIGHT_TIMEOUT_SEC,
            )
        except RuntimeError as e:
            raise PreflightError(
                "network",
                "cookie 验证请求发送失败",
                hint=str(e)[:300],
            )
        try:
            parse_json_or_raise(status, text, "preflight.services_query")
        except LoginExpired as e:
            raise PreflightError(
                "cookie_expired",
                "已保存的 cookie 已过期",
                hint=str(e),
            )
    return cookie


# ============================================================
# 手动 fallback 渲染：网络/依赖异常时给用户可操作的兜底指引
# ============================================================

def render_manual_fallback(
    err: PreflightError | None = None,
    *,
    env_name: str | None = None,
    end_time: str | None = None,
    services: list[dict] | None = None,
    branch: str | None = None,
    project_id: int = DEFAULT_PROJECT_ID,
) -> str:
    """把「zerus 网址 + 环境名 + 服务/分支表」渲染成给用户看的降级指引。

    Args:
        err: 预检失败原因（可选，为 None 时表示非预检失败但仍想给手动指引）
        env_name: 计划创建的环境名（可选）
        end_time: 计划过期时间 ISO 字符串（可选）
        services: [{name, branch}, ...] 每个 dict 至少含 name；缺 branch 则用顶层 branch
        branch: 兜底分支名（未指定 services[].branch 时用它）
        project_id: zerus projectId

    Returns:
        多行文本，直接 print 或塞进对话即可。
    """
    lines: list[str] = []
    lines.append("=" * 60)
    lines.append("[!] zerus 自动化流程不可用，请改为手动在浏览器创建环境")
    lines.append("=" * 60)

    if err is not None:
        lines.append(f"原因：{err.code} —— {err.message}")
        if err.hint:
            lines.append(f"建议：{err.hint}")
        if err.code == "deps_missing":
            lines.append("")
            lines.append("一键装依赖（脚本会先弹确认，再装 venv + pip 包 + chromium）：")
            lines.append("  $ cd skills/create-zerus/scripts")
            lines.append("  $ python3 bootstrap.py")
        lines.append("")

    lines.append(f"zerus 控制台：{BASE_URL}/")
    lines.append(f"项目 ID：{project_id}（palm）")
    lines.append(f"新建环境入口：{BASE_URL}/  → 左侧「联调环境」→「新建」")
    lines.append("")

    if env_name:
        lines.append(f"建议环境名：{env_name}")
    if end_time:
        lines.append(f"建议过期时间：{end_time}")

    if services:
        lines.append("")
        lines.append("需要覆盖的服务与分支：")
        # 简单表格
        lines.append(f"  {'#':<3} {'服务名':<30} {'分支':<40}")
        lines.append(f"  {'-'*3} {'-'*30} {'-'*40}")
        for i, s in enumerate(services, 1):
            name = str(s.get("name") or "")[:30]
            br = str(s.get("branch") or branch or "")[:40]
            lines.append(f"  {i:<3} {name:<30} {br:<40}")
        lines.append("")
        lines.append("操作步骤（在 zerus 页面）：")
        lines.append("  1. 打开控制台 → 新建联调环境")
        lines.append("  2. 填写环境名 / 过期时间 / 备注")
        lines.append(
            "  3. 添加上表中的每个服务，按 GIT_BRANCH_PROJECT 覆盖为对应分支；"
            "其余未列出的服务保持默认基线"
        )
        lines.append("  4. 保存并等待构建触发")
    elif branch:
        lines.append("")
        lines.append(f"分支（如未逐服务指定）：{branch}")

    lines.append("=" * 60)
    return "\n".join(lines)


# ============================================================
# Cookie 加载
# ============================================================

def load_cookie_header(cookies_file: Path | None = None) -> str:
    """把 Playwright 导出的 cookies.json 拼成 `Cookie: k=v; k=v` 里的 value 部分。

    Raises SystemExit(2) 时说明还没登录过 / cookie 文件丢了。
    """
    path = cookies_file or DEFAULT_COOKIES_FILE
    if not path.exists():
        print(
            f"[!] 找不到 {path}，请先 python scripts/login_and_save.py",
            file=sys.stderr,
        )
        sys.exit(2)
    with path.open("r", encoding="utf-8") as f:
        cookies = json.load(f)
    parts = [f"{c['name']}={c['value']}" for c in cookies if c.get("name")]
    return "; ".join(parts)


# ============================================================
# curl 底层
# ============================================================

def curl_json(
    url: str,
    cookie_header: str,
    *,
    method: str = "GET",
    body: str | None = None,
    timeout: int = DEFAULT_TIMEOUT_SEC,
) -> tuple[int, str]:
    """发一个 curl 请求，返回 (http_status, body_text)。

    - 不加 -L：碰到 302 → 登录页时能立刻发现 cookie 过期，而不是被塞一坨 HTML
    - `--http2` 是必需的，Python requests / httpx[http1] 会被 zerus 网关断连
    """
    cmd = [
        "curl", "-sS", "--max-time", str(timeout), "--http2",
        "-o", "-",
        "-w", "\n__HTTP_STATUS__=%{http_code}",
        "-X", method,
        "-H", f"Cookie: {cookie_header}",
        "-H", "Accept: application/json, text/plain, */*",
        "-H", "Accept-Language: zh-CN,zh;q=0.9,en;q=0.8",
        "-H", f"Referer: {BASE_URL}/",
        "-H", f"Origin: {BASE_URL}",
        "-H", (
            "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) "
            "AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.0 Safari/605.1.15"
        ),
    ]
    if body is not None:
        cmd += ["-H", "Content-Type: application/json", "--data", body]
    cmd += [url]

    result = subprocess.run(cmd, capture_output=True, text=True, check=False)
    if result.returncode != 0:
        raise RuntimeError(
            f"curl failed rc={result.returncode}\nstderr: {result.stderr[:300]}"
        )

    text = result.stdout
    status = 0
    if "\n__HTTP_STATUS__=" in text:
        text, tail = text.rsplit("\n__HTTP_STATUS__=", 1)
        try:
            status = int(tail.strip())
        except ValueError:
            status = 0
    return status, text


class LoginExpired(RuntimeError):
    """cookie 过期或未登录时抛出，交上层决定是否重登录。"""


def parse_json_or_raise(status: int, text: str, ctx: str) -> dict:
    """把 curl_json 的原始返回解析成 dict；302/登录页 → LoginExpired；4xx+ → SystemExit。"""
    if status == 302 or (
        status == 200 and "<html" in text.lower() and "登录" in text
    ):
        raise LoginExpired(
            f"{ctx}: HTTP {status}，被重定向到登录页 —— cookie 已过期，"
            f"请重新跑 scripts/login_and_save.py"
        )
    if status >= 400:
        raise SystemExit(f"[!] {ctx}: HTTP {status}\nbody preview: {text[:500]}")
    try:
        return json.loads(text)
    except json.JSONDecodeError:
        raise SystemExit(
            f"[!] {ctx}: 非 JSON 响应 (HTTP {status})\nbody preview: {text[:300]}"
        )


def shape(v, depth=0, max_depth=4):
    """对响应体做递归摘要，便于打印。字符串 > 100 时只显示长度。"""
    if depth > max_depth:
        return "..."
    if isinstance(v, dict):
        return {k: shape(sub, depth + 1, max_depth) for k, sub in v.items()}
    if isinstance(v, list):
        if not v:
            return []
        return [shape(v[0], depth + 1, max_depth), f"...(len={len(v)})"]
    if isinstance(v, str):
        return f"<str len={len(v)}>" if len(v) > 100 else v
    return v


# ============================================================
# §1 services_query
# ============================================================

def query_services(
    cookie: str,
    project_id: int = DEFAULT_PROJECT_ID,
    name_filter: str = "",
    page: int = 1,
    page_size: int = 99999,
) -> dict:
    """POST /api/v4/projects/{pid}/services_query，返回响应 dict。

    注意：路径以 _query 结尾看似 GET，实际是 POST + JSON body。
    """
    url = (
        f"{BASE_URL}/api/v4/projects/{project_id}"
        f"/services_query?page={page}&pageSize={page_size}"
    )
    body = json.dumps({"name": name_filter}, ensure_ascii=False)
    status, text = curl_json(url, cookie, method="POST", body=body)
    return parse_json_or_raise(status, text, "services_query")


# ============================================================
# §2 service_detail
# ============================================================

def get_service_detail(
    cookie: str,
    service_id: int | str,
    project_id: int = DEFAULT_PROJECT_ID,
) -> dict:
    """GET /api/v4/projects/{pid}/services/{sid}，返回响应 dict。"""
    url = f"{BASE_URL}/api/v4/projects/{project_id}/services/{service_id}"
    status, text = curl_json(url, cookie, method="GET")
    return parse_json_or_raise(status, text, f"service_detail#{service_id}")


# ============================================================
# §3 bk_params
# ============================================================

def get_bk_params(
    cookie: str,
    service_id: int | str,
    service_meta_id: int | str,
    project_id: int = DEFAULT_PROJECT_ID,
) -> dict:
    """GET .../services/{sid}/meta/{mid}/bk_params，返回响应 dict。

    响应里的 data.params[] 就是 §4 create_env 时要塞进 buildInfo.bk.params 的模板。
    """
    url = (
        f"{BASE_URL}/api/v4/projects/{project_id}"
        f"/services/{service_id}/meta/{service_meta_id}/bk_params"
    )
    status, text = curl_json(url, cookie, method="GET")
    return parse_json_or_raise(status, text, f"bk_params#{service_id}/{service_meta_id}")


def extract_params_list(bk_params_resp: dict) -> list[dict] | None:
    """从 get_bk_params() 返回的响应里取 data.params[]。空或结构异常返回 None。"""
    params = ((bk_params_resp.get("data") or {}).get("params")) or bk_params_resp.get("params")
    if not isinstance(params, list) or not params:
        return None
    return params


# ============================================================
# §4 create_env
# ============================================================

def override_git_branch(
    params: list[dict],
    branch: str,
    build_msg: str | None = None,
) -> bool:
    """就地覆盖 params 里 GIT_BRANCH_PROJECT.defaultValue（可选覆盖 BK_CI_BUILD_MSG）。

    - 找到 GIT_BRANCH_PROJECT 就返回 True；没找到返回 False（调用方需据此跳过该服务）
    - **`GIT_BRANCH_PROJECT.auto` 强制置为 `True`** —— 这是让蓝盾把 Git push 事件
      的 branch/commit 自动灌进该参数的开关，也是 zerus 环境「Git push 自动触发」
      能真正生效的必要字段（用户 UI 抓包对比：`auto=False` 无触发，`auto=True` 才有）
    - 其他字段一律不动（elementAtomCode / repositoryHashID / aliasName / url…）
    """
    found = False
    for p in params:
        pid = p.get("id")
        if pid == "GIT_BRANCH_PROJECT":
            p["defaultValue"] = branch
            p["auto"] = True  # 关键：开启 push 自动触发
            found = True
        elif pid == "BK_CI_BUILD_MSG" and build_msg is not None:
            p["defaultValue"] = build_msg
    return found


def build_service_instance(
    service_id: int,
    meta_id: int,
    service_name: str,
    branch: str,
    params_template: list[dict],
    *,
    build_msg: str | None = None,
    auto_new: bool = False,
) -> dict | None:
    """按 API.md §4 组装一个 serviceInstance。

    - 深拷贝 params_template 以免污染
    - 覆盖 GIT_BRANCH_PROJECT.defaultValue = branch，**并把 auto 置为 True**
      （这是"开启 Git push 自动触发"的真正开关，见 override_git_branch）
    - 找不到 GIT_BRANCH_PROJECT → 返回 None（调用方跳过）
    - **`base: {"autoNew": auto_new}` 与 push 触发无关**：
      * `auto_new=False`（默认）：仅创建 instance，等 Git push 触发（前提是
        `GIT_BRANCH_PROJECT.auto=True`，本函数已强制开启）
      * `auto_new=True`：创建时立刻用 GIT_BRANCH_PROJECT 分支跑一次首轮构建
      详见 references/API.md §4 语义说明
    """
    params = copy.deepcopy(params_template)
    if not override_git_branch(params, branch, build_msg=build_msg):
        return None
    return {
        "id": 0,
        "envID": 0,
        "serviceID": service_id,
        "serviceMetaID": meta_id,
        "name": service_name,
        "status": "",
        "type": "vt",
        "buildInfo": {
            "type": "bk",
            # autoNew=False：不立即首建（联调常规姿态）。push 触发由 GIT_BRANCH_PROJECT.auto=True 控制
            "base": {"autoNew": auto_new},
            "bk": {
                "projectID": "",
                "pipelineID": "",
                "pipelineVersion": 0,
                "pipelineStatus": "",
                "buildID": "",
                "params": params,
                "triggerMode": "",
                "triggerUser": "",
            },
        },
        "releaseInfo": {
            "type": "direct",
            # 与 buildInfo.base 语义相同：仅控制"是否立即发布一次"，与 push 触发无关
            "base": {"autoNew": auto_new},
            "bk": {
                "projectID": "",
                "pipelineID": "",
                "pipelineVersion": 0,
                "pipelineStatus": "",
                "buildID": "",
                "params": [],
                "triggerMode": "",
                "triggerUser": "",
            },
            "direct": {
                "clusterArea": "",
                "namespace": "",
                "cloneDeploymentName": "",
                "cloneServiceName": "",
                "selectorServiceName": "",
                "containerName": "",
                "msg": "",
            },
        },
        "creator": "",
    }


def build_create_env_body(
    name: str,
    end_time_iso: str,
    desc: str,
    service_instances: list[dict],
) -> dict:
    """按 API.md §4 顶层结构组装 create_env body。

    Args:
        name: 环境名
        end_time_iso: 过期时间 ISO 8601 (YYYY-MM-DDTHH:mm:ss.sssZ)
        desc: 备注
        service_instances: 已通过 build_service_instance() 产出的列表
    """
    return {
        "id": 0,
        "name": name,
        "hashName": "",
        "type": "vt",
        "desc": desc,
        "projectID": 0,
        "endTime": end_time_iso,
        "creator": "",
        "prevent": False,
        "status": 0,
        "serviceInstanceList": service_instances,
        "envCustom": {"additionEnvVars": []},
    }


def create_env(
    cookie: str,
    body: dict,
    project_id: int = DEFAULT_PROJECT_ID,
) -> dict:
    """POST /api/v4/projects/{pid}/envs，返回响应 dict。"""
    url = f"{BASE_URL}/api/v4/projects/{project_id}/envs"
    status, text = curl_json(url, cookie, method="POST", body=json.dumps(body, ensure_ascii=False))
    return parse_json_or_raise(status, text, "create_env")


# ============================================================
# §5 envs_query
# ============================================================

def query_envs(
    cookie: str,
    project_id: int = DEFAULT_PROJECT_ID,
    name_filter: str = "",
    hash_name: str = "",
    creator: str = "",
    desc: str = "",
    type_filter: str = "",
    page: int = 1,
    page_size: int = 100,
) -> dict:
    """POST /api/v4/projects/{pid}/envs_query，返回响应 dict。

    ⚠️ 尽管是查询，路径也没带 _query，实际就是 POST + JSON body。
    过滤条件目前实测 name 精确匹配（完整字符串命中）。
    """
    url = (
        f"{BASE_URL}/api/v4/projects/{project_id}/envs_query"
        f"?page={page}&pageSize={page_size}"
    )
    body = json.dumps({
        "name": name_filter,
        "hashName": hash_name,
        "creator": creator,
        "desc": desc,
        "type": type_filter,
    }, ensure_ascii=False)
    status, text = curl_json(url, cookie, method="POST", body=body)
    return parse_json_or_raise(status, text, "envs_query")


# ============================================================
# §6 update_env（PUT /envs/{envID}）
# ============================================================
#
# 语义（基于真实抓包 samples/update_env_request_example.json）：
# - **全量覆盖式 PUT**：body 就是「环境的完整最新状态」；服务端按新旧 diff 出增删改
# - 保留原有 instance：id / envID 保持非零（服务端按 id 找到旧 instance 沿用）
# - **新增** instance：id=0 / envID=0，其余按 §4 create_env 的 serviceInstance 结构填
# - **删除** instance：从 serviceInstanceList 里省略即可（不出现即被删）
# - **改分支** instance：保留 id / envID / serviceID / serviceMetaID / name，覆盖
#   buildInfo.bk.params 里的 GIT_BRANCH_PROJECT.defaultValue；其它字段原样带回
# - 顶层可改：name / desc / endTime / prevent / envCustom.additionEnvVars
# - 顶层不改的字段（id / hashName / projectID / creator / createdAt）**也必须原样回传**


def get_env_detail(
    cookie: str,
    env_id: int | str,
    project_id: int = DEFAULT_PROJECT_ID,
) -> dict:
    """按 env id 拿环境详情。§5 精确查 hashName 兜底；找不到时抛 SystemExit。

    实测 §5 单条 env 结构就等于本接口 data，因此这里直接复用 query_envs+过滤。
    如果后续抓到独立的 GET /envs/{id} 接口，可在此替换实现。
    """
    # 目前没有独立 detail 端点；用 §5 listAll + 客户端过滤兜底
    resp = query_envs(cookie, project_id=project_id, page=1, page_size=200)
    envs = extract_env_list(resp)
    for env in envs:
        if str(env.get("id")) == str(env_id):
            return env
    raise SystemExit(f"[!] get_env_detail: env id={env_id} 在项目 {project_id} 下未找到")


def update_env(
    cookie: str,
    env_id: int | str,
    body: dict,
    project_id: int = DEFAULT_PROJECT_ID,
) -> dict:
    """PUT /api/v4/projects/{pid}/envs/{envID}，返回响应 dict。

    body 必须是**完整环境状态**（顶层 id/hashName/... + serviceInstanceList 全量）；
    通常做法是先 get_env_detail() 拿到当前 env，然后就地 modify，再 PUT 回去。
    """
    url = f"{BASE_URL}/api/v4/projects/{project_id}/envs/{env_id}"
    status, text = curl_json(
        url, cookie, method="PUT", body=json.dumps(body, ensure_ascii=False),
    )
    return parse_json_or_raise(status, text, f"update_env#{env_id}")


def add_service_instance(env: dict, instance: dict) -> dict:
    """就地把一个新 instance 追加进 env['serviceInstanceList']（不去重）。

    - `instance` 通常来自 `build_service_instance(...)`，其 id=0 / envID=0 表示新增
    - 若 env 里已存在同 name 的 instance，本函数**不做处理**（不去重），由调用方决定
    """
    lst = env.setdefault("serviceInstanceList", [])
    lst.append(instance)
    return env


def remove_service_instance_by_name(env: dict, service_name: str) -> int:
    """就地移除 env['serviceInstanceList'] 里所有 name==service_name 的 instance。

    Returns:
        实际移除的数量。0 表示没找到。
    """
    lst = env.get("serviceInstanceList") or []
    kept = [x for x in lst if x.get("name") != service_name]
    removed = len(lst) - len(kept)
    env["serviceInstanceList"] = kept
    return removed


def update_service_branch_by_name(
    env: dict,
    service_name: str,
    branch: str,
    *,
    build_msg: str | None = None,
) -> bool:
    """就地把 env 里指定服务的 GIT_BRANCH_PROJECT 覆盖为新分支。

    - 只改 buildInfo.bk.params 里的 GIT_BRANCH_PROJECT.defaultValue（可选 BK_CI_BUILD_MSG）
    - 不动 id / envID / serviceID / serviceMetaID / status / releaseInfo 等其它字段
    - 未找到对应 service_name 时返回 False；未找到 GIT_BRANCH_PROJECT 也返回 False
    """
    for inst in env.get("serviceInstanceList") or []:
        if inst.get("name") != service_name:
            continue
        params = ((inst.get("buildInfo") or {}).get("bk") or {}).get("params")
        if not isinstance(params, list):
            return False
        return override_git_branch(params, branch, build_msg=build_msg)
    return False


# ============================================================
# §7 delete_env（DELETE /envs/{envID}）
# ============================================================
#
# 语义（基于真实抓包 samples/delete_env_request_example.md）：
# - `DELETE /api/v4/projects/{pid}/envs/{envID}`，请求体为空（`--data ''`）
# - 服务端只看 URL 里的 env id；不需要顶层 body / 不需要 hashName
# - 响应结构未实测，按其它接口规律推断为 `{code, message, data}`；skill 端只
#   把 `code == 0` 视为成功条件
# - **推测非幂等**：对已删除的 env 重复 DELETE 会 `code != 0`（`env not found`
#   之类）；因此本函数**不自动重试** 4xx 业务错


def delete_env(
    cookie: str,
    env_id: int | str,
    project_id: int = DEFAULT_PROJECT_ID,
) -> dict:
    """DELETE /api/v4/projects/{pid}/envs/{envID}，返回响应 dict。

    删除是**不可撤销**的操作：一旦服务端 code==0，对应 env 及其所有 instance
    的构建/发布记录都会随之作废。调用方**必须**在调用前跟用户二次确认。

    Args:
        cookie: preflight() 返回的 cookie header 字符串
        env_id: 目标环境 id（来自 §5 list 或 §4 响应）
        project_id: zerus projectId（默认 71）

    Returns:
        响应 dict。成功时 `code == 0`；失败时 `message` 里会带原因
        （如 "env not found" / "permission denied"）。

    Raises:
        LoginExpired: cookie 过期（HTTP 302 → 登录页）
        SystemExit: 服务端 4xx/5xx 或非 JSON 响应
    """
    url = f"{BASE_URL}/api/v4/projects/{project_id}/envs/{env_id}"
    # DELETE 无 body：curl_json 里 body=None 时不会加 Content-Type / --data
    status, text = curl_json(url, cookie, method="DELETE", body=None)
    return parse_json_or_raise(status, text, f"delete_env#{env_id}")


def extract_env_list(query_resp: dict) -> list[dict]:
    """从 query_envs() 响应里取 data.list[]。兼容几种可能的字段名。"""
    payload = query_resp.get("data") or {}
    if isinstance(payload, list):
        return payload
    for key in ("list", "items", "envs", "envList", "records"):
        v = payload.get(key) if isinstance(payload, dict) else None
        if isinstance(v, list):
            return v
    return []


# ============================================================
# 环境名 / endTime 帮助函数
# ============================================================

def default_env_name(prefix: str = "cospec") -> str:
    """默认环境名：<prefix>-<user>-<ts>，避免撞名。"""
    ts = int(time.time())
    try:
        who = getpass.getuser() or "anon"
    except Exception:
        who = "anon"
    who = "".join(c for c in who if c.isalnum()) or "anon"
    return f"{prefix}-{who}-{ts}"


def iso_utc_after(hours: float) -> str:
    """UTC + hours，返回 YYYY-MM-DDTHH:mm:ss.sssZ（毫秒精度）。"""
    dt = datetime.now(timezone.utc) + timedelta(hours=hours)
    return dt.strftime("%Y-%m-%dT%H:%M:%S.") + f"{dt.microsecond // 1000:03d}Z"


def iso_utc_after_days(days: float) -> str:
    """UTC + days 的便捷入口。skill 默认 7d。"""
    return iso_utc_after(days * 24)
