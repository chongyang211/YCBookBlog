"""端到端冒烟：随机挑几个 zerus 服务 → 拉 §3 params → 组装多服务 body → §5 防重 → §4 创建 → §5 反查。

覆盖 SKILL.md Step 2~5 的核心路径，但不依赖 spec / plan，仅用来验证：
- API 层的 §1~§5 逻辑通路
- multi-service body 组装正确
- autoNew / GIT_BRANCH_PROJECT 覆盖行为符合契约

用法：
  # dry-run：走完随机挑 + §3 拉参 + 组装 body，只打印不发 §4
  python scripts/smoke_test.py --dry-run

  # 真跑（会真的在 zerus 上创建环境！默认 1h 后过期）
  python scripts/smoke_test.py --confirm-create

  # 换分支 / 采样数量 / 复现种子
  python scripts/smoke_test.py --confirm-create --branch master --sample 3 --seed 42

安全：
- 默认 endTime = now + 1h（探测完就过期）
- 默认 name = cospec-smoke-<user>-<ts>（唯一，避免撞其他人环境）
- 未加 --confirm-create 只 dry-run
"""
from __future__ import annotations

import argparse
import json
import random
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from zerus_api import (  # noqa: E402
    DEFAULT_PROJECT_ID,
    SAMPLES_DIR,
    LoginExpired,
    PreflightError,
    build_create_env_body,
    build_service_instance,
    create_env,
    default_env_name,
    extract_env_list,
    extract_params_list,
    get_bk_params,
    get_service_detail,
    iso_utc_after,
    preflight,
    query_envs,
    query_services,
    render_manual_fallback,
    shape,
)

# load_cookie_header 已被 preflight() 内部调用，不再直接使用


SCRATCH_DIR = SAMPLES_DIR / "_scratch"
CACHED_SERVICES = SAMPLES_DIR / "services_query.json"


def _save(data: dict, filename: str) -> Path:
    SCRATCH_DIR.mkdir(parents=True, exist_ok=True)
    path = SCRATCH_DIR / filename
    path.write_text(json.dumps(data, ensure_ascii=False, indent=2), encoding="utf-8")
    return path


def load_services(cookie: str, project_id: int) -> list[dict]:
    """先读 samples/services_query.json 缓存；没有就实时 §1。"""
    if CACHED_SERVICES.exists():
        with CACHED_SERVICES.open("r", encoding="utf-8") as f:
            raw = json.load(f)
        services = (raw.get("data") or {}).get("list") or []
        if services:
            services = [s for s in services if s.get("enable")]
            print(
                f"[+] 从 {CACHED_SERVICES.name} 加载了 {len(services)} 个 enable 服务",
                file=sys.stderr,
            )
            return services
    print("[+] samples 缓存不存在或为空，实时打 §1 services_query...", file=sys.stderr)
    resp = query_services(cookie, project_id)
    services = [s for s in ((resp.get("data") or {}).get("list") or []) if s.get("enable")]
    print(f"[+] 拉到 {len(services)} 个 enable 服务", file=sys.stderr)
    return services


def pick_candidates(
    cookie: str,
    project_id: int,
    services: list[dict],
    want: int,
    max_attempts: int,
) -> list[dict]:
    """随机 shuffle 服务列表，逐个 GET 详情 + §3 params。
    可用 = 至少一个 type=vt 的 serviceMeta 且 §3 能拉到含 GIT_BRANCH_PROJECT 的 params[]。

    Returns:
        [{sid, mid, name, meta_name, params}, ...]
    """
    picked: list[dict] = []
    tried: set[int] = set()
    pool = list(services)
    random.shuffle(pool)

    for s in pool:
        if len(picked) >= want:
            break
        if len(tried) >= max_attempts:
            print(
                f"[!] 已尝试 {len(tried)} 个候选，仍不足 {want} 个，"
                f"就用已有的 {len(picked)} 个继续",
                file=sys.stderr,
            )
            break

        sid = s.get("id")
        sname = s.get("name") or f"svc_{sid}"
        if not sid or sid in tried:
            continue
        tried.add(sid)

        print(f"\n[候选] 查 service {sid}/{sname} 详情...", file=sys.stderr)
        try:
            detail_resp = get_service_detail(cookie, sid, project_id)
        except SystemExit as e:
            print(f"    -> 详情失败：{e}，跳过", file=sys.stderr)
            continue

        detail = (detail_resp or {}).get("data") or {}
        metas = detail.get("serviceMetas") or []
        vt_metas = [m for m in metas if m.get("type") == "vt"]
        if not vt_metas:
            print(
                "    -> 无 type=vt 的 serviceMeta，跳过（视为 zerus 未接入构建）",
                file=sys.stderr,
            )
            continue

        meta = vt_metas[0]
        mid = meta.get("id")
        if not mid:
            print("    -> meta 无 id，跳过", file=sys.stderr)
            continue

        print(
            f"    -> serviceMeta {mid}/{meta.get('name')} 拉 §3 params...",
            file=sys.stderr,
        )
        try:
            params_resp = get_bk_params(cookie, sid, mid, project_id)
        except SystemExit as e:
            print(f"    -> §3 失败：{e}，跳过", file=sys.stderr)
            continue
        params = extract_params_list(params_resp)
        if not params:
            print("    -> §3 params 空，跳过", file=sys.stderr)
            continue

        if not any(p.get("id") == "GIT_BRANCH_PROJECT" for p in params):
            print("    -> params 里没有 GIT_BRANCH_PROJECT，无法覆盖分支，跳过", file=sys.stderr)
            continue

        picked.append({
            "sid": sid,
            "mid": mid,
            "name": sname,
            "meta_name": meta.get("name") or "",
            "params": params,
        })
        print(
            f"    ✓ 入选 #{len(picked)}: sid={sid} name={sname} "
            f"mid={mid} meta={meta.get('name')}",
            file=sys.stderr,
        )

    return picked


