# MiniNet · 迷你网络栈

> 《网络协议原理》综合案例 · Go 从 0 到 1 徒手造一台"会讲协议"的机器
>
> 配套文档：`packages/website/04.计算机/02.网络协议原理/21.MiniNet迷你网络栈综合案例.md`

## 一句话定位

从一个 `socketpair` 字节流开始，**15 个阶段 · 58 个 Step**，徒手造一台既是客户端又是服务端的迷你网络栈。

## 快速开始

```bash
make build          # 编译 bin/mnet · bin/mnet-ping · bin/mnet-dig · bin/mnet-curl

# 1. REPL 玩法
./bin/mnet
>>> dump                              # 5 层协议栈当前状态
>>> peer up                           # 启动对端 goroutine
>>> ping 10.0.0.2 4                   # 发 4 个 ICMP echo
>>> tcp connect 10.0.0.2:8080         # TCP 三次握手 + 发消息
>>> quit

# 2. 四个独立诊断工具
./bin/mnet-ping -c 4 10.0.0.2
./bin/mnet-dig @8.8.8.8 www.example.com
./bin/mnet-curl -v http://httpbin.org/get                 # verbose
./bin/mnet-curl --tls-debug https://www.example.com/      # 看 ClientHello 字节

# 3. 🔥 四个造 BUG 现场
make wire-demo          # BUG-1 大小端
make bug2-demo          # BUG-2 子网/路由不一致
make bug3-demo          # BUG-3 丢包不重传
make bug4-demo          # BUG-4 EPOLL ET 漏读 (Linux only)

# 4. 本地演示 (不依赖公网)
make dns-demo           # DNS 缓存+singleflight
make http-demo          # HTTP server/client 全链路

# 5. 协议栈内部测试
make demo-tcp
go test ./pkg/mux/ -bench=. -benchtime=1s
```

## 当前进度

**第 1 次会话**（已完成）：
- ✅ ① Stack 骨架 + REPL
- ✅ ② 寻址 & 子网 + 最长前缀路由
- ✅ ③ 报文编解码 + 🔥BUG-1 大小端

**第 2 次会话**（已完成）：
- ✅ ④ ARP + 以太帧 + Loopback Driver + 60s 老化
- ✅ ⑤ IPv4 + ICMP + mnet-ping + 🔥BUG-2

**第 3 次会话**（已完成）：
- ✅ ⑥ TCP 握手挥手 11 态状态机
- ✅ ⑦ 滑窗 + Reno 拥塞 + RTO + 🔥BUG-3

**第 4 次会话**（已完成）：
- ✅ ⑧ Reactor + goroutine/epoll(LT+ET) + 🔥BUG-4
- ✅ ⑨ DNS 递归 + TTL 缓存 + singleflight + mnet-dig

**第 5 次会话**（已完成）：
- ✅ ⑩ TLS 1.3 握手可视化 (ClientHello 字节解码 + crypto/tls 包装)
- ✅ ⑪ HTTP/1.1 服务端 + 客户端 (自写 Request/Response/Mux/chunked)

```bash
make test               # 所有包测试全绿
# ok  mininet/pkg/common       (9  tests)
# ok  mininet/pkg/link         (11 tests)
# ok  mininet/pkg/net          (12 tests)
# ok  mininet/pkg/transport    (13 tests)
# ok  mininet/pkg/mux          (2  tests + 3 benchmark)
# ok  mininet/pkg/dns          (7  tests)
# ok  mininet/pkg/tls          (4  tests)   ← 本次新增
# ok  mininet/pkg/http         (10 tests)   ← 本次新增
```

## 项目结构（第 5 次会话结束）

```text
MiniNet/
├── go.mod · Makefile · README.md · .gitignore
├── cmd/
│   ├── mnet/                           REPL
│   ├── mnet-ping/                      独立 ping
│   ├── mnet-dig/                       独立 dig
│   └── mnet-curl/                     ◀── 本次新增 独立 curl
├── pkg/
│   ├── common/ · link/ · net/ · transport/ · mux/ · dns/  (继承)
│   ├── tls/                           ◀── 本次新增 TLS 可视化
│   │   ├── hello.go                    ClientHello/ServerHello 字节解析
│   │   ├── wrap.go                     crypto/tls 包装 + CaptureClientHello
│   │   └── hello_test.go
│   └── http/                          ◀── 本次新增 HTTP/1.1 栈
│       ├── message.go                  Request/Response 编解码 + chunked
│       ├── server.go                   Server + Mux (精确/前缀/fallback)
│       ├── client.go                   Client (http:// + https://)
│       ├── message_test.go
│       └── server_test.go              端到端 Server+Client
└── tests/
    ├── wire_demo/ · bug2_demo/ · bug3_demo/ · bug4_demo/ · dns_demo/ (继承)
    └── http_demo/                     ◀── 本次新增 HTTP 本地演示
```

**代码规模**：~9000 行 Go，68 个测试。

## 后续路线

| 会话 | 阶段 | 产出 |
|------|------|------|
| 6 | ⑫⑬⑭ 代理+缓存+WS | 迷你 Nginx + 聊天室 |
| 7 | ⑮⑯ QUIC + 总装 | 全链路 shop demo |
