"""
收集 create-zerus 需要的 payload 草稿。

扫描 `src/` 下的每个 git 仓库，读取：
- 当前分支名（HEAD）
- 最近一次 commit sha（短哈希）
- 该分支是否已 push 到 origin
- 与基线分支的差异（可选，用来判断这个仓库到底"有没有改"）

产出 payload JSON，字段与 run_automation.py 的 build_create_env_body 约定一致。

用法：
    # 默认扫描 <workspace>/src/*，输出到 stdout
    python collect_change_context.py \
        --workspace /Users/xzh/Project/GolandProjects/CoSpec \
        --story-id 134389917 \
        --slug dashboard-online-palm-statistics

    # 直接管道给 run_automation.py
    python collect_change_context.py --workspace ... --story-id ... --slug ... \
        | python run_automation.py create-env --payload -

约定：
- 仅纳入"当前分支不是基线分支"的仓库作为 `changes`（即真正有 feature 变更的）
- 基线分支表来自 --baseline-map 参数（JSON 文件）或默认 map（下方 DEFAULT_BASELINE_MAP）
- 未 push 到远端的分支会被标记 `pushed: false` —— skill 层据此在 Step 3 阻断创建
"""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path
from typing import Any

# 与 docs/git-workflow.md baseline 表保持一致。TBD 项等团队补齐后再更新。
DEFAULT_BASELINE_MAP: dict[str, str] = {
    "palm_local": "develop",
    "palm_proto": "master",
    "proto": "master",
    "palm_saas_fe": "develop",
    # 其余仓库 baseline 待补齐（docs/git-workflow.md 中标 [TBD] 的）
}


def _run_git(repo: Path, *args: str) -> str:
    """在 repo 目录下跑 git 命令，返回 stdout。失败返回空串。"""
    try:
        out = subprocess.check_output(
            ["git", "-C", str(repo), *args],
            stderr=subprocess.DEVNULL,
            text=True,
        )
    except (subprocess.CalledProcessError, FileNotFoundError):
        return ""
    return out.strip()


def _is_git_repo(path: Path) -> bool:
    return (path / ".git").exists()


def _current_branch(repo: Path) -> str:
    return _run_git(repo, "branch", "--show-current")


def _current_commit(repo: Path) -> str:
    return _run_git(repo, "rev-parse", "--short=7", "HEAD")


def _remote_has_branch(repo: Path, branch: str) -> bool:
    """检查 origin 上是否存在该分支。"""
    if not branch:
        return False
    out = _run_git(repo, "ls-remote", "--heads", "origin", branch)
    return bool(out.strip())


def _origin_url(repo: Path) -> str:
    """把 origin URL 归一为 group/project 形式，便于填 payload.repo。"""
    url = _run_git(repo, "config", "--get", "remote.origin.url")
    if not url:
        return ""
    # 支持形式：
    #   git@git.woa.com:group/project.git
    #   https://git.woa.com/group/project.git
    #   https://git.woa.com/group/subgroup/project
    stripped = url
    if stripped.endswith(".git"):
        stripped = stripped[:-4]
    if stripped.startswith("git@"):
        # git@host:group/project
        _, _, tail = stripped.partition(":")
        return tail
    if "://" in stripped:
        # protocol://host/group/project
        _, _, tail = stripped.partition("://")
        _, _, path = tail.partition("/")
        return path
    return stripped


def collect(
    src_dir: Path,
    baseline_map: dict[str, str],
) -> list[dict[str, Any]]:
    """扫描 src_dir 下的每个仓库，产出 changes 列表（仅含 feature 分支的仓库）。"""
    changes: list[dict[str, Any]] = []
    if not src_dir.is_dir():
        print(f"[!] {src_dir} 不存在或不是目录", file=sys.stderr)
        return changes

    for entry in sorted(src_dir.iterdir()):
        if not entry.is_dir() or not _is_git_repo(entry):
            continue
        service = entry.name
        branch = _current_branch(entry)
        baseline = baseline_map.get(service)
        # 仅当当前分支不等于基线（或基线未知但分支名带 feature/hotfix 前缀）时视为变更仓库
        is_feature = bool(branch) and (
            (baseline and branch != baseline)
            or branch.startswith(("feature/", "hotfix/"))
        )
        if not is_feature:
            continue

        commit = _current_commit(entry)
        pushed = _remote_has_branch(entry, branch)
        repo_slug = _origin_url(entry)

        changes.append(
            {
                "service": service,
                "repo": repo_slug,
                "branch": branch,
                "commit": commit,
                "baseline": baseline,
                "pushed": pushed,
            }
        )
    return changes


def load_baseline_map(path: Path | None) -> dict[str, str]:
    if path is None:
        return dict(DEFAULT_BASELINE_MAP)
    if not path.is_file():
        print(f"[!] --baseline-map 指向的文件不存在: {path}", file=sys.stderr)
        sys.exit(1)
    with path.open("r", encoding="utf-8") as f:
        data = json.load(f)
    if not isinstance(data, dict) or not all(
        isinstance(k, str) and isinstance(v, str) for k, v in data.items()
    ):
        print(f"[!] --baseline-map 必须是 {{service: baseline}} 的 JSON 对象", file=sys.stderr)
        sys.exit(1)
    return data


def _build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        prog="collect_change_context",
        description="扫描 src/* 下的仓库，产出 create-zerus payload 草稿",
    )
    p.add_argument(
        "--workspace",
        required=True,
        help="工作区根目录（其下应有 src/<repo>/）",
    )
    p.add_argument(
        "--src-dir",
        default=None,
        help="覆盖默认的 src 目录（默认为 <workspace>/src）",
    )
    p.add_argument("--story-id", default="", help="工蜂 story id，填入 payload.story_id")
    p.add_argument(
        "--slug",
        default="",
        help="spec slug（feature 名短语），用于生成默认 env_name",
    )
    p.add_argument("--env-name", default="", help="显式指定环境名，覆盖默认")
    p.add_argument("--notes", default="", help="备注，填入 payload.notes")
    p.add_argument(
        "--baseline-map",
        default=None,
        help="JSON 文件路径，覆盖默认的 baseline 映射；不指定时用内置默认",
    )
    return p


def main(argv: list[str] | None = None) -> int:
    args = _build_parser().parse_args(argv)
    workspace = Path(args.workspace).expanduser().resolve()
    src_dir = Path(args.src_dir).expanduser().resolve() if args.src_dir else workspace / "src"
    baseline_map = load_baseline_map(
        Path(args.baseline_map).expanduser().resolve() if args.baseline_map else None
    )

    changes = collect(src_dir, baseline_map)

    env_name = args.env_name
    if not env_name:
        parts = [p for p in ("spec", args.story_id, args.slug) if p]
        env_name = "-".join(parts) or "zerus-env"

    payload = {
        "env_name": env_name,
        "story_id": args.story_id or None,
        "changes": changes,
        "notes": args.notes or None,
    }

    # 便于人工审阅：把统计信息打到 stderr，payload 打到 stdout
    print(
        f"[+] 扫描 {src_dir}: 变更仓库 {len(changes)} 个",
        file=sys.stderr,
    )
    for c in changes:
        pushed_mark = "✅" if c["pushed"] else "❌ (未 push)"
        print(
            f"    - {c['service']:<20} {c['branch']:<50} @{c['commit']}  {pushed_mark}",
            file=sys.stderr,
        )

    json.dump(payload, sys.stdout, ensure_ascii=False, indent=2)
    sys.stdout.write("\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
