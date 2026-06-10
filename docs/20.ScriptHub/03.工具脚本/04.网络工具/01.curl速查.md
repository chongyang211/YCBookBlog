---
title: cURL速查
date: 2025-06-07 10:00:00
permalink: /pages/tool-curl/
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
# cURL速查

> HTTP 调试瑞士军刀——GET/POST/Header/Cookie/文件上传/代理/超时/下载/认证，一行搞定。

## 一、基础请求

curl 发起一次 HTTP 请求的背后经历了以下步骤：**DNS 解析 → TCP 三次握手 → TLS 握手（HTTPS）→ 发送 HTTP 请求 → 接收响应 → 关闭连接**。理解这个链路，方能诊断出"哪个环节慢"。

```bash
#!/bin/bash

# ===== GET —— 最常用 =====
curl https://api.example.com/users                # 基础 GET
curl -G https://api.example.com/users -d "page=1" # GET + 查询参数
curl -I https://api.example.com                   # 只看响应头（HEAD）
curl -v https://api.example.com                   # -v 打印请求/响应头+握手信息
curl -s https://api.example.com                   # 静默模式（不显示进度条）

# ===== POST —— 发送数据 =====
# form 数据（Content-Type: application/x-www-form-urlencoded）
curl -X POST https://api.example.com/login \
    -d "username=alice&password=secret"

# JSON 数据（推荐——现代 API 标配）
# -H 设置 Content-Type 告诉服务器解析方式，-d 指定 JSON 字符串
curl -X POST https://api.example.com/users \
    -H "Content-Type: application/json" \
    -d '{"name":"Alice","email":"alice@example.com"}'

# 从文件读取请求体——适合复杂/大 JSON
curl -X POST https://api.example.com/data -d @payload.json

# ===== PUT / PATCH / DELETE =====
# RESTful API 完整覆盖——方法名即语义
curl -X PUT https://api.example.com/users/1 -d '{"name":"Bob"}'
curl -X PATCH https://api.example.com/users/1 -d '{"email":"new@ex.com"}'
curl -X DELETE https://api.example.com/users/1
```

> **原理**：curl 默认 `-X GET`，当用了 `-d` 会自动切换为 POST——但显式写 `-X` 更可读。HTTP 响应头以 `\r\n\r\n` 结尾标识头部结束，其后为 body。

## 二、输出控制

curl 默认输出响应体到 stdout。生产环境中需要**分离输出**：响应体落文件、响应头单独存、性能指标提取分析。

```bash
#!/bin/bash

# ===== 输出到文件 =====
curl -o file.html https://example.com           # -o 指定文件名（覆盖）
curl -O https://example.com/file.tar.gz         # -O 按远程文件名保存
curl -O https://cdn.com/img[1-10].jpg           # 批量下载 img1.jpg ~ img10.jpg

# ===== 只输出响应码（健康检查核心）=====
curl -s -o /dev/null -w "%{http_code}" https://api.example.com
# -s 静默  -o /dev/null 丢弃body  -w 提取指定变量

# ===== 性能分析（-w 格式化变量）=====
# 一个请求的总时间可以拆解为各自独立的阶段：
curl -w "\n\
HTTP状态:      %{http_code}\n\
总耗时:        %{time_total}s\n\
DNS解析:       %{time_namelookup}s\n\
TCP连接:       %{time_connect}s\n\
SSL握手:       %{time_appconnect}s\n\
首字节时间:    %{time_starttransfer}s\n\
下载大小:      %{size_download} bytes\n\
下载速率:      %{speed_download} B/s\n" \
    -o /dev/null -s https://api.example.com
```

> **性能诊断公式**：`time_total = time_namelookup + (time_connect - time_namelookup) + (time_appconnect - time_connect) + (time_starttransfer - time_appconnect) + 下载时间`
> - DNS 慢 → 检查本地 DNS 缓存或解析服务器
> - TCP 连接慢 → 网络延迟高，考虑就近部署
> - SSL 握手慢 → 证书链过长或 OCSP 查询超时
> - TTFB 高 → 服务端处理慢

```bash
# ===== 显示响应头 + 响应体 =====
curl -i https://api.example.com                 # 一起输出（头+体）
curl -D headers.txt -o body.txt https://api.example.com  # 分开保存到文件
```

## 三、Header 与 Cookie

HTTP 是**无状态协议**。Header 和 Cookie 是"记住状态"的两种主要方式：Header 通常承载认证 token，Cookie 则自动携带会话信息。

```bash
#!/bin/bash

# ===== 自定义 Header =====
# 每条 -H 添加一个 Header，多个 -H 叠加
curl -H "Authorization: Bearer eyJhbGci..." https://api.example.com/me
curl -H "X-API-Key: abc123" -H "Accept: application/json" https://api.example.com

# ===== Cookie =====
# 发送 Cookie——登录态维持的基础
curl -b "session=abc123; user=alice" https://api.example.com
curl -b cookies.txt https://api.example.com     # 从文件加载（Netscape 格式）

# 登录并保存 Cookie（两步：先登录，再访问受保护页面）
curl -c cookies.txt -d "user=alice&pass=xxx" https://api.example.com/login
curl -b cookies.txt https://api.example.com/dashboard    # 带 Cookie 访问
# -c 写入  -b 读取——二者可以指向同一个文件实现自动延续
```

