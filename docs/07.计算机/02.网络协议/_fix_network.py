import re

FILE = "/Users/yc/YCBookBlog/docs/07.计算机/02.网络协议/01.通过看新闻熟悉网络.md"

with open(FILE, 'r', encoding='utf-8') as f:
    content = f.read()

# ====== 1. Find the TOC and insert new entries ======
toc_new = """#### 目录介绍
- 01.工作案例引入
  - 1.1 新闻App突然连不上了
  - 1.2 为什么要学网络协议
- 02.HTTP请求准备
  - 2.1 确定网络协议
  - 2.2 构建请求URL
  - 2.3 DNS解析流程
  - 2.4 HTTP请求准备
  - 2.5 建立网络连接
- 03.HTTP请求构建
  - 3.1 请求报文规范
  - 3.2 创建请求行
  - 3.3 设置请求头
  - 3.4 添加请求体
- 04.HTTP请求发送
  - 4.1 建立网络链接
  - 4.2 发送请求头
  - 4.3 发送请求体
  - 4.4 等待服务器响应
- 05.HTTP返回构建
  - 5.1 构建响应码
  - 5.2 构建响应头
  - 5.3 构建响应体
  - 5.4 发送响应数据
- 06.页面加载和渲染
  - 6.1 解析HTML
  - 6.2 加载外部资源
  - 6.3 渲染页面
  - 6.4 布局和绘制
  - 6.5 页面加载完成
- 07.网络协议深度分析
  - 7.1 TCP连接的细节
  - 7.2 HTTP协议的演进
  - 7.3 浏览器的并发策略
  - 7.4 网络请求的性能指标
- 08.常见网络问题排查
  - 8.1 DNS解析失败
  - 8.2 连接超时分析
  - 8.3 请求缓慢排查
  - 8.4 抓包分析方法
- 09.思考题与作业
  - 9.1 基础思考题目
  - 9.2 进阶思考题目
  - 9.3 动手实践作业"""

# Replace old TOC
content = re.sub(r'#### 目录介绍\n(?:- .*\n?)+', toc_new + '\n', content)

# ====== 2. Insert the new ## 01 section after TOC ======
new_section = """
## 01.工作案例引入

### 1.1 新闻App突然连不上了

**场景**：小张是一名工作两年的客户端工程师，负责公司新闻App的网络模块。某天用户大量投诉：**"首页一直在转圈圈，刷不出新闻"**。

小张从代码层面排查了一遍——接口URL是对的，数据格式没有变，本地缓存正常清理了，应用也重启过了。但问题依旧。

```bash
# 小张在服务器上排查
$ curl -I https://api.news.com/headlines
curl: (6) Could not resolve host: api.news.com

# ping 域名
$ ping api.news.com
ping: cannot resolve api.news.com: Unknown host

# 直接 ping IP 是通的
$ ping 8.8.8.8
64 bytes from 8.8.8.8: icmp_seq=1 ttl=117 time=12.3 ms
```

**疑惑**：网络明明是通的（能 ping 通 8.8.8.8），为什么 `api.news.com` 这个域名解析不了？昨天还好好的，今天什么都没改，怎么就崩了？

**追问链**：

- "网络能 ping 通不等于服务是好的" → 对，ping 用的是 ICMP 协议，和 HTTP 是两回事
- "那 DNS 解析失败是怎么发生的？" → DNS 查询走 UDP 53 端口，可能被运营商拦截、缓存污染、或 DNS 服务器挂了
- "为什么昨天能用，今天不能用？" → 可能运营商 DNS 服务器的缓存过期了，上游递归解析链路出了问题
- "那直接换 DNS 服务器呢？" → 把本机 DNS 改成 8.8.8.8（Google 公共 DNS），解析立刻恢复了
- "为什么改个 DNS 就好了？DNS 到底是怎么把域名解析成 IP 的？" → 这就是本章要回答的第一个问题
- "那从输入 api.news.com 到页面最终渲染出新闻，中间经历了哪些网络协议？" → DNS→TCP→TLS→HTTP→CDN→渲染，每个环节都有协议在背后工作

小张最后只用了一行命令修复——把系统的 DNS 从"自动获取"改成 `8.8.8.8`——就解决了这个让全栈工程师熬夜两小时的问题。但如果不是 DNS 问题，而是其他环节（TCP 连接、HTTPS 证书、CDN 回源）呢？

这一串追问，答案全部写在网络协议的知识体系里。

### 1.2 为什么要学网络协议

```mermaid
flowchart LR
    A["你在浏览器输入<br/>news.example.com"] --> B[DNS 解析]
    B --> C[TCP 三次握手]
    C --> D[（可选）TLS 握手]
    D --> E[HTTP 请求/响应]
    E --> F[浏览器解析渲染]
    style B fill:#ffe4b5
    style C fill:#90EE90
    style D fill:#87CEEB
    style E fill:#FFD700
```

你每天打开新闻网站、刷短视频、网购，背后是多个网络协议在一秒内接力完成的。但大部分开发者只看到 `fetch()` 或 `axios` 这一层，从不去想：

- 为什么有时候刷不出页面，但微信还能发？
- 为什么同一个 App 在公司 WiFi 飞快，切 4G 就卡？
- 为什么加了 CDN 后页面快了很多？CDN 是怎么"加速"的？
- 为什么 HTTPS 比 HTTP 安全？多花的那些时间值不值？
- 为什么打开一个页面要发几十个 HTTP 请求？

本章的目标，就是用一个"看新闻"的全流程串联 TCP/IP 协议栈的每一层：

- **应用层**：HTTP 请求和响应是怎么构造的？状态码、请求头、Cookie 背后是什么？
- **传输层**：TCP 怎么保证可靠性？三次握手为什么是三次？Keep-Alive 怎么复用？
- **网络层**：IP 地址怎么找到目标服务器？DNS 怎么把域名变成 IP？
- **协议演进**：为什么会有 HTTP/2、HTTP/3？每一代解决了什么问题？
- **问题排查**：当网站打不开时，怎么用所学知识一步步定位根因？

带着这五个问题，我们从一个 DNS 事故开始，跟着一次"看新闻"的请求流程层层深入。
"""

