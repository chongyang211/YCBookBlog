import re

path = "/Users/yc/YCBookBlog/docs/07.计算机/02.网络协议/06.传输协议TCP和UDP.md"

with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# ====== 1. Insert case intro before ## 01.传输协议场景 ======
new_section = r"""
## __NEW_SEC__工作案例引入

### __NEW_1_1__直播平台的"马赛克"事故

**场景**：小杨是一名刚接手公司直播业务的运维工程师。最近用户投诉越来越多——**"直播画面卡成PPT"**、**"声音和画面对不上"**、**"主播说话5秒后观众才听到"**。技术Leader让小杨负责排查。

小杨登录直播服务器，查看日志和监控：

```bash
# 查看网络状况
$ netstat -s | grep -E "segments|retransmit|loss"
    12345678 segments sent
      987654 retransmitted        # 重传率接近 8%！
       12345 segments retransmited
    98765432 segments received
      567890 duplicate ACKs       # 大量重复确认

# 查看服务进程的线程模型
$ ps -eLf | grep live_stream | wc -l
2567   # 2500+ 个线程！每个 TCP 连接一个线程
```

**疑惑**：直播用的不是 RTMP 协议吗？RTMP 基于 TCP，TCP 是"可靠"的——那为什么直播画面还会卡？TCP 保证数据可靠到达，但为什么"可靠"反而导致了直播的卡顿？

小杨做了个实验：用 tcpdump 抓包，发现每当网络有轻微抖动时，TCP 就会主动降低发送速度（拥塞控制），而且因为 TCP 要求数据有序到达，只要一个包丢了，后面即使收到的包也要等重传——画面就卡住了。

```bash
# 抓包分析
$ tcpdump -i eth0 -A port 1935 | head -20
# 发现大量的 TCP Dup ACK 和 Fast Retransmit
# 以及 TCP ZeroWindow（接收窗口为0，通知发送端停止发送）
```

**追问链**：

- "TCP 不是最靠谱的协议吗？为什么直播用 TCP 还会卡？" → 因为 TCP 的"可靠"是**牺牲了实时性**换来的——它要保证数据**有序、不丢、不重**，但在网络抖动时，**队头阻塞**会让后续数据包干等
- "那为什么不能换 UDP？UDP 不是不可靠吗？" → **UDP 的"不可靠"正是它低延迟的原因**——丢了就丢了，不需要重传，不需要排序，数据到了就直接交给应用层
- "但直播中关键的帧（如 I 帧）不能丢啊，丢了画面就花屏了" → 所以需要在 UDP 之上自己做**应用层可靠传输**，只对关键帧做重传，非关键帧（P/B 帧）丢了就跳过——这就是**选择性可靠**
- "那淘宝直播、抖音、快手他们也是这么做的吗？" → 是的，主流直播平台都基于 UDP 实现了自己的传输协议（如阿里云的 ARP、腾讯的 TCC），在 UDP 之上做了**可定制的可靠层**
- "那什么时候该用 TCP，什么时候该用 UDP？有没有一种协议能兼顾两者？" → 这正是本章要回答的核心问题

> 小杨后来把直播传输层改成了基于 UDP 的自定义协议——关键帧（I 帧）做可靠传输 + 非关键帧（P/B 帧）直接发不管丢不丢，配合前向纠错（FEC），直播卡顿率从 15% 降到了 1%。但他花了两周才搞懂 TCP 和 UDP 的本质差异——这就是为什么要学传输层协议的原因。

### __NEW_1_2__为什么要学传输层协议

```mermaid
flowchart TB
    subgraph 困惑[面试/工作中的困惑]
        A["TCP 可靠但慢<br/>UDP 快但不可靠"]
        B["到底什么时候用 TCP<br/>什么时候用 UDP？"]
    end
    subgraph 答案[本章要回答的问题]
        C[TCP 如何保证可靠<br/>三次握手/重传/滑动窗口]
        D[UDP 的设计哲学<br/>零开销/应用自行控制]
        E[TCP 的代价<br/>队头阻塞/慢启动/TIME_WAIT]
        F[在 UDP 上实现可靠传输<br/>QUIC/KCP/自定义协议]
        G[实战中的协议选择<br/>根据业务场景做决策]
    end
    A --> B
    B --> C
    B --> D
    C --> E
    D --> F
    E --> G
    F --> G
```

绝大多数开发者初学网络时都听说过这句话：**"TCP 可靠，UDP 不可靠"**。

但如果真的去选择传输协议，你会发现事情远没那么简单：
- 实时直播用 TCP → 卡顿（可靠的代价是队头阻塞）
- 文件下载用 UDP → 太复杂（应用层自己实现可靠性）
- 网页浏览用 TCP → 慢启动让短连接很慢
- 在线游戏用 UDP → 还得自己处理丢包

本章的目标，就是把这层关系彻底讲清楚：

1. **TCP 的可靠机制**：三次握手、四次挥手、滑动窗口、拥塞控制——它到底是怎么做到的？
2. **UDP 的简单哲学**：为什么零状态的设计在某些场景下反而是优势？
3. **TCP 的代价**：队头阻塞、慢启动、TIME_WAIT——因为什么牺牲了性能？
4. **在 UDP 之上构建可靠**：QUIC、KCP——新的趋势为什么都在 UDP 上"重新造轮子"？
5. **实战选择**：你的业务场景到底该用 TCP 还是 UDP？

带着这五个问题，我们从直播平台的"马赛克"事故开始，一步步拆解传输层协议的设计哲学。
"""