> **Cookie 文件格式**（Netscape HTTP Cookie File）：
> `example.com	FALSE	/	FALSE	0	session	abc123`
> 分别是：域名、子域名匹配标志、路径、安全标志、过期时间、名称、值。

## 四、认证方式

认证的本质是**向服务器证明身份**。不同认证方式对应不同的安全级别和应用场景：

| 方式 | 原理 | 安全性 | 场景 |
|------|------|--------|------|
| Basic Auth | 用户名:密码 → Base64 编码放 Header | 低（明文等效） | 内网工具、测试 |
| Bearer Token | 请求头携带令牌 | 中（有过期机制） | REST API 标配 |
| Digest Auth | 服务端发 nonce 挑战，客户端用哈希回应 | 较高（密码不传明文） | 老旧系统 |
| API Key | 静态密钥放在 Header | 中（依赖 HTTPS） | 第三方服务对接 |

```bash
#!/bin/bash

# ===== Basic Auth =====
curl -u alice:password https://api.example.com
# 等价于：curl -H "Authorization: Basic $(echo -n 'alice:password' | base64)"
# ⚠️ Base64 不是加密——等同于明文！务必在 HTTPS 下使用

# ===== Bearer Token（JWT/OAuth 2.0 标配）=====
curl -H "Authorization: Bearer TOKEN" https://api.example.com

# ===== API Key =====
curl -H "X-API-Key: sk-abc123def456" https://api.example.com

# ===== Digest Auth =====
curl --digest -u user:pass https://api.example.com
```

## 五、文件上传

HTTP 文件上传有两种主流方式：**multipart/form-data**（HTML 表单模拟，`-F`）和**原始二进制 PUT**（`--upload-file`）。前者兼容性最好，后者适合 API 直传。

```bash
#!/bin/bash

# ===== 上传文件（multipart/form-data）=====
# -F 自动设 Content-Type: multipart/form-data + boundary 分隔符
curl -F "file=@photo.jpg" https://api.example.com/upload
curl -F "file=@photo.jpg;type=image/jpeg" https://api.example.com/upload

# 多文件
curl -F "image1=@a.jpg" -F "image2=@b.jpg" https://api.example.com/upload

# 文件 + 表单字段（混合上传）
curl -F "file=@doc.pdf" -F "title=Report" -F "tags=important" https://api.example.com

# ===== PUT 上传原始二进制（对象存储常用）=====
curl --upload-file video.mp4 https://storage.example.com/object
# 等价于：curl -T video.mp4 https://storage.example.com/object
# 直接发送文件字节流，不编码
```

> **multipart 原理**：每个 `-F` 字段是一个"part"，用随机 boundary 字符串分隔。文件 part 还包含 `Content-Type` 子头部描述文件类型——浏览器上传文件就是用的这个协议。

## 六、超时 / 重试 / 代理 / SSL

```bash
#!/bin/bash

# ===== 超时——区分连接超时和总超时 =====
# TCP 三次握手最多等 5 秒，一旦连上不再受此限制
# 整次请求（含下载）最多 30 秒——慢服务器可能会被截断
curl --connect-timeout 5 --max-time 30 https://api.example.com

# ===== 重试——配合指数退避 =====
# curl 只对"连接问题"重试（网络超时/拒绝连接），
# HTTP 4xx/5xx 状态码不会触发重试（除非用 --retry-all-errors）
curl --retry 3 --retry-delay 5 --retry-max-time 60 https://unstable.api.com

# ===== 代理——不同层级对应不同协议 =====
# HTTP 代理：代理理解 HTTP 请求，可能修改 Headers
curl -x http://proxy:8080 https://api.example.com
# SOCKS5：只转发 TCP 流量，不关心应用层——更通用
curl -x socks5://proxy:1080 https://api.example.com
# 内网地址绕过代理
curl --noproxy "*.local,localhost,10.*" https://api.example.com

# ===== SSL / TLS =====
# -k 跳过证书验证——等于不验证服务器身份，仅限本地开发！
curl -k https://self-signed.badssl.com/
# 指定自签名 CA 证书（企业内部用）
curl --cacert ca.pem https://internal.company.com
# mTLS（双向认证）：客户端也提供证书证明身份
curl --cert client.pem --key client-key.pem https://api.example.com
```

> **TLS 握手耗时约占一次 HTTPS 请求的 30%~50%**。HTTP/2 和 HTTP/3 支持连接复用，避免每次请求重新握手。curl 默认 HTTP/1.1，加 `--http2` 可启用 HTTP/2。

## 七、实战脚本

```bash
#!/bin/bash
# ===== 一行 API 健康检查 =====
check_api() {
    code=$(curl -s -o /dev/null -w "%{http_code}" --connect-timeout 3 "$1")
    [[ "$code" == "200" ]] && echo "✅ $1" || echo "❌ $1 (HTTP $code)"
}

# ===== 下载文件带进度条和重试 =====
download_file() {
    # -f: 404→失败  -S: 显示错误  -L: 重定向跟踪  -#: 进度条样式
    curl -fSL --retry 3 -# -o "${2:-$(basename $1)}" "$1"
}
```
