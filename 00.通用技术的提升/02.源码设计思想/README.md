






### 3.3 M:N 模型（混合线程）

```
用户线程:   T1  T2  T3  T4  T5  T6
             \   |   |   |   |   /     M个用户线程映射到N个内核线程
              \  |   |   |   |  /
内核线程:    KT1    KT2    KT3
              \      |      /
               ──CPU调度器──
```

- **实现**：Go goroutine（GMP 模型）、Erlang process、Java 21 Virtual Thread
- **优点**：兼具用户级线程的轻量和内核级线程的并行能力
- **缺点**：调度器实现复杂

### 3.4 Go GMP 模型（M:N 的经典实现）

```
           ┌─── P (Processor，逻辑处理器) ───┐
           │  本地运行队列：[G1, G2, G3]      │
           │  当前运行的 G：G0                 │
           └──────────┬──────────────────────┘
                      │ 绑定
                      ▼
                M (Machine，OS 内核线程)
                      │
                    CPU 核

G = Goroutine（用户态协程，初始栈仅 2KB）
M = Machine（真实 OS 线程，栈 1~8MB）
P = Processor（调度上下文，数量 = GOMAXPROCS）

核心设计：
- Work Stealing：P 的本地队列空了，从其他 P 偷任务
- Hand Off：M 阻塞在 syscall 时，P 解绑并交给新 M
- Preemption：Go 1.14+ 基于信号的异步抢占
```

---

## 四、线程架构设计

### 4.1 操作系统层面（以 Linux 为例）

Linux 中线程和进程在内核中统一用 `task_struct` 表示：

```c
// 简化的 task_struct
struct task_struct {
    pid_t pid;                  // 线程 ID（每个线程唯一）
    pid_t tgid;                 // 线程组 ID（= 进程 PID，同进程的线程 tgid 相同）
    
    struct mm_struct *mm;       // 内存描述符 → 同进程的线程共享同一个 mm
    struct files_struct *files; // 文件描述符表 → 共享
    struct signal_struct *signal; // 信号处理 → 共享
    
    struct thread_struct thread; // CPU 寄存器状态 → 每线程独立
    void *stack;                 // 内核栈 → 每线程独立
    // ...
};
```

**`fork()` vs `clone()`**：

```
fork()  → 创建新进程：复制 mm、files、signal（写时复制）
clone() → 创建线程：共享 mm、files、signal（通过标志位控制）

clone(CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD, ...)
       共享内存    共享文件系统  共享文件描述符  共享信号处理    同一线程组
```

### 4.2 线程上下文切换

```
线程 A 运行中
    │
    ▼ 时钟中断 / 系统调用 / 主动让出
┌──────────────────────────────┐
│ 1. 保存 A 的寄存器到 A 的 TCB │  (Thread Control Block)
│ 2. 切换内核栈指针              │
│ 3. 恢复 B 的寄存器从 B 的 TCB │
│ 4. 跳转到 B 的 PC 继续执行    │
└──────────────────────────────┘
    │
    ▼
线程 B 运行中
```

**同进程线程切换 vs 跨进程切换**：

| 操作 | 同进程线程切换 | 跨进程切换 |
|------|--------------|-----------|
| 保存/恢复寄存器 | 需要 | 需要 |
| 切换页表（CR3） | **不需要** | 需要 |
| 刷新 TLB | **不需要** | 需要（代价极大） |
| 典型耗时 | ~1-2 μs | ~3-5 μs |

### 4.3 线程栈架构

```
高地址 ┌──────────────┐
       │  Guard Page   │ ← 栈溢出保护（访问触发 SIGSEGV）
       ├──────────────┤
       │              │
       │  线程栈空间   │ ← 默认 1~8MB（Linux 默认 8MB）
       │  (向下增长)   │
       │              │
       │      ↓       │
       │    栈顶 SP   │ ← 当前栈指针
       │              │
       ├──────────────┤
       │     TLS      │ ← 线程局部存储
低地址 └──────────────┘
```

### 4.4 内存可见性架构

多核 CPU 的缓存层次导致线程间可见性问题：

```
  Thread 1 (Core 0)          Thread 2 (Core 1)
       │                          │
   ┌───┴───┐                  ┌───┴───┐
   │L1 Cache│                  │L1 Cache│  ← 每核私有，可能有过期数据
   └───┬───┘                  └───┬───┘
   ┌───┴───┐                  ┌───┴───┐
   │L2 Cache│                  │L2 Cache│
   └───┬───┘                  └───┬───┘
       └──────────┬───────────────┘
              ┌───┴───┐
              │L3 Cache│  ← 共享
              └───┬───┘
              ┌───┴───┐
              │ 主内存  │
              └───────┘
```

**解决方案**——内存屏障（Memory Barrier）/ 缓存一致性协议（MESI）：
- **Store Barrier**：确保写操作对其他核可见
- **Load Barrier**：确保读到最新值
- **Full Barrier**：两者兼具

各语言的关键字映射：
- Java: `volatile`、`synchronized`
- C/C++: `std::atomic`、`memory_order_*`
- Go: `sync/atomic`、channel

---

## 五、线程生命周期

### 5.1 状态定义

```
         ┌──────────┐
         │   NEW    │ 创建但未启动
         └────┬─────┘
              │ start()
              ▼
         ┌──────────┐          ┌───────────┐
    ┌───→│ RUNNABLE │────────→ │  RUNNING  │───┐
    │    └──────────┘ 被调度    └─────┬─────┘   │
    │         ↑                      │          │
    │         │                      │          │
    │    调度器选中              以下事件触发      │
    │         │                      │          │
    │    ┌────┴──────────────────────┤          │
    │    │              ┌────────────┤          │
    │    │              │            │          │
    │    │              ▼            ▼          │
    │  ┌─┴────────┐ ┌────────┐ ┌─────────┐    │
    │  │ WAITING  │ │TIMED   │ │ BLOCKED │    │
    │  │(无限等待) │ │WAITING │ │(锁阻塞)  │    │
    │  └────┬─────┘ └───┬────┘ └────┬────┘    │
    │       │           │           │          │
    │  notify/     超时到期/     获得锁         │
    │  unpark      notify                      │
    │       │           │           │          │
    │       └───────────┴───────────┘          │
    │               │                          │
    └───────────────┘                          │
                                               │
                          run()结束 / 异常       │
                                               │
                                    ┌──────────▼──┐
                                    │ TERMINATED  │
                                    └─────────────┘
```

### 5.2 每种状态的流转细节

#### NEW → RUNNABLE

