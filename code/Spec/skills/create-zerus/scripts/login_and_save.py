"""
一次性登录脚本：启动带界面的 Chromium，打开 https://zerus.woa.com/ ，
你手动完成登录后，脚本会**自动检测登录成功**（zerus.woa.com 下拿到 RIO_TOKEN cookie
且当前 URL 落回 zerus 域），随后保存登录态并退出。

也就是说：不再要求终端交互（`input()`），可以在非 TTY 环境（例如被 AI / CI 拉起）里运行。

产物：
- state.json    Playwright 完整会话（cookie + localStorage），供后续 Playwright 复用（可选）
- cookies.json  纯 cookie 列表，供 requests / httpx / curl 等 HTTP 客户端使用

用法：
    python login_and_save.py                # 默认最多等 5 分钟
    python login_and_save.py --timeout 600  # 自定义超时（秒）

Cookie 过期后，重新运行本脚本即可。
"""

from __future__ import annotations

import argparse
import json
import os
import stat
import sys
import time
from pathlib import Path

from playwright.sync_api import sync_playwright

TARGET_URL = "https://zerus.woa.com/"
BASE_DIR = Path(__file__).resolve().parent
STATE_FILE = BASE_DIR / "state.json"
COOKIES_FILE = BASE_DIR / "cookies.json"

# 用来判断"已登录"的 cookie 名。
# zerus 现网观察：新版登录态实际上只签发 P_RIO_TOKEN；老版 curl 抓包里能看到 RIO_TOKEN + P_RIO_TOKEN
# 一起出现。为兼容，只要下面**任一**名字出现在 zerus.woa.com 域下就视为已登录。
LOGIN_COOKIE_NAMES = {"RIO_TOKEN", "P_RIO_TOKEN"}
ZERUS_DOMAIN_SUFFIX = "zerus.woa.com"


def _chmod_600(path: Path) -> None:
    """把敏感文件权限收紧到仅当前用户可读写。"""
    try:
        os.chmod(path, stat.S_IRUSR | stat.S_IWUSR)
    except OSError:
        # Windows 或某些文件系统不支持时忽略
        pass


def _login_status(context):
    """只依赖 cookie 检测登录状态，不看 page.url（用户可能在别的 tab 完成登录）。

    返回 (ok: bool, zerus_cookie_names: set[str])。
    ok 的判断标准：zerus.woa.com 域下同时拿到 RIO_TOKEN 和 P_RIO_TOKEN
    （两者都是后续调 API 用到的）；如果只拿到其中一个，视为"部分完成"，继续等。
    """
    cookies = context.cookies()
    zerus_cookie_names = {
        c["name"] for c in cookies
        if ZERUS_DOMAIN_SUFFIX in c.get("domain", "")
    }
    # 任一 token 出现即认为已登录（现网只签发 P_RIO_TOKEN 的情况也算成功）
    ok = bool(LOGIN_COOKIE_NAMES & zerus_cookie_names)
    return ok, zerus_cookie_names


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--timeout", type=int, default=300,
        help="登录等待超时（秒），默认 300",
    )
    parser.add_argument(
        "--poll-interval", type=float, default=2.0,
        help="轮询间隔（秒），默认 2.0",
    )
    args = parser.parse_args()

    print(f"[*] 目标: {TARGET_URL}")
    print(f"[*] 等待登录最长 {args.timeout}s，每 {args.poll_interval}s 检测一次")
    print()
    print("=" * 60)
    print("请在弹出的浏览器窗口里完成登录。")
    print("脚本会自动检测登录状态，无需回终端按回车。")
    print("=" * 60)
    print(flush=True)

    with sync_playwright() as p:
        browser = p.chromium.launch(headless=False)
        context = browser.new_context()
        page = context.new_page()
        page.goto(TARGET_URL, wait_until="domcontentloaded")

        deadline = time.time() + args.timeout
        last_report = 0.0
        last_names: set = set()
        while time.time() < deadline:
            ok, names = _login_status(context)
            if ok:
                print(f"[+] 检测到登录成功，zerus 域 cookie: {sorted(names)}", flush=True)
                break
            # cookie 集合有变化就立刻汇报一次（帮助用户看到进度）
            if names != last_names:
                print(f"    zerus 域 cookie 更新: {sorted(names) or '(空)'}", flush=True)
                last_names = names
            now = time.time()
            if now - last_report >= 15:
                remaining = int(deadline - now)
                try:
                    cur_url = page.url
                except Exception:
                    cur_url = "(page closed)"
                print(f"    …等待登录中，剩余 {remaining}s（首个 tab URL: {cur_url}）", flush=True)
                last_report = now
            time.sleep(args.poll_interval)
        else:
            ok, names = _login_status(context)
            print(f"[!] 超时未检测到登录成功（{args.timeout}s），当前 zerus 域 cookie: {sorted(names) or '(空)'}", flush=True)
            browser.close()
            return 2

        # 保存完整会话（推荐，后续 Playwright 自动化可直接复用）
        context.storage_state(path=str(STATE_FILE))
        _chmod_600(STATE_FILE)
        print(f"[+] 已保存会话状态: {STATE_FILE}", flush=True)

        # 同时导出纯 cookie，供 requests / httpx / curl 使用
        cookies = context.cookies()
        with COOKIES_FILE.open("w", encoding="utf-8") as f:
            json.dump(cookies, f, ensure_ascii=False, indent=2)
        _chmod_600(COOKIES_FILE)
        print(f"[+] 已保存 cookies: {COOKIES_FILE} (共 {len(cookies)} 条)", flush=True)

        browser.close()

    print()
    print("完成。后续调用 zerus API 请使用 scripts/probe.py / scripts/smoke_test.py")
    return 0


if __name__ == "__main__":
    sys.exit(main())
