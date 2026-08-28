"""zerus API §1/§2/§3 单点抓样脚本。

用途：给 API.md 补充字段样例、给 smoke_test.py 准备 bk_params 缓存。
本脚本只发只读请求（services_query / service_detail / bk_params），
不会创建 / 修改 zerus 上任何资源。

用法：
  # 默认：跑 §1 services_query + 前 2 个服务的 §2 详情
  python scripts/probe.py

  # 只探某个服务详情
  python scripts/probe.py --service-id 711

  # 探某个服务的某个 meta 的 §3 params
  python scripts/probe.py --service-id 711 --meta-id 1394

  # 默认流程结束后，对每个抓到的服务，遍历其 serviceMetas 逐个探 bk_params
  python scripts/probe.py --bk-params

原始响应会写到 samples/_scratch/*.json（gitignored），
如果发现有新样本值得作为契约参考，手动挪到 samples/ 顶层并起个规范名。
"""
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from zerus_api import (  # noqa: E402
    SAMPLES_DIR,
    DEFAULT_PROJECT_ID,
    PreflightError,
    get_bk_params,
    get_service_detail,
    preflight,
    query_services,
    render_manual_fallback,
    shape,
)


SCRATCH_DIR = SAMPLES_DIR / "_scratch"


def _save(data: dict, filename: str) -> Path:
    SCRATCH_DIR.mkdir(parents=True, exist_ok=True)
    path = SCRATCH_DIR / filename
    path.write_text(json.dumps(data, ensure_ascii=False, indent=2), encoding="utf-8")
    return path


def _report(data: dict, label: str) -> None:
    print(
        f"--- {label} top-level keys ---",
        list(data.keys()) if isinstance(data, dict) else type(data),
        file=sys.stderr,
    )
    print("--- shape ---", file=sys.stderr)
    print(json.dumps(shape(data), ensure_ascii=False, indent=2), file=sys.stderr)


def probe_services(cookie: str, project_id: int) -> list[dict]:
    print("=" * 60, file=sys.stderr)
    print(f"[§1] services_query project={project_id}", file=sys.stderr)
    data = query_services(cookie, project_id)
    path = _save(data, "services_query.json")
    _report(data, "services_query")
    print(f"[+] saved to {path}", file=sys.stderr)

    services = ((data.get("data") or {}).get("list")) or []
    print(f"--- total services: {len(services)} ---", file=sys.stderr)
    for i, s in enumerate(services[:3]):
        print(f"\n--- sample service[{i}] full ---", file=sys.stderr)
        print(json.dumps(s, ensure_ascii=False, indent=2), file=sys.stderr)
    return services


def probe_service_detail(cookie: str, project_id: int, service_id: int) -> dict:
    print("\n" + "=" * 60, file=sys.stderr)
    print(f"[§2] service_detail sid={service_id}", file=sys.stderr)
    data = get_service_detail(cookie, service_id, project_id)
    path = _save(data, f"service_{service_id}.json")
    _report(data, f"service#{service_id}")
    print(f"[+] saved to {path}", file=sys.stderr)
    return data


def probe_bk_params(
    cookie: str,
    project_id: int,
    service_id: int,
    meta_id: int,
) -> dict:
    print("\n" + "=" * 60, file=sys.stderr)
    print(f"[§3] bk_params sid={service_id} mid={meta_id}", file=sys.stderr)
    data = get_bk_params(cookie, service_id, meta_id, project_id)
    path = _save(data, f"bk_params_s{service_id}_m{meta_id}.json")
    _report(data, f"bk_params#{service_id}/{meta_id}")
    print(f"[+] saved to {path}", file=sys.stderr)
    return data


def main() -> int:
    parser = argparse.ArgumentParser(description="zerus API §1/§2/§3 抓样")
    parser.add_argument("--project-id", type=int, default=DEFAULT_PROJECT_ID)
    parser.add_argument(
        "--service-id", type=int, default=None,
        help="只探某个 service；不填则先跑 services_query 再抓前 2 个详情",
    )
    parser.add_argument(
        "--meta-id", type=int, default=None,
        help="配合 --service-id 使用：探 §3 bk_params",
    )
    parser.add_argument(
        "--bk-params", action="store_true",
        help="默认流程结束后，对每个抓到的服务遍历 serviceMetas 抓 bk_params",
    )
    args = parser.parse_args()

    try:
        cookie = preflight()
    except PreflightError as e:
        # probe 是只读脚本，无服务清单可给，只输出网址 + 报错原因
        print(render_manual_fallback(e, project_id=args.project_id), file=sys.stderr)
        return 10
    print(f"[+] preflight ok, cookie loaded ({len(cookie)} chars)", file=sys.stderr)

    # 单点模式：service_id + meta_id → §3
    if args.service_id is not None and args.meta_id is not None:
        probe_bk_params(cookie, args.project_id, args.service_id, args.meta_id)
        return 0

    # 单点模式：仅 service_id → §2
    if args.service_id is not None:
        probe_service_detail(cookie, args.project_id, args.service_id)
        return 0

    # 默认：§1 + 前 2 个 §2
    services = probe_services(cookie, args.project_id)
    if not services:
        return 3

    for s in services[:2]:
        sid = s.get("id")
        if sid is None:
            continue
        detail = probe_service_detail(cookie, args.project_id, sid)
        if args.bk_params:
            metas = ((detail or {}).get("data") or {}).get("serviceMetas") or []
            for m in metas:
                mid = m.get("id")
                if mid is None:
                    continue
                probe_bk_params(cookie, args.project_id, sid, mid)

    print(f"\n[+] all responses saved under {SCRATCH_DIR}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
