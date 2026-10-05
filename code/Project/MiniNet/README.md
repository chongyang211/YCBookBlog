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
>>> tcp connect 10.0.0.2:8080         # TCP 三次握手 (peer 自动跑 echo server)
>>> tcp show                          # 当前所有 TCB
>>> quit

# 2. 独立诊断工具
./bin/mnet-ping -c 4 10.0.0.2

# 3. 🔥 三个造 BUG 现场
make wire-demo          # BUG-1 大小端
make bug2-demo          # BUG-2 子网/路由不一致
make bug3-demo          # BUG-3 丢包不重传

# 4. TCP 握手挥手测试
make demo-tcp           # 4 个端到端测试
```

## 当前进度

**第 1 次会话**（已完成）：
- ✅ ① Stack 骨架 + REPL
- ✅ ② 寻址 & 子网 + 最长前缀路由
- ✅ ③ 报文编解码 + 🔥BUG-1 大小端

**第 2 次会话**（已完成）：
- ✅ ④ ARP + 以太帧 + Loopback Driver + 60s 老化表
- ✅ ⑤ IPv4 + ICMP + mnet-ping + 🔥BUG-2 子网/路由

**第 3 次会话**（已完成）：
- ✅ ⑥ TCP 握手挥手 11 态状态机 (CLOSED→SYN_SENT→ESTABLISHED→FIN_WAIT→TIME_WAIT)
- ✅ ⑦ TCP 滑窗 + 拥塞 + RTO 重传 + 快速重传 + 🔥BUG-3

```bash
make test               # 所有包测试全绿
# ok  mininet/pkg/common       (9 tests)
# ok  mininet/pkg/link         (11 tests)
# ok  mininet/pkg/net          (12 tests)
# ok  mininet/pkg/transport    (13 tests)   ← 第 3 次新增
```

## 项目结构（第 3 次会话结束）

```text
MiniNet/
├── go.mod · Makefile · README.md · .gitignore
├── cmd/
│   ├── mnet/                           REPL
│   │   ├── main.go                     dump/route/peer/arp/ping
│   │   └── tcp.go                      tcp listen/connect/show (本次新增)
│   └── mnet-ping/main.go               独立 ping 诊断工具
├── pkg/
│   ├── common/                         公共:日志/hex/大端/校验和 (9 测试)
│   ├── link/                           L2:链路层 (11 测试)
│   │   ├── driver.go · loopback.go
│   │   ├── ethernet.go · arp.go · l2.go · timer.go
│   ├── net/                            L3:网络层 (12 测试)
│   │   ├── subnet.go · route.go
│   │   ├── ipv4.go · icmp.go · l3.go   (l3.go 本次加 OnTCP/SendIP)
│   └── transport/                      L4:TCP (13 测试)   ◀── 本次新增包
│       ├── seq.go                      TCP seqnum 环绕算术
│       ├── tcp_header.go               TCP 头编解码 + 伪头 checksum
│       ├── state.go                    11 态枚举
│       ├── tcb.go                      TCB 结构
│       ├── l4.go                       L4Layer + Dial/Listen
│       ├── sm.go                       状态机核心
│       ├── conn.go                     Conn (Read/Write/Close)
│       ├── retrans.go                  RTO + 拥塞 + 重传队列
│       ├── seq_test.go
│       ├── tcp_header_test.go
│       ├── e2e_test.go                 握手/数据/挥手/大 payload
│       └── bug3_test.go                🔥 BUG-3 丢包重传
└── tests/
    ├── wire_demo/main.go               🔥 BUG-1 大小端
    ├── bug2_demo/main.go               🔥 BUG-2 子网/路由
    └── bug3_demo/main.go               🔥 BUG-3 丢包不重传   ◀── 本次新增
```

**代码规模**：~4700 行 Go，45 个测试。

## 后续路线

| 会话 | 阶段 | 产出 |
|------|------|------|
| 4 | ⑧⑨ epoll + DNS | 并发 server + 递归解析 |
| 5 | ⑩⑪ TLS + HTTP | 自己的 mnet-curl |
| 6 | ⑫⑬⑭ 代理+缓存+WS | 迷你 Nginx + 聊天室 |
| 7 | ⑮⑯ QUIC + 总装 | 全链路 shop demo |
