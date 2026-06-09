import re

path = "/Users/yc/YCBookBlog/docs/07.计算机/02.网络协议/05.网络编程模型的概念.md"

with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# ====== 1. Insert case intro before ## 01.客户端和服务端 ======
new_section = r"""
## __NEW_SEC__工作案例引入

### __NEW_1_1__万人聊天室项目崩了

**场景**：小陈是一名刚入职半年的后端开发，Leader交给他一个任务——写一个聊天室服务器，支持1万人同时在线聊天。

小陈心想：这不就是"每个客户端连上来就开一个线程处理"吗？两天就写完了。

```python
# 小陈的第一版聊天服务器（伪代码）
def handle_client(sock):
    while True:
        data = sock.recv(4096)       # 等着收消息
        if not data: break
        broadcast(data)              # 群发给所有人

server = create_server(port=8888)
while True:
    client = server.accept()         # 等着新连接
    threading.Thread(target=handle_client, args=(client,)).start()
```

上线测试，100个用户——稳。500个用户——还行。1000个用户——服务器卡死了。

```bash
$ ps aux | grep chat_server
yc  12345  3.2  1.5  2.1g  120m  ...  chat_server
# 2000个线程，光是线程栈就占了 2GB 内存

$ netstat -an | grep 8888 | wc -l
987  # 连接数还不到1000

$ top -b -n 1 | grep chat_server
    PID USER      PR  NI    VIRT    RES    SHR S  %CPU %MEM
  12345 yc        20   0   2.1g  1.2g  1.1g D  98.7  3.2
# 状态 D（不可中断睡眠），CPU 100%
```

**疑惑**：每来一个用户就开一个线程，多简单多直观——为什么一上千人就扛不住了？难道"一个连接一个线程"的做法有什么问题？线程多了会怎样？

**追问链**：

- "为什么每个线程只开一个连接，1000个线程就撑不住了？" → 每个线程默认栈 1~8MB，1000个线程光是栈就占 1~8GB 内存——**线程本身是有成本的**
- "那是不是可以用线程池？" → 线程池限制最大线程数，但阻塞IO意味着每个线程在 `recv()` 时都会挂起等待数据，如果 1000 个连接同时活跃，池里的线程都被占满，新的请求进不来——**阻塞IO的瓶颈**
- "那有没有办法用一个线程处理多个连接？" → 这就是**IO多路复用**的方案：一个线程可以同时监视几百上千个 socket，谁有数据就处理谁
- "为什么 `recv()` 会阻塞？阻塞和非阻塞IO背后是什么原理？" → 阻塞IO调用会引发进程上下文切换到内核态，内核等待数据到达后复制到用户空间才返回——**这就是系统调用的代价**
- "那 IO 多路复用（select/poll/epoll）是怎么工作的？它和 Reactor 模式、Proactor 模式是什么关系？" → 这就是本章要回答的核心问题

> 小陈后来把代码改成了 epoll + 非阻塞IO + 事件驱动，只用 4 个线程就支撑住了 1 万用户。但他花了整整一周才搞懂背后的原理——这就是我们为什么要学网络编程模型的原因。

### __NEW_1_2__为什么要学网络编程模型

```mermaid
flowchart TB
    subgraph 问题[新人常见困惑]
        A["一个连接一个线程<br/>写起来最直观"]
        B["但 1000 个连接就卡死<br/>为什么？"]
    end
    subgraph 答案[本章要回答的问题]
        C[阻塞 vs 非阻塞IO]
        D[同步 vs 异步IO]
        E[IO多路复用 epoll]
        F[Reactor 与 Proactor]
    end
    A --> B
    B --> C
    B --> D
    C --> E
    D --> F
    E --> G["用少量线程<br/>处理大量连接"]
    F --> G
```

新手写网络程序时几乎都会踩进同一个坑：**以为"每连接每线程"是万能方案**。这个坑背后的知识体系正是网络编程模型要解决的核心问题：

1. **IO模型**：阻塞IO、非阻塞IO、IO多路复用、异步IO——它们的区别是什么？各自适合什么场景？
2. **系统原理**：`accept()`、`recv()` 这些系统调用在操作系统层到底做了什么？
3. **并发模型**：Reactor 和 Proactor 模式怎么解决"大量连接、少量线程"的问题？
4. **协议设计**：TCP 的字节流特性为什么会导致"粘包"？应用层该怎么设计？

带着这四个问题，我们从"万人聊天室"的崩塌开始，逐一拆解网络编程中最重要的这几个概念。
"""

# Insert before the first ## heading (skip frontmatter)
# Find ## 01.客户端和服务端
content = re.sub(r'(\n\n## 01\.客户端和服务端)', new_section + r'\n\n## 01.客户端和服务端', content)

# ====== 2. Renumber sections: old 01-08 → 02-09 ======
for old_n, new_n in [('08', '09'), ('07', '08'), ('06', '07'), ('05', '06'),
                       ('04', '05'), ('03', '04'), ('02', '03'), ('01', '02')]:
    content = re.sub(rf'^## {old_n}\.', f'## {new_n}.', content, flags=re.MULTILINE)

# Renumber ### subsections: 1.x→2.x, 2.x→3.x ... 8.x→9.x
for old_s, new_s in [('8.', '9.'), ('7.', '8.'), ('6.', '7.'), ('5.', '6.'),
                      ('4.', '5.'), ('3.', '4.'), ('2.', '3.'), ('1.', '2.')]:
    content = re.sub(rf'^### {old_s}', f'### {new_s}', content, flags=re.MULTILINE)