```java
Thread t = new Thread(task);  // NEW: 仅分配 Java 对象
t.start();                     // → RUNNABLE: 
                               //   1. JVM 调用 pthread_create() 创建内核线程
                               //   2. 分配线程栈（默认 1MB）
                               //   3. 加入调度器就绪队列
```

**注意**：`start()` 只能调用一次，重复调用抛 `IllegalThreadStateException`。

#### RUNNABLE ↔ RUNNING

这两个状态在 Java 中统一为 `RUNNABLE`，但操作系统层面是区分的：

```
RUNNABLE（就绪队列中等待 CPU）
    │
    │ 调度器分配 CPU 时间片
    ▼
RUNNING（正在 CPU 上执行）
    │
    ├── 时间片用完 → 回到 RUNNABLE（时间片轮转）
    ├── 更高优先级线程抢占 → 回到 RUNNABLE
    └── 主动 yield() → 回到 RUNNABLE
```

#### RUNNING → BLOCKED（锁阻塞）

```java
synchronized (lockObj) {   // 如果锁被占用
    // ...                 // → BLOCKED: 进入锁的等待队列（EntryList）
}                          // 锁释放后 → RUNNABLE
```

操作系统层面的实现：
```
线程尝试获取锁
    │
    ├── CAS 成功 → 获得锁，继续执行
    │
    └── CAS 失败 → 自旋若干次（Adaptive Spinning）
              │
              ├── 自旋成功 → 获得锁
              └── 自旋失败 → 内核态 futex_wait() → 线程挂起
                              │
                           锁释放时 futex_wake() → 线程唤醒 → RUNNABLE
```

#### RUNNING → WAITING（无限等待）

| 触发方法 | 唤醒方式 |
|---------|---------|
| `Object.wait()` | `notify()` / `notifyAll()` |
| `Thread.join()` | 目标线程终止 |
| `LockSupport.park()` | `LockSupport.unpark(t)` |
| `Condition.await()` | `Condition.signal()` |

`wait/notify` 的内部机制：

```
synchronized (monitor) {
    monitor.wait();      // 1. 释放 monitor 锁
                         // 2. 线程进入 WaitSet
                         // 3. 状态 → WAITING
                         
    // 被 notify 唤醒后：
                         // 4. 从 WaitSet 移到 EntryList
                         // 5. 状态 → BLOCKED（等待重新获取锁）
                         // 6. 获得锁后 → RUNNABLE
}
```

```
┌─────── Monitor（对象监视器）───────┐
│                                    │
│   Owner:  当前持有锁的线程          │
│                                    │
│   EntryList: [T2, T3]             │  ← BLOCKED 状态线程
│   (等待获取锁的线程)                │
│                                    │
│   WaitSet:  [T4, T5]             │  ← WAITING 状态线程
│   (调用 wait() 的线程)             │
│                                    │
└────────────────────────────────────┘
```

#### RUNNING → TIMED_WAITING（限时等待）

| 触发方法 | 唤醒方式 |
|---------|---------|
| `Thread.sleep(ms)` | 超时自动唤醒 |
| `Object.wait(ms)` | 超时 / `notify()` |
| `Thread.join(ms)` | 超时 / 目标线程终止 |
| `LockSupport.parkNanos(ns)` | 超时 / `unpark()` |

内部实现依赖操作系统定时器：
```
sleep(1000)
  → futex_wait(addr, val, timeout=1s)
    → 内核注册 hrtimer（高精度定时器）
      → 定时器到期 → 唤醒线程 → RUNNABLE
```

#### → TERMINATED

```
run() 方法正常返回
    或
run() 方法抛出未捕获异常
    │
    ▼
TERMINATED:
  1. 释放线程栈内存
  2. 清理 TLS
  3. 唤醒所有 join() 等待的线程
  4. pthread_detach / 回收内核资源
  5. Thread 对象仍在堆上（等待 GC）
```

---

## 六、线程核心 API 设计

### 6.1 生命周期控制

| API | 语义 | 设计要点 |
|-----|------|---------|
| `new Thread(Runnable)` | 创建线程 | 将"做什么"（Runnable）与"谁来做"（Thread）分离 |
| `start()` | 启动线程 | 不可重入，内部通过 native 调用 `pthread_create` |
| `join()` / `join(timeout)` | 等待线程终止 | 底层用 `wait/notify` 实现（线程终止时 JVM 自动 `notifyAll`） |
| `interrupt()` | 设置中断标志 | **协作式取消**，不强制终止；阻塞 API 检测到标志抛 `InterruptedException` |
| `isAlive()` | 是否存活 | NEW 和 TERMINATED 返回 false |

**`interrupt()` 的协作式设计是关键设计决策**：

```java
// 错误思想：stop()（已废弃）—— 强制杀死线程
//   问题：可能在持有锁的临界区中被杀，导致数据不一致

// 正确思想：interrupt() —— 通知线程"请你终止"
//   线程自己决定何时、如何安全地终止

public void run() {
    while (!Thread.currentThread().isInterrupted()) {
        try {
            // 业务逻辑
            blockingOperation();  // sleep/wait/take 等
        } catch (InterruptedException e) {
            // 收到中断信号，清理资源后退出
            cleanup();
            break;  // 或 Thread.currentThread().interrupt() 传播
        }
    }
}
```

### 6.2 调度控制

| API | 语义 | 底层原理 |
|-----|------|---------|
| `sleep(ms)` | 当前线程休眠 | `nanosleep()` 系统调用，**不释放锁** |
| `yield()` | 提示调度器让出 CPU | `sched_yield()` 系统调用，调度器可忽略 |
| `setPriority(p)` | 设置优先级 | 映射到 OS 线程优先级（`nice` 值），效果因 OS 而异 |

**`sleep` vs `wait` 的本质区别**：

```
sleep(ms):
  - Thread 类的静态方法
  - 不需要持有锁
  - 不释放任何锁
  - 纯粹的时间等待

wait(ms):
  - Object 实例方法
  - 必须在 synchronized 块内调用
  - 释放当前对象的 monitor 锁
  - 等待条件变化的通知
```

### 6.3 同步原语

```
低级 ──────────────────────────────────────── 高级
  │                                              │
  ├─ CAS（CPU 指令: cmpxchg）                    │
  │    └─ Atomic 类                              │
  │         └─ AQS (AbstractQueuedSynchronizer)  │
  │              ├─ ReentrantLock                 │
  │              ├─ Semaphore                     │
  │              ├─ CountDownLatch                │
  │              ├─ CyclicBarrier                 │
  │              └─ ReadWriteLock                 │
  │                                              │
  ├─ futex (Linux 快速用户态互斥)                  │
  │    └─ pthread_mutex                          │
  │         └─ synchronized (JVM Monitor)        │
  │                                              │
  └─ 内存屏障 (mfence/lfence/sfence)             │
       └─ volatile                               │
```

