---
title: 端口DNS
date: 2025-06-07 10:00:00
permalink: /pages/tool-port/
categories:
  - ScriptHub
  - 工具脚本
  - 网络工具
tags:
  - 工具
  - 网络
author:
  name: 杨充
  link: https://github.com/yangchong211
---
# 端口DNS

> 端口检测(nc/telnet)、DNS 诊断(dig/nslookup)、端口占用排查(ss/lsof)、Python 端口扫描。

## 端口基础：TCP 三次握手

**为什么"端口通了"不等于"服务正常"？** ——端口的"通"只说明 TCP 三次握手成功（SYN → SYN-ACK → ACK），但服务进程可能处于假死状态：listen 队列满但不再 accept 新连接。因此在生产环境中，端口检测只是第一步，HTTP 层面的健康检查才是最终验证。

> **TCP 三次握手（深度）**：
> ① 客户端发送 SYN（序列号 x）→ ② 服务端回复 SYN-ACK（序列号 y，确认号 x+1）→ ③ 客户端回复 ACK（确认号 y+1）
> 这个过程中不包含任何应用层数据——只建立连接，不验证服务。

## 一、端口检测

**`/dev/tcp` 伪文件**是 bash 内建特性（非所有 shell 支持）。`echo > /dev/tcp/host/port` 本质是让 bash 发起 TCP connect，成功则无输出。比 `nc` 更快（不 fork 新进程），但功能较少。

### 1.1 nc / ncat——端口连通性测试

```bash
#!/bin/bash

# ===== TCP 端口检测 =====
# -z 只检测连通性（不发送数据），-v 显示结果，-w 超时秒数
nc -zv 192.168.1.10 80
nc -zv -w 3 192.168.1.10 22

# 端口范围扫描
nc -zv 192.168.1.10 80-100

# ===== UDP 端口 =====
# UDP 无连接——nc 发空包并观察是否有 ICMP Port Unreachable 返回
nc -zuv 192.168.1.10 53

# ===== 发送数据与应用层交互 =====
# nc 不仅检测端口——更能直接与应用协议对话，验证服务层是否正常
echo "GET / HTTP/1.0\r\n\r\n" | nc 192.168.1.10 80   # 测试 HTTP
echo "PING" | nc -w 1 192.168.1.10 6379               # 测试 Redis PING

# ===== bash 内建替代（无 nc 时使用）=====
# bash 的 /dev/tcp 伪设备直接发起 TCP 连接——一行搞定
timeout 3 bash -c "echo > /dev/tcp/192.168.1.10/80" && echo "通" || echo "不通"
```

### 1.2 lsof / ss——查看端口被谁占用

```bash
#!/bin/bash

# ===== lsof (List Open Files)=====
# "一切皆文件"哲学——socket 连接在 Linux 中也是文件描述符
lsof -i :80                                    # 查看 80 端口被哪个进程占用
lsof -i :80-100                                # 端口范围
lsof -i tcp:80                                 # 只看 TCP
lsof -i @192.168.1.10                          # 查看连接到某 IP 的所有连接
lsof -i -P -n                                  # 全部网络连接（不解析）

# ===== ss (Socket Statistics)=====
# 比 netstat 快百倍——直接从 /proc/net 读内核数据，不遍历 /proc/pid
ss -tlnp | grep :80
ss -tlnp 'sport = :80'

# ===== 找出谁在监听 =====
p80=$(ss -tlnp 'sport = :80' | awk 'NR>1{print $NF}' | grep -oP '(?<=pid=)\d+')
[[ -n "$p80" ]] && ps -o pid,user,comm -p "$p80"
```

> **ss vs netstat 性能**：netstat 遍历所有 `/proc/<pid>/fd` 统计连接，10000 个连接约 1 秒；ss 直接读 `netlink` 套接字，时间复杂度 O(1)，几乎瞬间完成。

### 1.3 Python——端口扫描器

Python 的 `socket.connect_ex()` 替代 `connect()`——返回错误码而非抛异常，更适合扫描场景。**并发**用 `ThreadPoolExecutor`：100 个线程同时发起 TCP connect，每个线程等待 2 秒超时——1024 个端口 ≈ 20 秒扫完。