# Insert after the TOC (replace pattern "## 01.HTTP请求准备" will be removed and re-inserted later)
content = re.sub(r'\n\n## 01\.HTTP请求准备', new_section + '\n\n## 02.HTTP请求准备', content)

# ====== 3. Renumber existing sections ======
# Renumber ## sections: 01→02, 02→03, 03→04, 04→05, 05→06, 06→07, 07→08
# Renumber ### subsections: x.y → (x+1).y
renumber_map = {
    '01': '02', '02': '03', '03': '04', '04': '05', '05': '06', '06': '07', '07': '08'
}

# Renumber ## headings
for old_num, new_num in renumber_map.items():
    content = re.sub(rf'^## {old_num}\.', f'## {new_num}.', content, flags=re.MULTILINE)

# Renumber ### headings
for old_num, new_num in renumber_map.items():
    content = re.sub(rf'^### {old_num}\.', f'### {new_num}.', content, flags=re.MULTILINE)

# ====== 4. Fix TOC anchor references in the text ======
# The TOC anchors in the body text use format like (#01xx)
# We need to update them from (#01xxx to (#02xxx etc.
for old_num, new_num in renumber_map.items():
    content = re.sub(rf'\(#{old_num}([^)]*)\)', f'(#{new_num}\\1)', content)

