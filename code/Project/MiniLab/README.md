# MiniLab · 12 盒操作系统实验盒 · 配套代码

> 本仓库是文档 `packages/website/04.计算机/03.操作系统原理/12.MiniLab操作系统实验盒.md` 的配套代码骨架。
> 每盒独立可编译、独立可跑、独立可 commit。

## 快速开始

```bash
cd code/Project/MiniLab
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# 跑单盒
./build/bin/box_proc              # 盒 ① 进程状态机
./build/bin/box_coro 100000 2048  # 盒 ② 10w 协程
./build/bin/box_sched demo/sched/mixed.task all   # 盒 ③ 五种调度器
./build/bin/box_vm belady         # 盒 ⑦ Belady 异常
./build/bin/box_malloc            # 盒 ⑥ 迷你 malloc REPL
./build/bin/bench_pipe all        # 盒 ④ pipe benchmark
./build/bin/box_lock bug 3        # 盒 ⑤ 死锁复现

# 跑全套（~90s，自动画图）
cmake --build build --target demo-all
open out/report.html              # 聚合报告（Python 画图后）
```

## 目录结构

```
MiniLab/
├── CMakeLists.txt                顶层：12 盒公共设置 + fmt 依赖
├── README.md                     本文件
├── Vagrantfile                   Linux 虚拟机（盒 ⑩⑪⑫ 用）
├── Dockerfile                    Linux 容器兜底
├── common/                       12 盒共用的头
│   ├── repl.hpp
│   ├── log.hpp
│   ├── csv.hpp
│   └── timer.hpp
├── boxes/
│   ├── 01_proc/                  盒 ① 进程状态机
│   ├── 02_coro/                  盒 ② 用户态协程
│   ├── 03_sched/                 盒 ③ 五种调度器
│   ├── 04_ipc/                   盒 ④ IPC 压测
│   ├── 05_lock/                  盒 ⑤ 哲学家 + 银行家
│   ├── 06_malloc/                盒 ⑥ 迷你 malloc
│   ├── 07_vm/                    盒 ⑦ 页替换
│   ├── 08_fs/                    盒 ⑧ inode FS
│   ├── 09_io/                    盒 ⑨ select/poll/epoll
│   ├── 10_drv/                   盒 ⑩ 内核模块（独立 Makefile）
│   ├── 11_ns/                    盒 ⑪ unshare 假容器
│   └── 12_cgrp/                  盒 ⑫ cgroup v2
├── tools/
│   └── plot.py                   统一画图（matplotlib）
├── scripts/
│   └── demo-all.sh               一键跑全套
├── demo/                         各盒预制输入
└── out/                          运行产物（gitignored）
```

## 盒 ↔ 专栏对照

| 盒 | 文档章节 | 对应原理专栏 | 平台 |
|---|---|---|---|
| ① | §09 | §01 进程线程 | ✅ All |
| ② | §10 | §01 协程 | ✅ All (macOS 需 _XOPEN_SOURCE) |
| ③ | §11 | §02 调度策略 | ✅ All |
| ④ | §12 | §03 IPC | ✅ All (mq 仅 Linux) |
| ⑤ | §13 | §04 同步互斥 | ✅ All |
| ⑥ | §14 | §05 内存管理 | ✅ All |
| ⑦ | §15 | §06 虚拟内存 | ✅ All |
| ⑧ | §16 | §07 文件系统 | ✅ All |
| ⑨ | §17 | §08 IO 模型 | ⚠️ epoll 仅 Linux |
| ⑩ | §18 | §09 设备驱动 | 🐧 Linux only |
| ⑪ | §19 | §10 容器 | 🐧 Linux only |
| ⑫ | §20 | §10 容器 | 🐧 Linux only |

## 环境要求

- **CMake** ≥ 3.16
- **C++20 编译器**：GCC 11+ / Clang 14+ / Apple Clang 15+
- **Python 3** + `pandas` + `matplotlib`（仅画图用，不装也能跑）
- **Linux**（可选）：盒 ⑩⑪⑫ 需要；macOS 用户用 `vagrant up` 或 `docker build`

## 没 Linux？两条路

```bash
# A. Vagrant（推荐，接近真实）
vagrant up && vagrant ssh
cd /vagrant && cmake -B build && cmake --build build -j

# B. Docker
docker build -t minilab .
docker run --rm -it --privileged -v $PWD:/work minilab bash
```

## 调试

- 单步调试：`cmake --build build -DCMAKE_BUILD_TYPE=Debug`，`lldb ./build/bin/box_xxx`
- 内核模块（盒 ⑩）：`dmesg -w` 实时看日志
- cgroup（盒 ⑫）：`watch -n 0.5 cat /sys/fs/cgroup/mytest/memory.current`

## 更多

完整教程、灵魂三问、造 BUG 高峰、修复 benchmark 全在文档：

> `packages/website/04.计算机/03.操作系统原理/12.MiniLab操作系统实验盒.md`

本仓库只提供**可运行的最小骨架**，用来验证"编得过、跑得起、不同系统能移植"。深入解释、设计思路、踩坑经验在博客文档里。