```python
#!/usr/bin/env python3
"""快速 TCP 端口扫描——并发检测
原理：对每个端口发起 TCP connect()，
    成功=端口开放(RST/超时=关闭)，用线程池实现并发
"""
import socket, sys, concurrent.futures

def check_port(ip, port, timeout=2):
    """connect_ex 返回 0 表示连接成功（端口开放）"""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.settimeout(timeout)
        return port if s.connect_ex((ip, port)) == 0 else None

def scan_ports(ip, ports):
    open_ports = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=100) as ex:
        futures = {ex.submit(check_port, ip, p): p for p in ports}
        for fut in concurrent.futures.as_completed(futures):
            result = fut.result()
            if result: open_ports.append(result)
    return sorted(open_ports)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(f"用法: {sys.argv[0]} <IP> [起始端口] [结束端口]")
        sys.exit(1)
    ip = sys.argv[1]
    start = int(sys.argv[2]) if len(sys.argv) > 2 else 1
    end   = int(sys.argv[3]) if len(sys.argv) > 3 else 1024
    print(f"扫描 {ip} 端口 {start}-{end} ...")
    open_ports = scan_ports(ip, range(start, end + 1))
    if open_ports:
        print(f"开放端口: {', '.join(map(str, open_ports))}")
    else:
        print("未发现开放端口")
```

## 二、DNS 诊断

### DNS 解析链路

**访问 `www.example.com` 时发生了什么？**
1. **浏览器/OS 缓存**：本地 hosts 文件 → DNS 缓存
2. **递归查询**：请求本地 DNS 服务器（如路由器/运营商）
3. **迭代查询**：本地 DNS 没有缓存 → 根 DNS → `.com` TLD DNS → `example.com` 权威 DNS
4. **返回结果**：A 记录返回 IP，TTL 决定缓存时间

`dig +trace` 可以可视化整个迭代查询过程——从根服务器一路追踪到权威 DNS。

### 2.1 dig——DNS 瑞士军刀

```bash
#!/bin/bash

# ===== 基础查询 =====
dig example.com                                # 完整信息（HEADER + QUESTION + ANSWER）
dig example.com +short                         # 只输出 IP（脚本友好）
dig example.com A                              # 查询 A 记录（IPv4）
dig example.com AAAA                           # IPv6
dig example.com MX                             # 邮件服务器
dig example.com NS                             # 域名服务器
dig example.com TXT                            # TXT 记录（SPF/DKIM 等）

# ===== 指定 DNS 服务器（比对各 DNS 结果）=====
dig @8.8.8.8 example.com                      # Google DNS
dig @1.1.1.1 example.com                      # Cloudflare

# ===== 追踪解析过程——看 DNS 请求如何逐级传递 =====
dig example.com +trace
# 输出示例：
# .                   518400 IN NS  a.root-servers.net.    ← 根服务器
# com.                172800 IN NS  a.gtld-servers.net.   ← .com TLD
# example.com.        86400  IN A   93.184.216.34         ← 权威 DNS 返回

# ===== 反向解析 IP → 域名 =====
dig -x 8.8.8.8

# ===== 查看解析耗时（毫秒）=====
dig example.com | grep "Query time"
```

> **dig 输出关键字段**：`ANSWER SECTION` 是最终答案；`AUTHORITY SECTION` 是权威 DNS；`ADDITIONAL SECTION` 是附加信息（如 NS 对应的 IP）。`status: NOERROR` 表示查询成功。

### 2.2 Python DNS 查询

```python
#!/usr/bin/env python3
"""Python DNS 查询——标准库 socket 提供基础解析"""
import socket

def dns_lookup(domain, record_type='A'):
    """gethostbyname 只返回一个 IP——内部调用 gethostbyname_r"""
    try:
        if record_type == 'A':
            return socket.gethostbyname(domain)
        elif record_type == 'ALL':
            return socket.getaddrinfo(domain, None)
    except socket.gaierror as e:
        return f"解析失败: {e}"

print(dns_lookup('baidu.com'))

# 复杂查询用 dnspython（支持 MX/NS/SOA 等全部记录类型）
# import dns.resolver
# answers = dns.resolver.resolve('example.com', 'MX')
```

## 三、实战：网络环境检测脚本

```bash
#!/bin/bash
# 综合端口 + DNS 检测——快速定位网络问题在哪个环节

echo "===== 网络环境检测 ====="

# 1. DNS 测试（解析服务器——域名→IP 是上网第一步）
echo "--- DNS 解析 ---"
for domain in baidu.com google.com github.com; do
    if host "$domain" > /dev/null 2>&1; then
        echo "✅ $domain ($(dig +short "$domain" | head -1))"
    else
        echo "❌ $domain 解析失败"
    fi
done

# 2. 端口测试（TCP 层——DNS 之后能否建立连接）
echo ""
echo "--- 端口连通性 ---"
declare -A TARGETS=(
    ["www.baidu.com:443"]="外网HTTPS"
    ["8.8.8.8:53"]="DNS"
    ["localhost:22"]="SSH"
)
for target in "${!TARGETS[@]}"; do
    host="${target%:*}"
    port="${target#*:}"
    if timeout 2 bash -c "echo > /dev/tcp/$host/$port" 2>/dev/null; then
        echo "✅ ${TARGETS[$target]} ($host:$port)"
    else
        echo "❌ ${TARGETS[$target]} ($host:$port)"
    fi
done
```
