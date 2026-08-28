"""一键装 create-zerus skill 的运行依赖（venv + pip 包 + playwright chromium）。

设计原则：
- 有副作用（改 fs / 下载几百 MB），必须先跟用户确认；除非显式 --yes。
- 幂等：venv 已存在 / 包已装 / chromium 已下载都会跳过。
- 不做系统层安装（curl / python 自身），只在用户已有 python3 的前提下装 skill 依赖。
- 输出全走 stderr，方便被主流程 tee/日志。

用法：
    python3 scripts/bootstrap.py           # 交互确认后执行
    python3 scripts/bootstrap.py --yes     # 跳过确认，直接装
    python3 scripts/bootstrap.py --dry-run # 只打印将执行的命令，不改环境

退出码：
    0  成功（或全部已就绪）
    1  用户拒绝
    2  执行失败（会打印失败步骤）
    3  环境不满足（python 版本 / 无法建 venv）
"""
from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
VENV_DIR = SCRIPT_DIR / ".venv"
REQUIREMENTS = SCRIPT_DIR / "requirements.txt"

MIN_PY = (3, 9)


def _log(msg: str) -> None:
    print(msg, file=sys.stderr)


def _venv_python() -> Path:
    """venv 里的 python 可执行路径（跨平台）。"""
    if sys.platform == "win32":
        return VENV_DIR / "Scripts" / "python.exe"
    return VENV_DIR / "bin" / "python"


def _run(cmd: list[str], *, dry_run: bool) -> int:
    _log(f"    $ {' '.join(cmd)}")
    if dry_run:
        return 0
    return subprocess.run(cmd, check=False).returncode


def _check_python() -> None:
    if sys.version_info < MIN_PY:
        _log(f"[!] 需要 Python {MIN_PY[0]}.{MIN_PY[1]}+，当前是 {sys.version.split()[0]}")
        sys.exit(3)


def _venv_ready() -> bool:
    return _venv_python().exists()


def _pip_packages_ok() -> bool:
    """粗判：能 import playwright 和 requests 就算装好。"""
    py = _venv_python()
    if not py.exists():
        return False
    r = subprocess.run(
        [str(py), "-c", "import playwright, requests"],
        capture_output=True, text=True, check=False,
    )
    return r.returncode == 0


def _chromium_installed() -> bool:
    """粗判：playwright 能定位到 chromium executable。"""
    py = _venv_python()
    if not py.exists():
        return False
    # 使用 playwright 自带的 API 判断是否已安装
    code = (
        "from playwright.sync_api import sync_playwright;"
        "import sys;"
        "p = sync_playwright().start();"
        "path = p.chromium.executable_path;"
        "sys.exit(0 if path and __import__('os').path.exists(path) else 1)"
    )
    r = subprocess.run(
        [str(py), "-c", code], capture_output=True, text=True, check=False, timeout=15,
    )
    return r.returncode == 0


def _plan() -> list[tuple[str, list[str]]]:
    """返回要执行的步骤列表 [(标签, 命令)]，跳过已就绪的。"""
    steps: list[tuple[str, list[str]]] = []

    if not _venv_ready():
        steps.append((
            f"创建 venv：{VENV_DIR}",
            [sys.executable, "-m", "venv", str(VENV_DIR)],
        ))

    # pip install 总是跑一次（幂等；未装则装，已装则秒过）
    py = str(_venv_python())
    if not _pip_packages_ok():
        steps.append((
            f"pip 安装依赖：{REQUIREMENTS.name}",
            [py, "-m", "pip", "install", "-r", str(REQUIREMENTS)],
        ))

    if not _chromium_installed():
        steps.append((
            "playwright install chromium（几百 MB 下载）",
            [py, "-m", "playwright", "install", "chromium"],
        ))

    return steps


def _confirm(steps: list[tuple[str, list[str]]]) -> bool:
    _log("=" * 60)
    _log("[create-zerus] 安装依赖 —— 即将执行以下步骤：")
    _log("=" * 60)
    for i, (label, cmd) in enumerate(steps, 1):
        _log(f"  {i}. {label}")
        _log(f"     $ {' '.join(cmd)}")
    _log("")
    # 按实际计划动态描述影响
    impacts: list[str] = []
    labels_joined = " | ".join(lbl for lbl, _ in steps)
    if "创建 venv" in labels_joined:
        impacts.append(f"在 {VENV_DIR} 创建虚拟环境")
    if "pip 安装" in labels_joined:
        impacts.append("pip 装 playwright + requests（约 30 MB）")
    if "playwright install chromium" in labels_joined:
        impacts.append("下载 chromium 浏览器约 150~300 MB")
    if impacts:
        _log("影响：" + "；".join(impacts) + "。")
        _log("")
    try:
        answer = input("确认执行？[y/N] ").strip().lower()
    except EOFError:
        return False
    return answer in ("y", "yes")


def main() -> int:
    parser = argparse.ArgumentParser(description="create-zerus skill 一键装依赖")
    parser.add_argument("--yes", "-y", action="store_true", help="跳过交互确认")
    parser.add_argument("--dry-run", action="store_true", help="只打印，不执行")
    args = parser.parse_args()

    _check_python()

    if not REQUIREMENTS.exists():
        _log(f"[!] 找不到 {REQUIREMENTS}")
        return 3

    steps = _plan()
    if not steps:
        _log("[+] 所有依赖已就绪，无需安装。")
        _log(f"    venv:       {VENV_DIR}")
        _log(f"    激活方式：  source {VENV_DIR}/bin/activate")
        return 0

    if not args.yes and not args.dry_run:
        if not _confirm(steps):
            _log("[!] 已取消。")
            return 1
    elif args.dry_run:
        _log("[dry-run] 将执行以下步骤（不实际执行）：")
        for label, cmd in steps:
            _log(f"  - {label}")
            _log(f"    $ {' '.join(cmd)}")
        return 0

    _log("")
    _log("[+] 开始安装...")
    for i, (label, cmd) in enumerate(steps, 1):
        _log(f"\n[{i}/{len(steps)}] {label}")
        rc = _run(cmd, dry_run=False)
        if rc != 0:
            _log(f"[!] 步骤失败 (rc={rc})：{label}")
            _log("    修复后可重跑本脚本；已完成的步骤会自动跳过。")
            return 2

    _log("")
    _log("=" * 60)
    _log("[+] 依赖安装完成。")
    _log(f"    venv:      {VENV_DIR}")
    _log(f"    激活方式： source {VENV_DIR}/bin/activate")
    _log("    下一步：  python scripts/login_and_save.py  # 登录 zerus")
    _log("=" * 60)
    return 0


if __name__ == "__main__":
    sys.exit(main())