# Insert before the first ## heading
content = re.sub(r'(\n\n## 01\.传输协议场景)', new_section + r'\n\n## 01.传输协议场景', content)

# ====== 2. Renumber sections: old 01-07 → 02-08 ======
for old_n, new_n in [('07', '08'), ('06', '07'), ('05', '06'), ('04', '05'),
                       ('03', '04'), ('02', '03'), ('01', '02')]:
    content = re.sub(rf'^## {old_n}\.', f'## {new_n}.', content, flags=re.MULTILINE)

# Renumber ### subsections: 1.x→2.x...7.x→8.x
for old_s, new_s in [('7.', '8.'), ('6.', '7.'), ('5.', '6.'), ('4.', '5.'),
                      ('3.', '4.'), ('2.', '3.'), ('1.', '2.')]:
    content = re.sub(rf'^### {old_s}', f'### {new_s}', content, flags=re.MULTILINE)

# ====== 3. Replace placeholders ======
content = content.replace('__NEW_SEC__', '01.')
content = content.replace('__NEW_1_1__', '1.1 ')
content = content.replace('__NEW_1_2__', '1.2 ')

# ====== 4. Update TOC in #### 目录介绍 block ======
# Remove the old TOC lines between #### 目录介绍 and the first ## heading
# and replace with new TOC
new_toc = [
    '#### 目录介绍\n',
    '- 01.工作案例引入\n',
    '  - 1.1 直播平台的马赛克事故\n',
    '  - 1.2 为什么要学传输层协议\n',
    '- 02.传输协议场景\n',
    '  - 2.1 应用TCP的场景\n',
    '  - 2.2 应用UDP的场景\n',
    '  - 2.3 一些有争议场景\n',
    '  - 2.4 TCP发送案例分析\n',
    '  - 2.5 UPD发送案例分析\n',
    '- 03.TCP基础概念\n',
    '  - 3.1 TCP的特点\n',
    '  - 3.2 TCP对应的协议\n',
    '  - 3.3 一些常见概念\n',
    '  - 3.4 TCP报文结构\n',
    '  - 3.5 报文重点概念\n',
    '  - 3.6 TCP连接最大值\n',
    '  - 3.7 能同时发起握手\n',
    '- 04.TCP靠谱协议\n',
    '  - 4.1 如何建立连接\n',
    '  - 4.2 三次握手连接\n',
    '  - 4.3 四次挥手断开\n',
    '  - 4.4 如何保证可靠\n',
    '  - 4.5 如何保证顺序\n',
    '  - 4.6 如何避免丢包\n',
    '  - 4.7 如何连接维护\n',
    '  - 4.8 如何控制流量\n',
    '  - 4.9 如何避免拥塞\n',
    '  - 4.10 停止等待操作\n',
    '- 05.UDP基础概念\n',
    '  - 5.1 什么是UDP\n',
    '  - 5.2 UDP对应的协议\n',
    '  - 5.3 UDP的特点\n',
    '  - 5.4 UDP包数据\n',
    '  - 5.5 一些常见概念\n',
    '  - 5.6 UDP也能握手吗\n',
    '- 06.UDP不可靠协议\n',
    '  - 6.1 为何无连接\n',
    '  - 6.2 如何限制大小\n',
    '  - 6.3 丢包怎么办\n',
    '- 07.TCP和UDP实践\n',
    '  - 7.1 TCP实践案例\n',
    '  - 7.2 UDP实践案例\n',
    '- 08.TCP与UDP的设计哲学\n',
    '  - 8.1 可靠vs效率的权衡\n',
    '  - 8.2 TCP的设计代价\n',
    '  - 8.3 基于UDP的可靠传输\n',
    '  - 8.4 如何选择TCP还是UDP\n',
    '- 09.思考题与作业\n',
    '  - 9.1 基础思考题目\n',
    '  - 9.2 进阶思考题目\n',
    '  - 9.3 动手实践作业\n',
    '\n',
]

# Find the TOC block
toc_start = None
toc_end = None
for m in re.finditer(r'^#### 目录介绍\s*$', content, re.MULTILINE):
    toc_start = m.start()

if toc_start is not None:
    # Find where TOC ends (before the first ## heading or blank line after TOC)
    after_toc = content[toc_start:]
    m2 = re.search(r'\n\n## ', after_toc)
    if m2:
        toc_end = toc_start + m2.start()

if toc_start is not None and toc_end is not None:
    before = content[:toc_start]
    after = content[toc_end:]
    content = before + ''.join(new_toc) + after
    print(f"TOC updated")
else:
    print(f"TOC not found: start={toc_start}, end={toc_end}")