**AQS 的核心设计**（Doug Lea 的杰作）：

```
┌─────────── AQS ────────────┐
│                              │
│   state: int (原子变量)       │  ← 0=未锁, >0=已锁/重入次数
│                              │
│   CLH Queue (双向链表):       │  ← 等待线程排队
│   head ↔ Node(T1) ↔ Node(T2)│
│                              │
│   acquire(): 尝试获取         │
│     CAS(state) 成功 → 获得   │
│     失败 → 入队 + park()     │
│                              │
│   release(): 释放            │
│     修改 state               │
│     unpark(队首线程)          │
└──────────────────────────────┘
```

### 6.4 线程间通信

| 机制 | 原理 | 适用场景 |
|------|------|---------|
| `wait/notify` | Monitor 的 WaitSet | 经典生产者-消费者 |
| `Lock/Condition` | AQS + 条件队列 | 精细化条件等待（多个条件） |
| `BlockingQueue` | Lock + Condition 封装 | 生产者-消费者（推荐） |
| `volatile` | 内存屏障保证可见性 | 状态标志、双检锁 |
| `CountDownLatch` | AQS state 倒数到 0 | 等待 N 个任务完成 |
| `CyclicBarrier` | ReentrantLock + Condition | N 个线程互相等待到齐 |

---

## 七、线程使用

### 7.1 线程池——生产级使用方式

直接创建线程的问题：创建/销毁开销、无法控制并发数、缺乏任务排队。

**线程池架构**：

```
                    ┌─────── ThreadPool ────────┐
                    │                            │
  submit(task) ──→  │  BlockingQueue             │
                    │  [task1, task2, task3, ...] │
                    │        │  │  │              │
                    │        ▼  ▼  ▼              │
                    │  Worker Threads:            │
                    │  [W1] [W2] [W3] ... [Wn]   │
                    │                            │
                    │  拒绝策略:                   │
                    │  队列满 + 线程满 → reject   │
                    └────────────────────────────┘
```

**核心参数设计**：

```java
ThreadPoolExecutor(
    int corePoolSize,      // 核心线程数（常驻）
    int maximumPoolSize,   // 最大线程数
    long keepAliveTime,    // 非核心线程空闲存活时间
    TimeUnit unit,
    BlockingQueue<Runnable> workQueue,    // 任务队列
    ThreadFactory threadFactory,          // 线程工厂
    RejectedExecutionHandler handler     // 拒绝策略
)
```

**任务提交决策流程**：

```
提交任务
    │
    ▼
当前线程数 < corePoolSize ?
    ├── 是 → 创建新核心线程执行
    └── 否 ↓
        workQueue 未满 ?
        ├── 是 → 任务入队等待
        └── 否 ↓
            当前线程数 < maximumPoolSize ?
            ├── 是 → 创建新非核心线程执行
            └── 否 → 执行拒绝策略
                      ├─ AbortPolicy（抛异常，默认）
                      ├─ CallerRunsPolicy（提交者自己执行）
                      ├─ DiscardPolicy（静默丢弃）
                      └─ DiscardOldestPolicy（丢弃队首）
```

### 7.2 线程池参数调优原则

```
CPU 密集型任务：
  corePoolSize = CPU 核数 + 1
  队列用有界队列
  理由：CPU 始终在计算，更多线程只增加切换开销

I/O 密集型任务：
  corePoolSize = CPU 核数 × 2 （或更高）
  更精确公式：N × U × (1 + W/C)
    N = CPU 核数
    U = 目标 CPU 利用率 (0~1)
    W = 等待时间
    C = 计算时间
  理由：线程大部分时间在等 I/O，需要更多线程保持 CPU 忙碌
```

### 7.3 并发设计模式

**模式一：不可变对象（最简单的线程安全）**
```java
// 不可变 = 天然线程安全，无需任何同步
public final class Point {
    private final int x, y;
    public Point(int x, int y) { this.x = x; this.y = y; }
}
```

**模式二：线程封闭（ThreadLocal）**
```java
// 每个线程有自己的副本，无共享 = 无竞争
private static final ThreadLocal<SimpleDateFormat> dateFormat =
    ThreadLocal.withInitial(() -> new SimpleDateFormat("yyyy-MM-dd"));
```

**模式三：生产者-消费者**
```java
BlockingQueue<Task> queue = new LinkedBlockingQueue<>(1000);

// 生产者
queue.put(task);      // 队列满则阻塞

// 消费者
Task task = queue.take();  // 队列空则阻塞
```

### 7.4 常见陷阱

```java
// 陷阱1：共享可变状态未同步
int count = 0;                    // ← 多线程 count++ 会丢失更新
AtomicInteger count = new AtomicInteger(0);  // ✓ 用原子类

// 陷阱2：死锁（交叉加锁）
// Thread1: lock(A) → lock(B)
// Thread2: lock(B) → lock(A)     // ← 互相等待
// 解法：统一加锁顺序，或用 tryLock + 超时

// 陷阱3：忘记处理 InterruptedException
try { Thread.sleep(1000); }
catch (InterruptedException e) { /* 空catch */ }  // ← 吞掉中断信号
// 正确做法：恢复中断标志或向上传播
catch (InterruptedException e) {
    Thread.currentThread().interrupt();  // 恢复标志
}
```

---

## 总结

```
进程太重 → 线程诞生（共享地址空间的轻量执行单元）
    │
    ├── 设计思想：资源容器(进程) 与 执行单元(线程) 正交分离
    │
    ├── 三种模型：1:1(OS原生) / N:1(用户态) / M:N(混合)
    │
    ├── 架构：task_struct + clone() + 栈 + TLS + 缓存一致性
    │
    ├── 生命周期：NEW → RUNNABLE ↔ RUNNING → BLOCKED/WAITING/TIMED_WAITING → TERMINATED
    │
    ├── 核心API：start/join/interrupt(协作取消) + sleep/wait/notify + Lock/AQS
    │
    └── 使用：线程池(生产标配) + 不可变/封闭/生产消费模式 + 避免死锁/竞争/中断丢失
```


- 01.线程基础概念
    - 1.1 线程问题答疑
    - 1.2 线程设计思想
    - 1.3 线程模型设计
    - 1.4 线程基础概念
    - 1.5 为何设计多线程
    - 1.6 线程调度机制
- 02.线程生命周期设计
    - 2.1 生命周期模型图
    - 2.2 Java线程生命周期
    - 2.3 生命周期如何流转
    - 2.4 线程销毁和死亡
    - 2.5 影响生命周期因素