def main() -> int:
    parser = argparse.ArgumentParser(description="skill 端到端冒烟：多服务随机建环境")
    parser.add_argument("--project-id", type=int, default=DEFAULT_PROJECT_ID)
    parser.add_argument("--sample", type=int, default=2, help="想覆盖的服务个数，默认 2")
    parser.add_argument(
        "--max-attempts", type=int, default=6,
        help="随机试探候选上限，默认 6",
    )
    parser.add_argument(
        "--branch", default="master",
        help="所有服务的 GIT_BRANCH_PROJECT，默认 master",
    )
    parser.add_argument("--name", default=None, help="环境名，默认 cospec-smoke-<user>-<ts>")
    parser.add_argument("--hours", type=float, default=1.0, help="过期时间偏移，默认 1h")
    parser.add_argument("--desc", default="cospec skill smoke test，可随时删除")
    parser.add_argument("--seed", type=int, default=None, help="随机种子，方便复现")
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument(
        "--confirm-create", action="store_true",
        help="真发 §4 请求，会在 zerus 上创建环境",
    )
    parser.add_argument("--skip-precheck", action="store_true")
    args = parser.parse_args()

    random.seed(args.seed if args.seed is not None else int(time.time()))

    # 计划中的环境名 / 过期时间，preflight 失败时用来生成手动 fallback 提示
    planned_env_name = args.name or default_env_name(prefix="cospec-smoke")
    planned_end_time = iso_utc_after(args.hours)

    # ---- Preflight：curl+http2 → 网络 → cookie → 登录态 ----
    try:
        cookie = preflight()
    except PreflightError as e:
        print(
            render_manual_fallback(
                e,
                env_name=planned_env_name,
                end_time=planned_end_time,
                branch=args.branch,
                project_id=args.project_id,
            ),
            file=sys.stderr,
        )
        return 10
    print(f"[+] preflight ok, cookie loaded ({len(cookie)} chars)", file=sys.stderr)

    services = load_services(cookie, args.project_id)
    if not services:
        print("[!] 服务列表空 —— 中止", file=sys.stderr)
        return 3

    picked = pick_candidates(
        cookie, args.project_id, services,
        want=args.sample, max_attempts=args.max_attempts,
    )
    if not picked:
        print("[!] 没挑到任何可用服务，中止", file=sys.stderr)
        return 4

    print("\n" + "=" * 60, file=sys.stderr)
    print(f"[挑选完成] {len(picked)} 个服务：", file=sys.stderr)
    for i, p in enumerate(picked, 1):
        print(
            f"  {i}. sid={p['sid']:<5} mid={p['mid']:<5} "
            f"name={p['name']:<25} meta={p['meta_name']}",
            file=sys.stderr,
        )

    # 组装 body（沿用 preflight 前计算好的 name / endTime）
    env_name = planned_env_name
    end_time = planned_end_time
    instances = []
    for p in picked:
        inst = build_service_instance(
            service_id=p["sid"],
            meta_id=p["mid"],
            service_name=p["name"],
            branch=args.branch,
            params_template=p["params"],
            build_msg=f"[cospec-smoke] create env for {args.branch}",
            auto_new=False,
        )
        if inst is None:
            # pick_candidates 已校验过，正常不会走到
            print(f"[!] {p['name']} 组装失败（GIT_BRANCH_PROJECT 缺失），跳过", file=sys.stderr)
            continue
        instances.append(inst)

    body = build_create_env_body(
        name=env_name, end_time_iso=end_time, desc=args.desc,
        service_instances=instances,
    )

    print("\n" + "=" * 60, file=sys.stderr)
    print("[assembled body]", file=sys.stderr)
    print(f"  name      = {body['name']}", file=sys.stderr)
    print(f"  endTime   = {body['endTime']}", file=sys.stderr)
    print(f"  branch    = {args.branch}  (所有服务统一)", file=sys.stderr)
    print(f"  services  = {len(body['serviceInstanceList'])}", file=sys.stderr)
    for inst in body["serviceInstanceList"]:
        b_base = inst["buildInfo"].get("base")
        first_build = b_base.get("autoNew") if b_base else None
        # push 自动触发的真正开关是 GIT_BRANCH_PROJECT.auto
        params = ((inst.get("buildInfo") or {}).get("bk") or {}).get("params") or []
        gbp = next((p for p in params if p.get("id") == "GIT_BRANCH_PROJECT"), None)
        push_trigger = "on" if (gbp and gbp.get("auto")) else "off"
        print(
            f"    - sid={inst['serviceID']:<5} "
            f"mid={inst['serviceMetaID']:<5} "
            f"name={inst['name']:<25} "
            f"push-trigger={push_trigger} "
            f"first-build={first_build}",
            file=sys.stderr,
        )

    _save(body, "smoke_assembled_body.json")

    if not args.confirm_create:
        print(
            "\n[dry-run] 未加 --confirm-create，不发 §4 请求。审核 body 无误再真跑。",
            file=sys.stderr,
        )
        return 0

    # §5 同名防重
    if not args.skip_precheck:
        print("\n" + "=" * 60, file=sys.stderr)
        print(f"[§5] 创建前按 name={env_name!r} 精确查同名...", file=sys.stderr)
        try:
            precheck = query_envs(
                cookie, args.project_id,
                name_filter=env_name, type_filter="vt",
            )
        except LoginExpired as e:
            print(f"[!] {e}", file=sys.stderr)
            return 2
        _save(precheck, "smoke_envs_query_precheck.json")
        envs = extract_env_list(precheck)
        dup = [e for e in envs if e.get("name") == env_name]
        if dup:
            print(f"\n[!] 同名环境已存在 ({len(dup)} 条)，中止避免污染", file=sys.stderr)
            return 5
        print("[+] precheck ok，无同名环境", file=sys.stderr)

    # §4 创建
    print("\n" + "=" * 60, file=sys.stderr)
    print(f"[§4] POST create_env, services={len(body['serviceInstanceList'])}", file=sys.stderr)
    manual_services = [
        {"name": inst["name"], "branch": args.branch}
        for inst in body["serviceInstanceList"]
    ]
    try:
        resp = create_env(cookie, body, args.project_id)
    except LoginExpired as e:
        print(f"[!] {e}", file=sys.stderr)
        print(
            render_manual_fallback(
                PreflightError("cookie_expired", "运行过程中 cookie 过期", hint=str(e)),
                env_name=env_name, end_time=end_time,
                services=manual_services, branch=args.branch,
                project_id=args.project_id,
            ),
            file=sys.stderr,
        )
        return 2
    _save(resp, "smoke_create_env_response.json")
    print(json.dumps(shape(resp), ensure_ascii=False, indent=2), file=sys.stderr)

    if resp.get("code") != 0:
        print(f"\n[!] create_env code={resp.get('code')} message={resp.get('message')}", file=sys.stderr)
        print(
            render_manual_fallback(
                None,
                env_name=env_name, end_time=end_time,
                services=manual_services, branch=args.branch,
                project_id=args.project_id,
            ),
            file=sys.stderr,
        )
        return 6

    env_id = ((resp.get("data") or {}).get("id"))
    env_hash = ((resp.get("data") or {}).get("hashName"))
    print(f"\n[+] created env id={env_id} hashName={env_hash}", file=sys.stderr)

    # §5 反查
    print("\n" + "=" * 60, file=sys.stderr)
    print(f"[§5] 创建后按 name={env_name!r} 反查...", file=sys.stderr)
    try:
        after = query_envs(cookie, args.project_id, name_filter=env_name)
    except LoginExpired as e:
        print(f"[!] {e}", file=sys.stderr)
        return 2
    _save(after, "smoke_envs_query_after_create.json")

    print("\n[+] smoke test 完成。抓样写到 samples/_scratch/", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