# ====== 5. Add thinking questions section at the end ======
thinking = """


## 09.思考题与作业

### 9.1 基础思考题目

1. **三次握手的必要性**：为什么 TCP 建立连接需要三次握手而不是两次？请从"防止已失效的连接请求到达服务器"和"确保双方都有收发能力"两个角度回答。

2. **四次挥手为什么比三次握手多一次**：关闭连接时，为什么服务端的 ACK 和 FIN 不能合并在一个报文里发送？什么情况下它们才会合并？

3. **TCP 流量控制和拥塞控制的区别**：请说明这两个概念的本质区别。流量控制的"窗口"和拥塞控制的"窗口"分别是谁负责计算的？如果网络很好但接收方处理很慢，哪个机制会起作用？

4. **UDP 为什么有长度限制**：UDP 数据报的最大长度是 65535 字节。这个限制来自哪里？为什么实际使用中建议不要超过 1472 字节（以太网 MTU）？

5. **TCP 的队头阻塞问题**：请用一句话描述什么是"队头阻塞"。为什么它在 HTTP/2 多路复用场景下尤其严重？HTTP/3（基于 QUIC）是怎么解决这个问题的？

### 9.2 进阶思考题目

1. **复习案例**：回到 1.1 节中直播平台的"马赛克"事故。小杨发现 TCP 的重传率高达 8%，而卡顿的直接原因是 TCP 的队头阻塞和拥塞控制。请分析：
   - 如果直播平台改用 UDP，但什么应用层机制都不加，会出现什么问题？
   - 如果对关键帧（I 帧）做可靠传输，对非关键帧（P/B 帧）不做，如何在应用层实现这种"选择性可靠"？
   - 前向纠错（FEC）是怎么工作的？为什么它比单纯的重传更适合实时流媒体？

2. **TIME_WAIT 的危害与对策**：在高并发短连接场景下（如 Web 服务器），TIME_WAIT 状态会导致端口耗尽。请说明 TIME_WAIT 为什么要等待 2MSL（而不是更短）？列举至少 3 种缓解 TIME_WAIT 问题的方法。

3. **QUIC 为什么比 TCP 快**：QUIC 在连接建立上比 TCP 少了多少个 RTT？QUIC 是如何实现"无队头阻塞"的多路复用的？为什么说 QUIC 的"连接迁移"特性对移动端应用至关重要？

4. **SYN Flood 攻击的原理**：攻击者发送大量 SYN 包但不完成三次握手，导致服务器维护大量半连接。请从"服务器的连接表"和"内存消耗"两个角度分析攻击原理，并说明 SYN Cookie 是如何防御的。

5. **KCP 的激进重传策略**：KCP 选择了"收到 2 个重复 ACK 就重传"（TCP 需要 3 个）。这种激进的策略在什么场景下是优势？在什么场景下可能浪费带宽？如果网络中有大量乱序包，KCP 会有什么问题？

### 9.3 动手实践作业

**作业一（必做）**：抓包分析 TCP 三次握手和四次挥手。

- 用 Wireshark 或 tcpdump 抓取一次 HTTP 请求的完整 TCP 流。
- 在抓包结果中找出三次握手的三个包（SYN / SYN+ACK / ACK），标注它们的序列号、确认号和窗口大小。
- 再找出四次挥手的四个包（FIN / ACK / FIN / ACK），观察 TIME_WAIT 状态的持续时间。
- 思考：如果三次握手中的第二次握手丢了（SYN+ACK 丢失），从抓包中能看到什么？

**作业二（必做）**：对比 TCP 和 UDP 的传输延迟差异。

- 写一个小程序（Python/C），分别用 TCP 和 UDP 发送 1000 条长度为 100 字节的消息到本地回环地址（127.0.0.1）。
- 测量从发送到收到回复的平均 RTT，对比 TCP 和 UDP 的差异。
- 再把发送方和接收方放在两台真实的机器上（中间经过一个路由器），重新测试。UDP 的延迟优势在跨网络时是否更明显？

**作业三（选做）**：模拟 TCP 拥塞控制实验。

- 编写一个简单的网络模拟程序（或使用 Mininet），模拟一个带宽受限、有随机丢包的链路。
- 在链路上跑 TCP 流量，监控 cwnd（拥塞窗口）的变化曲线。画出 cwnd 随时间的变化图，标注出慢启动、拥塞避免、快重传/快恢复的阶段。
- 改变丢包率（0.1%、1%、5%），观察 TCP 吞吐量的变化。思考：在高丢包率下，为什么 TCP 的表现极差？

**作业四（拓展）**：动手搭建一个简单的 QUIC 服务。

- 使用 quic-go（Go 语言）或 aioquic（Python）搭建一个简单的 HTTP/3 服务端和客户端。
- 对比 HTTP/1.1（基于 TCP）和 HTTP/3（基于 QUIC）在同一网络条件下的首屏加载时间。
- 把对比结果整理成表格，分析 QUIC 在哪些环节节省了时间（连接建立、队头阻塞消除等）。
"""

content = content.rstrip() + thinking

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)

print("Done")
