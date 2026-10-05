# MiniNet · 迷你网络栈

> 《网络协议原理》综合案例 · Go 从 0 到 1 徒手造一台"会讲协议"的机器
>
> 配套文档：`packages/website/04.计算机/02.网络协议原理/21.MiniNet迷你网络栈综合案例.md`

## 一句话定位

从一个 `socketpair` 字节流开始，**15 个阶段 · 58 个 Step**，徒手造一台既是客户端又是服务端的迷你网络栈。

## 快速开始

```bash
make build          # 编译 bin/mnet · bin/mnet-ping · bin/mnet-dig

# 1. REPL 玩法
./bin/mnet
>>> dump                              # 5 层协议栈当前状态
>>> peer up                           # 启动对端 goroutine
>>> arp whois 10.0.0.2                # 主动 ARP
>>> ping 10.0.0.2 4                   # 发 4 个 ICMP echo
>>> tcp connect 10.0.0.2:8080         # TCP 三次握手 + 发消息
>>> quit

# 2. 独立诊断工具
./bin/mnet-ping -c 4 10.0.0.2
./bin/mnet-dig @8.8.8.8 www.example.com         # 直接查 (本机需能联公网)
./bin/mnet-dig -recursive www.example.com       # 从根开始自己递归

# 3. 🔥 四个造 BUG 现场
make wire-demo          # BUG-1 大小端
make bug2-demo          # BUG-2 子网/路由不一致
make bug3-demo          # BUG-3 丢包不重传
make bug4-demo          # BUG-4 EPOLL ET 漏读 (Linux only)

# 4. 本地演示 (不依赖公网)
make dns-demo           # fake DNS + Resolver 缓存 + singleflight

# 5. 协议栈内部测试
make demo-tcp           # 握手挥手 4 个端到端
go test ./pkg/mux/ -bench=. -benchtime=1s      # reactor QPS 对比
```

## 当前进度

**第 1 次会话**（已完成）：
- ✅ ① Stack 骨架 + REPL
- ✅ ② 寻址 & 子网 + 最长前缀路由
- ✅ ③ 报文编解码 + 🔥BUG-1 大小端

**第 2 次会话**（已完成）：
- ✅ ④ ARP + 以太帧 + Loopback Driver + 60s 老化
- ✅ ⑤ IPv4 + ICMP + mnet-ping + 🔥BUG-2 子网/路由

**第 3 次会话**（已完成）：
- ✅ ⑥ TCP 握手挥手 11 态状态机
- ✅ ⑦ 滑窗 + Reno 拥塞 + RTO 重传 + 🔥BUG-3

**第 4 次会话**（已完成）：
- ✅ ⑧ Reactor + goroutine/epoll(LT+ET)/kqueue stub + 🔥BUG-4 ET 漏读
- ✅ ⑨ DNS 报文编解码 + 递归解析 + TTL 缓存 + singleflight + mnet-dig

```bash
make test               # 所有包测试全绿
# ok  mininet/pkg/common       (9  tests)
# ok  mininet/pkg/link         (11 tests)
# ok  mininet/pkg/net          (12 tests)
# ok  mininet/pkg/transport    (13 tests)
# ok  mininet/pkg/mux          (2  tests + 3 benchmark)   ← 本次新增
# ok  mininet/pkg/dns          (7  tests)                 ← 本次新增
```

## 项目结构（第 4 次会话结束）

```text
MiniNet/
├── go.mod · Makefile · README.md · .gitignore
├── cmd/
│   ├── mnet/                           REPL
│   ├── mnet-ping/                      独立 ping
│   └── mnet-dig/                       独立 dig    ◀── 本次新增
├── pkg/
│   ├── common/                         公共:日志/hex/大端/校验和
│   ├── link/                           L2:链路层 Driver+ARP
│   ├── net/                            L3:IPv4+ICMP+路由
│   ├── transport/                      L4:TCP (11 态状态机+Reno+RTO)
│   ├── mux/                           ◀── 本次新增 Reactor 事件循环
│   │   ├── reactor.go                  Reactor 接口 + goroutine 实现
│   │   ├── epoll_linux.go              Linux epoll (LT + ET + ET-bug)
│   │   ├── epoll_other.go              非 Linux 占位
│   │   ├── fdhelper_linux.go
│   │   ├── reactor_test.go
│   │   └── bench_test.go               goroutine vs epoll QPS 对比
│   └── dns/                           ◀── 本次新增 DNS 协议栈
│       ├── message.go                  RFC1035 编解码(含压缩指针)
│       ├── resolver.go                 递归解析 + singleflight
│       ├── cache.go                    TTL 缓存
│       ├── message_test.go
│       └── resolver_test.go            本地 fake DNS 端到端
└── tests/
    ├── wire_demo/                      🔥 BUG-1
    ├── bug2_demo/                      🔥 BUG-2
    ├── bug3_demo/                      🔥 BUG-3
    ├── bug4_demo/                     ◀── 本次新增 🔥 BUG-4
    └── dns_demo/                      ◀── 本次新增 DNS 本地演示
```

**代码规模**：~7100 行 Go，54 个测试。

## 后续路线

| 会话 | 阶段 | 产出 |
|------|------|------|
| 5 | ⑩⑪ TLS + HTTP | 自己的 mnet-curl |
| 6 | ⑫⑬⑭ 代理+缓存+WS | 迷你 Nginx + 聊天室 |
| 7 | ⑮⑯ QUIC + 总装 | 全链路 shop demo |