- 03.线程核心Api设计
    - 3.0 线程简单使用api
    - 3.1 start方法设计原理
    - 3.2 sleep方法设计原理
    - 3.3 join方法设计原理
    - 3.4 yield方法设计原理
    - 3.5 stop方法设计原理
    - 3.6 interrupt方法设计
- 04.线程使用实践
    - 4.1 线程死锁怎么办
    - 4.2 线程异常如何处理
    - 4.3 如何确保线程安全
    - 4.4 线程睡眠和唤醒
    - 4.5 设置线程优先级
    - 4.6 尽量设置线程名称
- 05.核心原理探索&分析
    - 5.1 创建线程核心原理
    - 5.2 线程映射核心原理
- 06.线程问题排查&优化
    - 6.1 如何优化线程性能
    - 6.2 线程问题诊断分析


## 01.线程基础概念

### 1.1 线程问题答疑

面试官可能会以此为契机，从各种不同角度考察你对线程的掌握：

1. 相对理论一些的面试官可以会问你线程到底是什么以及Java底层实现方式。
2. 线程状态的切换，以及和锁等并发工具类的互动。
3. 线程编程时容易踩的坑与建议等。

比较底层的一些问题：

1. 线程锁机制，如何让线程切换状态。线程的并发设计核心思想是什么？

### 1.2 线程设计思想

线程是操作系统里的一个概念

虽然各种不同的开发语言如 Java、C# 等都对其进行了封装，但是万变不离操作系统。Java 语言里的线程本质上就是操作系统的线程，它们是一一对应的。

不管是那种语言设计线程，其核心思想大概是

1.设计线程并发性；2.设计线程共享资源；3.设计线程同步和互斥；4.设计异步编程；5.设计线程调度等等。

### 1.3 线程模型设计

Java的线程模型设计主要基于线程和锁的概念，以及相关的同步机制。以下是Java线程模型的一些关键设计要点：

1. 线程：Java中的线程是独立的执行单元，可以并发执行。线程的生命周期包括新建、就绪、运行、阻塞和终止等状态。
2. 同步：Java提供了synchronized关键字和Lock接口等机制来实现线程间的同步。保证多个线程对共享资源的访问顺序和互斥性。
3. 锁：Java中的锁是用于控制对共享资源的访问的机制。锁可以实现互斥访问和条件等待，确保线程安全和正确的执行顺序。
4. 线程间通信：Java提供了wait()、notify()和notifyAll()等方法来实现线程间的通信。
5. 线程池：Java的线程池是一种管理和复用线程的机制。
6. 并发集合：Java提供了一系列的并发集合类，例如ConcurrentHashMap、ConcurrentLinkedQueue等。


### 1.4 线程基础概念

线程（英语：thread）是操作系统能够进行运算调度的最小单位。

1. 被包含在进程之中，是进程中的实际运作单位。一条线程指的是进程中一个单一顺序的控制流。
2. 在一个进程内部又可以执行多个任务（并发/并行），每条线程并行执行不同的任务。线程是程序使用CPU的基本单位。注意：线程是依赖于进程存在的。

同进程不同线程资源共享

同一进程中的多条线程将共享该进程中的全部系统资源，如虚拟地址空间，文件描述符和信号处理等等。

但同一进程中的多个线程有各自的调用栈（call stack），自己的寄存器环境（register context），自己的线程本地存储（thread-local storage）。

线程有自己的栈（Stack）、寄存器（Register）、本地存储（Thread Local）等，但是会和进程内其他线程共享文件描述符、虚拟地址空间等。

### 1.5 为何设计多线程

设计多线程的背景：主要包括并发编程需求、多核处理器的普及、响应性和用户体验的要求、异步编程需求以及资源共享和协同工作的需求。

设计多线程主要目的：为了实现并发编程和提高程序的性能、响应能力以及资源利用率。

设计多线程是否带来其他问题：多线程编程也带来了一些挑战，如线程安全性、竞态条件、死锁等问题。这里面就涉及到锁机制，线程优先级，资源抢占等知识点！

多线程的作用不是提高执行速度，而是为了提高应用程序的使用率。程序在运行的使用，都是在抢CPU的时间片(执行权)，CPU在多线程程序中执行的时间要比单线程多，所以就提高了程序的使用率。哪个线程能抢占到CPU的资源呢，这个是不确定的，多线程具有随机性。

### 1.6 线程调度机制

应用程序在执行的时候都需要依赖于线程去抢占CPU的时间片，谁抢占到了CPU的时间片，那么CPU就会执行谁

线程的执行：假如我们的计算机只有一个 CPU，那么 CPU在某一个时刻只能执行一条指令，线程只有得到CPU时间片，也就是使用权，才可以执行指令。

线程有两种调度模型：

1. **分时调度模型**：所有线程轮流使用CPU的使用权，平均分配每个线程占用 CPU 的时间片。
2. **抢占式调度模型**：优先让优先级高的线程使用 CPU，如果线程的优先级相同，那么会随机选择一个，优先级高的线程获取的 CPU 时间片相对多一些。

Java使用的是抢占式调度模型。

## 02.线程生命周期设计

通用的线程生命周期基本上可以用下图这个“五态模型”来描述。这五态分别是：初始状态、可运行状态、运行状态、休眠状态和终止状态。

### 2.1 生命周期模型图

每一种状态分别做了什么事情

1. **初始状态**，指的是线程已经被创建，但是还不允许分配 CPU 执行。这个状态属于编程语言特有的，不过这里所谓的被创建，仅仅是在编程语言层面被创建，而在操作系统层面，真正的线程还没有创建。
2. **可运行状态**，指的是线程可以分配 CPU 执行。在这种状态下，真正的操作系统线程已经被成功创建了，所以可以分配 CPU 执行。
3. **运行状态**，当有空闲的 CPU 时，操作系统会将其分配给一个处于可运行状态的线程，被分配到 CPU 的线程的状态就转换成了运行状态。
4. **休眠状态**，运行状态的线程如果调用一个阻塞的 API（例如以阻塞方式读文件）或者等待某个事件（例如条件变量），那么线程的状态就会转换到休眠状态，同时释放 CPU 使用权，休眠状态的线程永远没有机会获得 CPU 使用权。当等待的事件出现了，线程就会从休眠状态转换到可运行状态。
5. **终止状态**，线程执行完或者出现异常就会进入终止状态，终止状态的线程不会切换到其他任何状态，进入终止状态也就意味着线程的生命周期结束。

### 2.2 Java线程生命周期

