



## 线程通信的核心设计与原理

线程通信的本质问题是：**多个线程共享同一进程的内存空间，如何安全地协调数据访问和执行顺序。**

### 两大核心范式


**2. 消息传递（Message Passing）**

线程之间不直接共享数据，而是通过**通道/队列**发送消息：

- **Go**：`chan`（CSP 模型）— "Don't communicate by sharing memory; share memory by communicating"
- **Rust**：`mpsc::channel`
- **Erlang/Elixir**：Actor 模型，每个进程有自己的邮箱
- **C++**：没有语言级 channel，通常用 `std::queue` + `mutex` + `condition_variable` 手动实现

### 核心要解决的三个问题

```
┌──────────────┐
│  互斥(Mutual │  同一时刻只有一个线程修改共享数据
│  Exclusion)  │  → mutex, atomic, spinlock
└──────┬───────┘
       │
┌──────▼───────┐
│  同步(Sync/  │  线程间协调执行顺序（A 做完再 B 做）
│  Ordering)   │  → condition_variable, semaphore, barrier, future/promise
└──────┬───────┘
       │
┌──────▼───────┐
│  可见性      │  一个线程的写入对其他线程可见
│ (Visibility) │  → memory barrier, volatile(Java), atomic memory order
└──────────────┘
```

### 内存模型（Memory Model）

这是最底层的原理。现代 CPU 有多级缓存、乱序执行、Store Buffer，导致：
- 线程 A 写了变量 x=1，线程 B 可能读到旧值（缓存不一致）
- 编译器/CPU 可能重排指令顺序

因此 C++11 定义了内存模型（`std::memory_order`），规定了不同原子操作的可见性保证：

```
memory_order_relaxed  → 无序，仅保证原子性
memory_order_acquire  → 读操作之后的读写不会被重排到前面
memory_order_release  → 写操作之前的读写不会被重排到后面
memory_order_seq_cst  → 全局顺序一致（最强，默认）
```

### 常见模式总结

| 模式 | 实现 | 适用场景 |
|------|------|----------|
| 生产者-消费者 | queue + mutex + cond_var | 任务分发、线程池 |
| 读写锁 | shared_mutex | 读多写少 |
| Future/Promise | std::future/std::promise | 异步获取单次结果 |
| 无锁队列 | atomic + CAS | 高性能低延迟 |
| 事件通知 | eventfd / pipe / cond_var | 线程唤醒 |

**一句话**：线程通信的本质就是在**共享内存可见性**和**执行顺序**上建立约定，要么通过锁+条件变量显式同步，要么通过消息传递隐式同步（把同步封装在 channel 内部）。