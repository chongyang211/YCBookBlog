# MiniNet · 迷你网络栈

> 《网络协议原理》综合案例 · Go 从 0 到 1 徒手造一台"会讲协议"的机器
>
> 配套文档：`packages/website/04.计算机/02.网络协议原理/21.MiniNet迷你网络栈综合案例.md`

## 一句话定位

从一个 `socketpair` 字节流开始，**15 个阶段 · 58 个 Step**，徒手造一台既是客户端又是服务端的迷你网络栈。
把《网络协议原理》18 篇专栏的每一个知识点都在自己代码里落一遍：
ARP → IP → TCP 状态机 → TLS → HTTP → 反代/缓存 → WebSocket → QUIC。

## 快速开始

需要 Go 1.22+。

```bash
make build          # 编译,产物在 bin/mnet
./bin/mnet          # 启动 REPL

>>> dump            # 看到 5 层 [Empty] 骨架
>>> route add 10.0.0.0/8 via 10.0.0.1
>>> route lookup 10.1.2.3
>>> help
>>> quit
```

## 当前进度

**第 1 次会话** 已完成（本仓库当前状态）：

- ✅ 阶段 ① Stack 骨架 + REPL
- ✅ 阶段 ② 寻址 & 子网 + 最长前缀路由
- ✅ 阶段 ③ 报文编解码 + 🔥造 BUG-1 大小端修复

```bash
make test           # 单元测试全绿
make wire-demo      # 大小端 BUG vs 修复版 hex 对比
```

## 项目结构

```text
MiniNet/
├── go.mod
├── Makefile
├── cmd/mnet/main.go                # REPL 主程序
├── pkg/
│   ├── common/                     # 公共工具: 日志/hex/大端/校验和
│   │   ├── log.go
│   │   ├── hex.go
│   │   ├── bigendian.go
│   │   └── checksum.go
│   └── net/                        # 网络层: 地址/子网/路由
│       ├── subnet.go
│       └── route.go
└── tests/wire_demo/main.go         # 🔥 BUG-1 现场 + 修复对比
```

## 后续路线

| 会话 | 阶段 | 产出 |
|------|------|------|
| 2 | ④⑤ ARP + ICMP ping | 真·两进程互 ping |
| 3 | ⑥⑦ TCP 握手 + 滑窗 | 自己写的 TCP 栈 |
| 4 | ⑧⑨ epoll + DNS | 并发 server + 递归解析 |
| 5 | ⑩⑪ TLS + HTTP | 自己的 mnet-curl |
| 6 | ⑫⑬⑭ 代理+缓存+WS | 迷你 Nginx + 聊天室 |
| 7 | ⑮⑯ QUIC + 总装 | 全链路 shop demo |