介绍完通用的线程生命周期模型，接下来就来详细看看 Java 语言里的线程生命周期是什么样的。

1. NEW（新建），表示线程被创建出来还没真正启动的状态，可以认为它是个Java内部状态。
2. RUNNABLE（就绪），表示该线程已经在JVM中执行，当然由于执行需要计算资源，它可能是正在运行，也可能还在等待系统分配给它CPU片段，在就绪队列里面排队。
3. BLOCKED（阻塞），这个状态和我们前面两讲介绍的同步非常相关，阻塞表示线程在等待Monitor lock。比如，线程试图通过synchronized去获取某个锁，但是其他线程已经独占了，那么当前线程就会处于阻塞状态。
4. WAITING（等待状态），当调用wait方法或者join方法等，会让线程处于等待执行状态，表示正在等待其他线程采取某些操作。
5. TIMED_WAITING（计时等待），其进入条件和等待状态类似，但是调用的是存在超时条件的方法，比如wait或join等方法的指定超时版本
6. TERMINATED（终止状态），当线程执行任务完毕则会自动销毁，并且销毁后不会流转到其他状态

注意点说明一下：就绪（RUNNABLE），在其他一些分析中，会额外区分一种状态RUNNING，但是从Java API的角度，并不能表示出来。

其实在操作系统层面，Java 线程中的 BLOCKED、WAITING、TIMED_WAITING 是一种状态。

即前面我们提到的休眠状态。也就是说只要 Java 线程处于这三种状态之一，那么这个线程就永远没有 CPU 的使用权。

### 2.3 生命周期如何流转

BLOCKED、WAITING、TIMED_WAITING 可以理解为线程导致休眠状态的三种原因。

那具体是哪些情形会导致线程从 RUNNABLE 状态转换到这三种状态呢？而这三种状态又是何时转换回 RUNNABLE 的呢？以及 NEW、TERMINATED 和 RUNNABLE 状态是如何转换的？

RUNNABLE（运行） 与 BLOCKED（阻塞） 的状态转换

1. 只有一种场景会触发这种转换，就是线程等待 synchronized 的隐式锁。synchronized 修饰的方法、代码块同一时刻只允许一个线程执行，其他线程只能等待，这种情况下，等待的线程就会从 RUNNABLE 转换到 BLOCKED 状态。
2. 当等待的线程获得 synchronized 隐式锁时，就又会从 BLOCKED 转换到 RUNNABLE 状态。有个疑问：线程调用阻塞式 API 时，是否会转换到 BLOCKED 状态呢？
3. 在操作系统层面，线程是会转换到休眠状态的，但是在 JVM 层面线程的状态不会发生变化，也就是说 Java 线程的状态会依然保持 RUNNABLE 状态。线程会阻塞，指的是操作系统线程的状态，并不是 Java 线程的状态。

RUNNABLE（运行） 与 WAITING （等待）的状态转换

1. 第一种场景，获得 synchronized 隐式锁的线程，调用无参数的 Object.wait() 方法。
2. 第二种场景，调用无参数的 Thread.join() 方法。当线程 thread A 执行完，原来等待它的B线程又会从 WAITING 状态转换到 RUNNABLE。
3. 第三种场景，调用 LockSupport.park() 方法。调用 LockSupport.park() 方法，当前线程会阻塞，线程的状态会从 RUNNABLE 转换到 WAITING。

RUNNABLE（运行） 与 TIMED_WAITING 的状态转换

1. 调用带超时参数的 Thread.sleep(long millis) 方法；
2. 获得 synchronized 隐式锁的线程，调用带超时参数的 Object.wait(long timeout) 方法；
3. 调用带超时参数的 Thread.join(long millis) 方法；
4. 调用带超时参数的 LockSupport.parkNanos(Object blocker, long deadline) 方法；
5. 调用带超时参数的 LockSupport.parkUntil(long deadline) 方法。

从 NEW（新建） 到 RUNNABLE（运行）状态的状态转换

1. 创建出来的 Thread 对象就是 NEW 状态，而创建 Thread 对象主要有两种方法。
2. 一种是继承 Thread 对象，重写 run() 方法。另一种是实现 Runnable 接口，重写 run() 方法，并将该实现类作为创建 Thread 对象的参数。
3. NEW 状态的线程，不会被操作系统调度，因此不会执行。从 NEW 状态转换到 RUNNABLE 状态很简单，只要调用线程对象的 start() 方法就可以了。

### 2.4 线程销毁和死亡

线程执行完 run() 方法后，会自动转换到 TERMINATED 状态，当然如果执行 run() 方法的时候异常抛出，也会导致线程终止。

有时候需要强制中断 run() 方法的执行，例如 run() 方法访问一个很慢的网络，我们等不下去了，想终止怎么办呢？

Java 的 Thread 类里面倒是有个 stop() 方法，不过已经标记为 @Deprecated，所以不建议使用了。正确的姿势其实是调用 interrupt() 方法。

那 stop() 和 interrupt() 方法的主要区别是什么呢？

1. stop() 方法会真的杀死线程，不给线程喘息的机会，如果线程持有 ReentrantLock 锁，被 stop() 的线程并不会自动调用 unlock() 去释放锁，那其他线程就再也没机会获得 ReentrantLock 锁，这实在是太危险了。
2. interrupt() 方法仅仅是通知线程，线程有机会执行一些后续操作，同时也可以无视这个通知。被 interrupt 的线程，是怎么收到通知的呢？一种是异常，另一种是主动检测。

### 2.5 影响生命周期因素

从线程生命周期的状态开始展开，那么在Java编程中，有哪些因素可能影响线程的状态呢？主要有：

线程自身的方法，除了start，还有多个join方法，等待线程结束；yield是告诉调度器，主动让出CPU；另外，就是一些已经被标记为过时的resume、stop、suspend之类，据我所知，在JDK最新版本中，destroy/stop方法将被直接移除。

基类Object提供了一些基础的wait/notify/notifyAll方法。如果我们持有某个对象的Monitor锁，调用wait会让当前线程处于等待状态，直到其他线程notify或者notifyAll。所以，本质上是提供了Monitor的获取和释放的能力，是基本的线程间通信方式。

并发类库中的工具，比如CountDownLatch.await()会让当前线程进入等待状态，直到latch被基数为0，这可以看作是线程间通信的Signal。

这里画了一个状态和方法之间的对应图：

Thread和Object的方法，听起来简单，但是实际应用中被证明非常晦涩、易错，这也是为什么Java后来又引入了并发包。总的来说，有了并发包，大多数情况下，我们已经不再需要去调用wait/notify之类的方法了。

## 03.线程核心Api设计

### 3.0 线程简单使用api

