#!/usr/bin/env python3
"""
tools/plot.py — MiniLab 统一画图
从 out/csv/*.csv 读数据，画 PNG 到 out/plots/。

用法：
  python3 tools/plot.py all                        # 画全部
  python3 tools/plot.py <kind> <csv> <out_png>     # 画单张
    kind ∈ {sched, vm, coro, lock, ipc}
"""
import os
import sys
from pathlib import Path

try:
    import pandas as pd
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
except ImportError:
    print("missing deps: pip install pandas matplotlib", file=sys.stderr)
    sys.exit(1)


def plot_sched(csv, out):
    df = pd.read_csv(csv)
    metrics = ["avg_turnaround", "avg_response", "avg_wait"]
    fig, ax = plt.subplots(figsize=(10, 5))
    x = range(len(df))
    w = 0.25
    for i, m in enumerate(metrics):
        ax.bar([v + i * w for v in x], df[m], width=w, label=m)
    ax.set_xticks([v + w for v in x])
    ax.set_xticklabels(df["algo"])
    ax.set_ylabel("time (ms)")
    ax.set_title("Box ③ Scheduler Comparison")
    ax.legend()
    ax.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(out, dpi=120)
    plt.close()


def plot_vm(csv, out):
    df = pd.read_csv(csv)
    fig, ax = plt.subplots(figsize=(10, 5))
    for algo in df["algo"].unique():
        sub = df[df["algo"] == algo].sort_values("frames")
        ax.plot(sub["frames"], sub["hit_rate"], marker="o", label=algo)
    ax.set_xlabel("frames")
    ax.set_ylabel("hit rate")
    ax.set_title("Box ⑦ Page Replacement — Hit Rate vs Frames")
    ax.legend()
    ax.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(out, dpi=120)
    plt.close()


def plot_coro(csv, out):
    df = pd.read_csv(csv).sort_values("n")
    fig, ax = plt.subplots(figsize=(10, 5))
    ax.plot(df["n"], df["elapsed_ms"], marker="o", color="tab:blue",
            label="elapsed_ms")
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_xlabel("coroutine count N")
    ax.set_ylabel("elapsed (ms)")
    ax.set_title("Box ② Coroutine Scaling")
    ax.grid(True, which="both", alpha=0.3)
    plt.tight_layout()
    plt.savefig(out, dpi=120)
    plt.close()


def plot_lock(csv, out):
    df = pd.read_csv(csv)
    agg = df.groupby("mode")["qps"].mean().reset_index()
    fig, ax = plt.subplots(figsize=(8, 5))
    colors = {"bug": "red", "order": "green", "parity": "blue", "trylock": "orange"}
    ax.bar(agg["mode"], agg["qps"],
           color=[colors.get(m, "gray") for m in agg["mode"]])
    ax.set_ylabel("meals/sec (higher = better)")
    ax.set_title("Box ⑤ Deadlock vs 3 Fixes")
    ax.grid(True, alpha=0.3, axis="y")
    for i, v in enumerate(agg["qps"]):
        ax.text(i, v, f"{v:.0f}", ha="center", va="bottom")
    plt.tight_layout()
    plt.savefig(out, dpi=120)
    plt.close()


def plot_ipc(csv, out):
    df = pd.read_csv(csv)
    bw  = df[df["metric"] == "bandwidth"]
    lat = df[df["metric"] == "latency"]
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
    if not bw.empty:
        ax1.bar(bw["ipc"], bw["value"], color="tab:blue")
        ax1.set_ylabel("MB/s"); ax1.set_title("Bandwidth"); ax1.set_yscale("log")
    if not lat.empty:
        ax2.bar(lat["ipc"], lat["value"], color="tab:orange")
        ax2.set_ylabel("us/rtt"); ax2.set_title("Pingpong Latency")
    plt.suptitle("Box ④ IPC Benchmark")
    plt.tight_layout()
    plt.savefig(out, dpi=120)
    plt.close()


PLOTS = {
    "sched": ("box_sched.csv", "box_sched.png", plot_sched),
    "vm":    ("box_vm.csv",    "box_vm.png",    plot_vm),
    "coro":  ("box_coro.csv",  "box_coro.png",  plot_coro),
    "lock":  ("box_lock.csv",  "box_lock.png",  plot_lock),
    "ipc":   ("box_ipc.csv",   "box_ipc.png",   plot_ipc),
}


def plot_all():
    csv_dir = Path("out/csv")
    out_dir = Path("out/plots")
    out_dir.mkdir(parents=True, exist_ok=True)
    for kind, (csv, png, fn) in PLOTS.items():
        c = csv_dir / csv
        o = out_dir / png
        if not c.exists():
            print(f"skip {kind}: {c} not found")
            continue
        try:
            fn(c, o)
            print(f"wrote {o}")
        except Exception as e:
            print(f"plot {kind} failed: {e}", file=sys.stderr)


if __name__ == "__main__":
    if len(sys.argv) == 2 and sys.argv[1] == "all":
        plot_all()
    elif len(sys.argv) == 4:
        kind, csv, out = sys.argv[1], sys.argv[2], sys.argv[3]
        if kind not in PLOTS:
            print(f"unknown kind: {kind}; options: {list(PLOTS.keys())}")
            sys.exit(1)
        _, _, fn = PLOTS[kind]
        fn(csv, out)
        print(f"wrote {out}")
    else:
        print(__doc__)
        sys.exit(1)
