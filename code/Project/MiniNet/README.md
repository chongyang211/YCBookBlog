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
- ✅ ⑩ TLS 1.3 握手可视化
- ✅ ⑪ HTTP/1.1 服务端 + 客户端

**第 6 次会话**（已完成）：
- ✅ ⑫ Keep-Alive 池 + Client ConnPool
- ✅ ⑬ ReverseProxy + LRU + singleflight + 🔥 BUG-5 缓存击穿（334× 保护）
- ✅ ⑭ WebSocket RFC6455 + 聊天室 + 🔥 BUG-6 无心跳被 LB 杀

**第 7 次会话**（已完成 · **完结** 🎉）：
- ✅ ⑮ MiniQUIC: ConnID + 多流 + 迁移 + 🔥 BUG-7 TCP 队头阻塞（10× 加速）
- ✅ ⑯ shop 全栈总装: 浏览器可点的 http://127.0.0.1:8080/

```bash
make test               # 所有包测试全绿
# ok  mininet/pkg/cache        (4  tests)
# ok  mininet/pkg/common       (9  tests)
# ok  mininet/pkg/dns          (7  tests)
# ok  mininet/pkg/http         (13 tests)
# ok  mininet/pkg/link         (11 tests)
# ok  mininet/pkg/mux          (2  tests + 3 benchmark)
# ok  mininet/pkg/net          (12 tests)
# ok  mininet/pkg/proxy        (4  tests)
# ok  mininet/pkg/quic         (4  tests)   ← 本次新增
# ok  mininet/pkg/tls          (4  tests)
# ok  mininet/pkg/transport    (13 tests)
# ok  mininet/pkg/ws           (5  tests)
# ─────────────────────────────────────────
# 共 88 测试, 12 个包全绿
```

## 项目结构（**第 7 次会话结束 · 本案例完结** 🎉）

```text
MiniNet/
├── go.mod · Makefile · README.md · .gitignore
├── cmd/                              6 个独立二进制
│   ├── mnet/           REPL
│   ├── mnet-ping/      ping         · mnet-dig/   dig
│   ├── mnet-curl/      curl         · mnet-proxy/ 反代
│   └── mnet-chat/      WebSocket 聊天室
├── pkg/                              12 个包
│   ├── common/ · link/ · net/ · transport/ · mux/ · dns/ · tls/ (继承)
│   ├── http/ · cache/ · proxy/ · ws/  (继承)
│   └── quic/          ◀── 本次新增 MiniQUIC 教学版
│       ├── packet.go           包/帧编解码
│       ├── conn.go             Conn + 多流 + 迁移
│       └── quic_test.go        (含 ConnID 迁移测试)
└── tests/                            7 个 BUG + 3 个 demo
    ├── wire · bug2~bug6 · dns · http_demo (继承)
    ├── bug7_demo/     ◀── 本次新增 🔥 TCP 队头阻塞 vs QUIC
    └── shop_demo/     ◀── 本次新增 🎉 全栈 shop.html
        ├── main.go              3 服务一进程 (origin + edge + ws)
        └── shop.html            浏览器可点
```

**最终规模**：~12500 行 Go，88 个测试全绿，7 个 BUG 现场，1 个 shop 浏览器 demo。

## 🎉 完结里程碑

```bash
make build && make test              # 6 二进制 + 88 测试
make bug7-demo                       # 看 TCP vs QUIC 10× 对比
make demo-shop                       # 打开浏览器 http://127.0.0.1:8080/
git tag v1.0.0                       # 完结纪念
```