在具体实现中，线程还分为内核线程、用户线程，Java的线程实现其实是与虚拟机相关的。对于我们最熟悉的Sun/Oracle JDK，其线程也经历了一个演进过程，基本上在Java 1.2之后，JDK已经抛弃了所谓的Green Thread，也就是用户调度的线程，现在的模型是一对一映射到操作系统内核线程。

如果我们来看Thread的源码，你会发现其基本操作逻辑大都是以JNI形式调用的本地代码。

```java
private native void start0();
private native void setPriority0(int newPriority);
private native void interrupt0();
```

这种实现有利有弊，总体上来说，Java语言得益于精细粒度的线程和相关的并发操作，其构建高扩展性的大型应用的能力已经毋庸置疑。但是，其复杂性也提高了并发编程的门槛，近几年的Go语言等提供了协程（coroutine），大大提高了构建并发应用的效率。于此同时，Java也在Loom项目中，孕育新的类似轻量级用户线程（Fiber）等机制，也许在不久的将来就可以在新版JDK中使用到它。

下面，我来分析下线程的基本操作。如何创建线程想必你已经非常熟悉了，请看下面的例子：

```java
Runnable task = () -> {System.out.println("Hello World!");};
Thread myThread = new Thread(task);
myThread.start();
myThread.join();
```

我们可以直接扩展Thread类，然后实例化。但在本例中，我选取了另外一种方式，就是实现一个Runnable，将代码逻放在Runnable中，然后构建Thread并启动（start），等待结束（join）。

Runnable的好处是，不会受Java不支持类多继承的限制，重用代码实现，当我们需要重复执行相应逻辑时优点明显。而且，也能更好的与现代Java并发库中的Executor之类框架结合使用，比如将上面start和join的逻辑完全写成下面的结构：

```java
Future future = Executors.newFixedThreadPool(1)
  .submit(task)
  .get();
```
这样我们就不用操心线程的创建和管理，也能利用Future等机制更好地处理执行结果。线程生命周期通常和业务之间没有本质联系，混淆实现需求和业务需求，就会降低开发的效率。

### 3.1 start方法设计原理

为什么要设计start方法，而不是创建对象即启动线程？出于那些考虑：

1. 线程初始化：在创建线程对象时，可能需要进行一些初始化操作，例如设置线程的名称、优先级、绑定特定的处理器等。通过将线程的启动操作放在start()方法中，可以确保在线程初始化完成后再启动线程。
2. 线程状态管理：通过使用start()方法，可以在适当的时机将线程状态从新建状态转换为就绪状态，然后由线程调度器决定何时运行该线程。
3. 安全性考虑：直接在创建线程对象时启动线程可能会导致一些安全性问题。例如，如果在构造函数中启动线程，可能会导致线程在对象完全初始化之前就开始执行，从而可能访问到未初始化的数据。

问你的问题

一个线程两次调用start()方法会出现什么情况？谈谈线程的生命周期和状态转移。

典型回答

Java的线程是不允许启动两次的，第二次调用必然会抛出IllegalThreadStateException，这是一种运行时异常，多次调用start被认为是编程错误。

在第二次调用start()方法的时候，线程可能处于终止或者其他（非NEW）状态，但是不论如何，都是不可以再次启动的。

### 3.2 sleep方法设计原理

为什么要设计线程sleep方法

1. 因为线程是抢占式的，为了让线程可以调度。设计sleep()方法通过告知线程调度器，当前线程暂时不需要执行，从而让其他线程有机会执行。
2. 有场景需要指定了当前线程暂停执行的时间长度。设计sleep()方法即可设置JVM线程休眠时间
3. 模拟一些时间敏感的操作，如定时任务等。设计sleep()方法，可以控制操作的时间间隔，从而实现一定的时间延迟效果。

Thread.Sleep(0) 。既然是 Sleep 0 毫秒，那么他跟去掉这句代码相比，有啥区别么？

答案是：有，而且区别很明显。Thread.Sleep(0)的作用，就是“触发操作系统立刻重新进行一次CPU竞争”。

为了让某些优先级比较低的线程也能获取到CPU控制权，可以使用Thread.sleep(0)手动触发一次操作系统分配时间片的操作，这也是平衡CPU控制权的一种操作。

竞争的结果也许是当前线程仍然获得CPU控制权，也许会换成别的线程获得CPU控制权。


### 3.3 join方法设计原理

Java thread 为何要设计join方法，出于什么考虑？

设计join()方法是为了实现线程之间的协作和同步。这种设计可以确保线程的执行顺序和协作，避免并发问题和数据竞争，提高多线程程序的可靠性和稳定性。

join()有什么作用？Thread的join()的含义是等待该线程终止，即将挂起调用线程的执行，直到被调用的对象完成它的执行。比如存在两个线程t1和t2，下述代码表示先启动t1，直到t1的任务结束，才轮到t2启动。

```
t1.start();
t1.join();
t2.start();
```

join方法实现原理，join方法是通过调用线程的wait方法来达到同步的目的的。

例如A线程中调用了B线程的join方法，则相当于在A线程中调用了B线程的wait方法，当B线程执行完，B线程会自动调用自身的notifyAll方法唤醒A线程，从而达到同步的目的。

### 3.4 yield方法设计原理

Java thread 为何要设计yield方法，出于什么考虑？

设计yield()方法是为了实现线程的让步。提高多线程程序的公平性和协作性。

线程礼让的原理是是什么

暂定当前的线程，让CPU去执行其他的线程,这个暂定的时间是相当短暂的;当我某一个线程暂定完毕以后,其他的线程还没有抢占到cpu的执行权;那么这个是时候当前的线程会和其他的线程再次抢占cpu的执行权;

yield礼让线程会释放锁吗

yield()方法和sleep()方法类似，也不会释放“锁标志”，区别在于，它没有参数，即yield()方法只是使当前线程重新回到可执行状态，所以执行yield()的线程有可能在进入到可执行状态后马上又被执行，另外yield()方法只能使同优先级或者高优先级的线程得到执行机会，这也和sleep()方法不同。

什么是线程优先级了？下面就来具体聊一聊。

现代操作系统基本采用时分的形式调度运行的线程，操作系统会分出一个个时间片，线程会分配到若干时间片，当前时间片用完后就会发生线程调度，并等待这下次分配。线程分配到的时间多少也就决定了线程使用处理器资源的多少，而线程优先级就是决定线程需要或多或少分配一些处理器资源的线程属性。