# ====== 5. Add 09.思考题与作业 section at the end ======
# Find the last non-empty line and add before it
thinking_section = """

## 09.思考题与作业

### 9.1 基础思考题目

1. **DNS 解析过程**：画出一张图，从浏览器输入 `www.163.com` 到获得 IP 地址，列出每一步涉及的 DNS 服务器类型。如果本地 `/etc/hosts` 文件中配置了 `127.0.0.1 www.163.com`，DNS 查询会怎么走？

2. **TCP 三次握手**：为什么是三次不是两次？为什么不是四次？假设一个客户端发送 SYN 后，收到了 SYN+ACK，但最后一个 ACK 在网络中丢失了——这个连接在服务端和客户端分别是什么状态？

3. **HTTP 请求流程**：列出一次 `GET https://www.163.com/index.html` 从输入到页面加载完成，涉及的所有协议（从应用层到链路层），并标注每层的主要职责。

4. **HTTP 状态码**：以下场景分别返回什么状态码：
   - 请求的资源不存在
   - 请求的资源被永久移到了新 URL
   - 服务器内部抛出异常
   - 请求的资源未修改（缓存可用）
   - 请求的 URL 需要登录才能访问

5. **Keep-Alive 的作用**：HTTP/1.1 默认开启 Keep-Alive。如果一个页面上有 100 个资源（图片、CSS、JS），在 Keep-Alive 开启和关闭两种情况下，分别需要多少个 TCP 连接？假设浏览器对同一域名的并发连接限制为 6 个。

### 9.2 进阶思考题目

1. **1.1 节复盘**：小张的新闻 App DNS 故障。如果换成 HTTPS，DNS 劫持是否还会影响？HTTPS 能防 DNS 劫持吗？（提示：区分"域名解析"和"通信加密"两个层面）

2. **CDN 加速原理**：新闻网站的图片通常通过 CDN 分发。CDN 是怎么知道"北京的用户从最近的节点拿图片"的？全局负载均衡（GSLB）是怎么通过 DNS 实现的？和 1.1 节的 DNS 故障有什么关系？

3. **HTTP/2 多路复用的真相**：都说 HTTP/2 解决了 HTTP/1.1 的队头阻塞，但 HTTP/2 在 TCP 层面仍有队头阻塞——为什么？HTTP/3 的 QUIC 又是怎么彻底解决这个问题的？

4. **tcpdump 抓包分析**：如果你在服务器上执行 `curl https://www.163.com`，用 `tcpdump -i eth0 port 443` 抓包，你会看到哪些包的交互序列？请从 TCP 三次握手、TLS 握手、HTTP 请求响应三个阶段分别写出期望看到的包类型。

5. **移动端网络优化**：新闻 App 在弱网环境（如地铁、电梯）下，用户经常看到"加载中"转圈。从网络协议的角度，你能给出哪些优化方案？（提示：考虑 DNS 预解析、连接复用、预加载、HTTP/2 Server Push）

### 9.3 动手实践作业

**作业一（必做）**：用开发者工具分析真实页面。

```bash
# 打开 Chrome DevTools → Network 面板
# 访问 https://www.163.com
# 截图并回答以下问题：
1. 总共发了多少个 HTTP 请求？
2. 使用了多少个不同的域名？
3. 首字节时间（TTFB）是多少？
4. 有没有使用 HTTP/2？（查看 Protocol 列）
5. 有没有资源通过 CDN 加载？（查看 IP 归属）
```

**作业二（选做）**：DNS 排查实战。

```bash
# 1. 用 dig 命令查看 www.163.com 的 DNS 解析过程
dig www.163.com +trace

# 2. 用 nslookup 分别使用 8.8.8.8 和 223.5.5.5（阿里 DNS）查询同一个域名
nslookup www.163.com 8.8.8.8
nslookup www.163.com 223.5.5.5

# 3. 对比两次查询结果的时间差异
# 如果两个 DNS 服务器返回了不同的 IP，说明了什么？
```

**作业三（选做）**：模拟 1.1 节的 DNS 故障。

```bash
# 1. 修改 /etc/hosts 文件（需要 root），把 www.163.com 指向一个错误的 IP
echo "127.0.0.2 www.163.com" >> /etc/hosts

# 2. 用 curl 访问，观察返回结果
curl -v https://www.163.com

# 3. 然后用正确的 IP 替换回来，再次对比
# 4. 分析：为什么 hosts 文件的优先级比 DNS 高？这个机制的设计初衷是什么？
```

**作业四（架构思考）**：对你当前负责的一个服务，画出它的"网络协议全景图"。

- 从"用户请求"到"服务返回"，中间经过了哪些网络设备和协议层？
- 标注每一个环节的协议、延迟量级（如 DNS 20ms、TCP 30ms、TLS 50ms）
- 当前瓶颈在哪？如果流量增加 10 倍，哪个环节会先扛不住？为什么？
"""

content = content.rstrip() + thinking_section

# ====== 6. Write result ======
with open(FILE, 'w', encoding='utf-8') as f:
    f.write(content)

print("Done! File updated successfully.")

# Verify: count total ## and ### headings
headings = re.findall(r'^## \d+\.|^### \d+\.', content, re.MULTILINE)
print(f"Total headings found: {len(headings)}")