# ====== 3. Replace placeholders ======
content = content.replace('__NEW_SEC__', '01.')
content = content.replace('__NEW_1_1__', '1.1 ')
content = content.replace('__NEW_1_2__', '1.2 ')

# ====== 4. Add thinking questions section at the end ======
thinking = """


## 10.思考题与作业

### 10.1 基础思考题目

1. **阻塞与非阻塞的区别**：请用你自己的话解释，当调用 `recv()` 但数据还没到达时，阻塞模式和非阻塞模式下分别发生了什么？从"进程状态"和"系统调用返回值"两个角度说明。

2. **同步异步区分**：很多人把"阻塞"等同于"同步"，把"非阻塞"等同于"异步"。这种说法准确吗？请用"数据拷贝由谁完成"这个关键维度重新区分这四个概念。

3. **IO多路复用对比**：select、poll、epoll 三者都是 IO 多路复用机制。请说明：
   - 每次调用时需要拷贝什么数据？
   - 能处理的最大连接数分别是多少？
   - 就绪事件的通知方式有什么不同？

4. **Reactor模式**：请画出"单 Reactor 单线程"和"主从 Reactor 多线程"的结构图，并说明各自的优缺点和适用场景。

5. **粘包问题**：TCP 是面向字节流的协议，不保留消息边界。当用 TCP 发送两条消息时，接收方可能一次收到两条、也可能分多次收到。请列出至少3种解决粘包问题的方案，并说明各自的优缺点。

### 10.2 进阶思考题目

1. **复习案例**：回到 1.1 节中小陈的"万人聊天室"案例。他的第一版代码用了"每连接每线程 + 阻塞IO"。请分析：
   - 当连接数增长到 1000 时，系统为什么会卡死？（从内存、CPU 上下文切换、线程调度三个角度）
   - 如果改用"线程池 + 阻塞IO"，能解决吗？为什么？
   - 最终用"epoll + 非阻塞IO + 事件驱动"的方案为什么能用 4 个线程处理 1 万用户？

2. **epoll 的边缘触发 vs 水平触发**：epoll 支持两种触发模式。请说明它们的区别，并分析在什么场景下应该选择哪种模式。一个常见的 Bug 是在边缘触发模式下没有读完所有数据就继续等待——这会导致什么问题？

3. **io_uring 的革新**：Linux 5.1 引入的 io_uring 被称为"下一代异步IO"方案。它与 epoll 最大的不同是什么？为什么说它（而不是 epoll）才是真正的"异步IO"？请从"提交队列"和"完成队列"的共享内存设计角度分析。

4. **零拷贝与网络编程**：sendfile() 系统调用可以把一个文件直接从磁盘发送到 socket，不需要经过用户空间。请说明它的工作流程，并分析它在"阻塞 vs 非阻塞"这个维度上属于哪种IO模型。如果用 io_uring 来实现零拷贝，会有什么优势？

5. **Netty 中的三种 Reactor 模式**：Netty 支持单线程模型、多线程模型、主从多线程模型。请画出这三种模型的线程模型图，并说明各适合什么类型的应用（如：Redis vs Nginx vs Netty 默认配置）。

### 10.3 动手实践作业

**作业一（必做）**：自己动手验证阻塞与非阻塞的区别。

- 写一个简单的 Python 或 C 程序：创建两个 socket，分别设置为阻塞模式和非阻塞模式。连接一个不存在的 IP（如 10.255.255.1:9999），观察两种模式下的行为和返回时间差异。
- 用 `strace`（Linux）或 `dtruss`（macOS）跟踪系统调用，看看阻塞模式下 `connect()` 和 `recv()` 到底停在哪个内核函数上。

**作业二（必做）**：用 epoll 重写万人聊天室。

- 参考 1.1 节中小陈的最终方案，用 epoll（或 kqueue，macOS 用户）实现一个简单的 echo 服务器：接收客户端消息，原样返回。
- 注意处理以下边界情况：`EAGAIN`/`EWOULDBLOCK` 错误、消息分片（粘包）、客户端断开连接。
- 用 `ab`（Apache Bench）或自己写客户端并发工具压测，对比"每连接每线程"和"epoll"两种实现分别在 100/500/1000 并发下的资源消耗（内存和 CPU）。

**作业三（选做）**：抓包分析 TCP 流。

- 用 Wireshark 或 tcpdump 抓取一次 HTTP 请求的 TCP 数据流。
- 观察 TCP 是如何把应用层的数据拆分成多个段的（wireshark 中可以看到 "TCP segment of a reassembled PDU"）。
- 对照 5.4 节"字节流的设计"中的粘包分析，验证 TCP 确实不保留消息边界。

**作业四（拓展）**：深入理解一种 Reactor 框架。

- 选择一个开源 Reactor 框架阅读其核心源码，如：libevent（C）、Netty（Java）、muduo（C++）。
- 画出它们的线程模型图，标注清楚 Reactor（事件分发器）、Handler（事件处理器）和 Worker（业务处理线程池）之间的交互关系。
- 说明它们在处理"慢业务"（如数据库查询）时，是如何避免阻塞 Reactor 线程的。
"""

content = content.rstrip() + thinking

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)

print("Done")