在Java程序中，通过一个整型成员变量Priority来控制优先级，优先级的范围从1~10.在构建线程的时候可以通过**setPriority(int)**方法进行设置，默认优先级为5，优先级高的线程相较于优先级低的线程优先获得处理器时间片。需要注意的是在不同JVM以及操作系统上，线程规划存在差异，有些操作系统甚至会忽略线程优先级的设定。

另外需要注意的是，sleep()和yield()方法，同样都是当前线程会交出处理器资源，而它们不同的是，sleep()交出来的时间片其他线程都可以去竞争，也就是说都有机会获得当前线程让出的时间片。而yield()方法只允许与当前线程具有相同优先级的线程能够获得释放出来的CPU时间片。

### 3.5 stop方法设计原理

Java thread 为何要设计stop方法，出于什么考虑？

stop()方法是用于停止线程的方法。然而，需要注意的是，stop()方法已被标记为过时（deprecated），不推荐使用。

这是因为stop()方法存在一些潜在的问题和风险，导致其设计不符合安全性和可靠性的要求。

为什么stop设计不符合安全性和可靠性要求：

安全性问题：stop()方法会立即终止线程的执行，可能导致线程在不可预测的状态下停止。这可能会导致资源泄漏、数据不一致或其他潜在的问题。

数据一致性：线程在执行过程中可能会修改共享的数据结构或对象状态。如果线程被突然终止，可能会导致数据结构处于不一致的状态，从而引发错误或异常。

锁的释放：线程在执行过程中可能持有某些锁，如果线程被强制终止，可能会导致锁无法释放，从而导致其他线程无法访问共享资源，引发死锁等问题。

可控性和可靠性：stop()方法的使用会导致线程的突然终止，无法进行清理和善后工作。

### 3.6 interrupt方法设计

Java thread 为何要设计interrupt方法，出于什么考虑？

设计interrupt()方法是为了实现线程的中断，提供一种优雅的方式来处理线程的中断请求。提高多线程程序的可控性和可靠性。

interrupt()是如何设计停止线程的操作？

interrupt()方法是用于停止线程的操作。并不能直接停止线程的执行。它只是向线程发出一个中断请求，线程可以根据自身的逻辑来决定如何响应中断请求。

有两种相应中断请求方式：一种是异常，另一种是主动检测。

1. 异常处理中断请求：interrupt()方法会触发InterruptedException异常，以响应线程的中断请求。当线程处于阻塞状态（如调用了sleep()、wait()、join()等方法）时，如果收到中断请求，会立即抛出InterruptedException异常，提前结束阻塞状态。
2. 主动检测处理中断请求：通过调用Thread.interrupted()方法，可以获取当前线程的中断状态，并根据需要进行相应的处理。


## 04.线程使用实践

### 4.1 线程死锁怎么办

什么场景下会出现死锁

线程死锁是指两个或多个线程相互等待对方释放资源而无法继续执行的情况。

如果发生了线程死锁，可以采取以下几种方式来解决：

分析和定位死锁：需要分析和定位导致死锁的原因和位置。可以使用工具来检测死锁，如Java自带的jstack命令或使用专业的性能分析工具。通过分析死锁的堆栈信息，可以确定哪些线程和资源参与了死锁。

解除死锁：如果能够确定死锁的原因和位置，可以解除死锁。一种常见的方法是通过打破循环等待条件来解除死锁。

避免嵌套锁：尽量避免在持有一个锁的情况下去请求另一个锁，这样容易导致死锁。如果确实需要多个锁，可以尝试按照固定的顺序获取锁，避免循环等待。

优化代码逻辑：对于可能导致死锁的代码逻辑，可以进行优化和重构，避免出现死锁的情况。例如，减少锁的粒度、减少资源竞争、使用更合适的并发数据结构等。

### 4.2 线程异常如何处理

在Java中，线程中的异常是不能抛出到调用该线程的外部方法中捕获的。为什么不能抛出到外部线程捕获？

JVM的这种设计源自于这样一种理念：因为线程是独立执行的代码片断，线程的问题应该由线程自己来解决，而不要委托到外部。

”基于这样的设计理念，在Java中，线程方法的异常都应该在线程代码边界之内（run方法内）进行try catch并处理掉。换句话说，我们不能捕获从线程中逃逸的异常。

基于这样的设计理念，在Java中，线程方法的异常（无论是checked还是unchecked exception），都应该在线程代码边界之内（run方法内）进行try catch并处理掉。

Java中如何捕获线程异常

1. 第一种：使用try-catch块：可以在线程的run()方法内部使用try-catch块来捕获异常。
2. 第二种：可以为线程设置一个未捕获异常处理器（UncaughtExceptionHandler）。通过实现UncaughtExceptionHandler接口，并将其设置给线程，可以在线程抛出未捕获的异常时进行处理。
3. 第三种：如果使用ExecutorService来管理线程池，可以通过submit()方法提交任务，并使用Future对象来捕获线程执行过程中的异常。

```java
Thread thread = new Thread(() -> {
    // 线程执行逻辑
});
thread.setUncaughtExceptionHandler((t, e) -> {
    // 异常处理逻辑
});
thread.start();
```

### 4.3 如何确保线程安全

首先思考一下为何会出现线程不安全问题

线程不安全问题的主要原因是多个线程同时访问共享的可变状态或资源，而没有适当的同步机制来保证线程之间的互斥访问和正确的操作顺序。

可以采取以下几种方式来确保线程安全

1. 使用同步（synchronization）：通过使用synchronized关键字或使用Lock接口及其实现类，可以在多个线程之间实现互斥访问共享资源。
2. 使用原子类（Atomic classes）：如AtomicInteger提供了原子性操作，可以确保对共享变量的操作是线程安全的。原子类的操作是基于底层的CAS（Compare and Swap）机制实现的，可以避免竞态条件和数据不一致的问题。
3. 使用volatile关键字：对于某些特定的场景，可以使用volatile关键字来确保共享变量的可见性和有序性，避免线程之间的数据不一致问题。
4. 避免共享可变状态：尽量避免多个线程共享可变状态，尽量将可变状态限制在单个线程内部。


### 4.4 线程睡眠和唤醒

怎么设置线程睡眠

1. 第一种方式：sleep()方法，来自Thread类静态方法。
2. 第二种方式：wait()方法，来自Object类实例方法。

怎么唤醒一个阻塞的线程

1. 如果线程是因为调用了wait()、sleep()或者join()方法而导致的阻塞，可以中断线程，并且通过抛出InterruptedException来唤醒它；
2. 如果线程遇到了IO阻塞，无能为力，因为IO是操作系统实现的，Java代码并没有办法直接接触到操作系统。

假设现在是 2018-4-7 12:00:00.000，如果我调用一下 Thread.Sleep(1000) ，在 2018-4-7 12:00:01.000 的时候，这个线程会不会被唤醒？

