# MiniNet · 迷你网络栈

> 《网络协议原理》综合案例 · Go 从 0 到 1 徒手造一台"会讲协议"的机器
>
> 配套文档：`packages/website/04.计算机/02.网络协议原理/21.MiniNet迷你网络栈综合案例.md`

## 一句话定位

从一个 `socketpair` 字节流开始，**15 个阶段 · 58 个 Step**，徒手造一台既是客户端又是服务端的迷你网络栈。

## 快速开始

```bash
make build          # 编译 bin/mnet · bin/mnet-ping

# 1. REPL 玩法
./bin/mnet
>>> dump                              # 5 层协议栈当前状态
>>> peer up                           # 启动对端 goroutine
>>> arp whois 10.0.0.2                # 主动 ARP
>>> ping 10.0.0.2 4                   # 发 4 个 ICMP echo
>>> route add 10.0.0.0/8 via 10.0.0.1
>>> route lookup 10.1.2.3
>>> arp -a
>>> quit

# 2. 独立诊断工具
./bin/mnet-ping -c 4 10.0.0.2

# 3. 🔥 造 BUG 高峰现场
make wire-demo          # BUG-1 大小端
make bug2-demo          # BUG-2 子网/路由不一致
```

## 当前进度

**第 1 次会话**（已完成）：
- ✅ 阶段 ① Stack 骨架 + REPL
- ✅ 阶段 ② 寻址 & 子网 + 最长前缀路由
- ✅ 阶段 ③ 报文编解码 + 🔥BUG-1 大小端修复

**第 2 次会话**（已完成）：
- ✅ 阶段 ④ ARP + 以太帧 + Loopback Driver + ArpTable 老化
- ✅ 阶段 ⑤ IPv4 + ICMP + mnet-ping + 🔥BUG-2 子网/路由现场

```bash
make test               # 所有包测试全绿
# ok  mininet/pkg/common    (9 tests)
# ok  mininet/pkg/link      (11 tests)
# ok  mininet/pkg/net       (12 tests)
```

## 项目结构（第 2 次会话结束）

```text
MiniNet/
├── go.mod · Makefile · README.md · .gitignore
├── cmd/
│   ├── mnet/main.go                # REPL (dump/route/peer/arp/ping)
│   └── mnet-ping/main.go           # 独立 ping 工具
├── pkg/
│   ├── common/                     # 日志 · hex · 大端 · 校验和
│   │   ├── log.go · hex.go · bigendian.go · checksum.go
│   │   └── *_test.go (9 测试)
│   ├── link/                       # L2:链路层
│   │   ├── driver.go               # Driver 接口
│   │   ├── loopback.go             # Loopback 实现(chan 交叉)
│   │   ├── ethernet.go             # MAC + 以太帧
│   │   ├── arp.go                  # ARP 协议 + ArpTable 老化
│   │   ├── l2.go                   # L2Layer (Driver+ARP+Demuxer)
│   │   ├── timer.go                # 内部辅助
│   │   └── *_test.go (11 测试)
│   └── net/                        # L3:网络层
│       ├── subnet.go               # IPv4Addr + Subnet
│       ├── route.go                # RouteTable + 最长前缀匹配
│       ├── ipv4.go                 # IPv4 头编解码
│       ├── icmp.go                 # ICMP Echo
│       ├── l3.go                   # L3Layer (IPv4+ICMP+Ping)
│       └── *_test.go (12 测试)
└── tests/
    ├── wire_demo/main.go           # 🔥 BUG-1 对比
    └── bug2_demo/main.go           # 🔥 BUG-2 对比
```

**代码规模**：约 2200 行 Go，32 个测试。

## 后续路线

| 会话 | 阶段 | 产出 |
|------|------|------|
| 3 | ⑥⑦ TCP 握手 + 滑窗 | 自己写的 TCP 栈 |
| 4 | ⑧⑨ epoll + DNS | 并发 server + 递归解析 |
| 5 | ⑩⑪ TLS + HTTP | 自己的 mnet-curl |
| 6 | ⑫⑬⑭ 代理+缓存+WS | 迷你 Nginx + 聊天室 |
| 7 | ⑮⑯ QUIC + 总装 | 全链路 shop demo |