答案是：不一定。因为你只是告诉操作系统：在未来的1000毫秒内我不想再参与到CPU竞争。那么1000毫秒过去之后，这时候也许另外一个线程正在使用CPU，那么这时候操作系统是不会重新分配CPU的，直到那个线程挂起或结束；况且，即使这个时候恰巧轮到操作系统进行CPU 分配，那么当前线程也不一定就是总优先级最高的那个，CPU还是可能被其他线程抢占去。

与此相似的，Thread有个Resume函数，是用来唤醒挂起的线程的。好像上面所说的一样，这个函数只是“告诉操作系统我从现在起开始参与CPU竞争了”，这个函数的调用并不能马上使得这个线程获得CPU控制权。

wait和sleep方法的区别，最大的不同是在等待时wait会释放锁，而sleep一直持有锁。Wait通常被用于线程间交互，sleep通常被用于暂停执行。

wait()和sleep()其他区别：

1. sleep()方法调用的过程中，线程不会释放对象锁。而 调用 wait 方法线程会释放对象锁
2. sleep()睡眠后不出让系统资源，wait让出系统资源其他线程可以占用CPU
3. wait()方法必须要在同步方法或者同步块中调用，也就是必须已经获得对象锁。而sleep()方法没有这个限制可以在任何地方种使用。另外，wait()方法会释放占有的对象锁，使得该线程进入等待池中，等待下一次获取资源。而sleep()方法只是会让出CPU并不会释放掉对象锁；
4. sleep()方法在休眠时间达到后如果再次获得CPU时间片就会继续执行，而wait()方法必须等待Object.notify/Object.notifyAll通知后，才会离开等待池，并且再次获得CPU时间片才会继续执行。


### 4.5 设置线程优先级

Java thread 为何要设计线程优先级，出于什么考虑？

设计线程优先级，可以实现线程调度和资源分配的灵活性和可控性，提高线程的执行优先级和响应性，以及优化系统的整体性能和效率。

这种设计可以帮助开发者更好地管理和控制多线程应用程序的执行顺序和资源分配。

线程优先级的设计考虑了以下几个方面

1. 线程调度：线程优先级可以影响线程在竞争CPU资源时的调度顺序。具有较高优先级的线程在竞争CPU资源时更有可能被调度执行。
2. 资源分配：线程优先级可以影响线程在竞争其他系统资源时的分配顺序。
3. 平衡性：通过合理设置线程的优先级，可以使得不同类型的任务或线程能够得到适当的资源分配，从而提高系统的整体性能和效率。
4. 可控性：通过设置线程的优先级，开发者可以根据任务的重要性和紧急程度来调整线程的执行顺序，以满足特定的需求和优化性能。

设置线程的优先级，需要使用如下方法来设置

```
public final void setPriority(int newPriority)
```

参数值 newPriority 指定了线程的优先级，取值必须在 MIN_PRIORITY 和 MAX_PRIORITY 之间，默认取值是 NORM_PRIORITY

使用方法如下所示，建立三个线程，分别设置不同的优先级

```java
public class MyThread extends Thread {

	private int index;

	public MyThread(int index) {
		this.index = index;
	}

	@Override
	public void run() {
		for (int i = 0; i < 5; i++) {
			System.out.println("Index: " + index + "-----" + i);
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}

}

public class Test {

	public static void main(String[] args) {
		Thread thread1=new MyThread(1);
		Thread thread2=new MyThread(2);
		Thread thread3=new MyThread(3);
		thread1.setPriority(Thread.MIN_PRIORITY);
		thread2.setPriority(Thread.NORM_PRIORITY);
		thread3.setPriority(Thread.MAX_PRIORITY);
		thread1.start();
		thread2.start();
		thread3.start();
	}

}
```

输出结果为：

```text
Index: 2-----0
Index: 1-----0
Index: 3-----0
Index: 3-----1
Index: 1-----1
Index: 2-----1
Index: 3-----2
Index: 1-----2
Index: 2-----2
Index: 3-----3
Index: 1-----3
Index: 2-----3
Index: 3-----4
Index: 2-----4
Index: 1-----4
```

### 4.6 尽量设置线程名称


## 05.核心原理探索&分析

### 5.1 创建线程核心原理

线程创建和启动流程总结：

使用 new Thread() 定义一个线程对象，然后调用 start() 方法进行 Java 层面的线程启动

调用本地方法 start0()，然后会去调用JVM中的JVM_StartThread方法进行线程创建和启动

调用 new JavaThread() 会进入内核模式，由底层操作系统进行线程创建

底层操作系统新创建的线程为Initialized装态(初始化)，调用 sync -> wait() 方法进行等待，直到被唤醒才会执行 thread -> run()；

回到JVM层，会将Java中的Thread和JVM中的Thread进行绑定

调用Thread::start() 方法进行线程启动，并将线程状态设置成RUNNABLE，接着调用OS::start_thread() 根据不同的操作系统选择不同的线程启动方式

线程启动后状态设置成RUNNABLE，唤醒等待线程，执行 thread -> run() 方法

JavaThread::run() 方法会回调 new Thread 中复写的 run() 方法

所以，Java创建线程会涉及到内核模式的调用，非常消耗CPU资源，应该尽量让Java线程能够得到复用，减少不必要的重复创建。


### 5.2 线程映射核心原理

Java线程属于内核级线程，JDK1.2 基于操作系统原生线程模型来实现。

Sun JDK，它的Windows版本和Linux版本都使用一对一的线程模型实现，一条Java线程就映射到一条轻量级进程之中。

内核级线程（Kernel Level Thread ，KLT）：它们是依赖于内核的，即无论是用户进程中的线 程，还是系统进程中的线程，它们的创建、撤消、切换都由内核实现。

用户级线程（User Level Thread，ULT）：操作系统内核不知道应用线程的存在。


## 06.线程问题排查&优化

### 6.1 如何优化线程性能

线程性能可以通过合理的线程调度、减少线程上下文切换、避免过度同步等方式进行优化。使用线程池和并发集合也可以提高性能。

### 6.2 线程问题诊断分析

多线程程序很难调试，出了 Bug 基本上都是靠日志，靠线程 dump 来跟踪问题

分析线程 dump 的一个基本功就是分析线程状态，大部分的死锁、饥饿、活锁问题都需要跟踪分析线程的状态。

你可以通过 jstack 命令或者Java VisualVM这个可视化工具将 JVM 所有的线程栈信息导出来，完整的线程栈信息不仅包括线程的当前状态、调用栈，还包括了锁的信息。








