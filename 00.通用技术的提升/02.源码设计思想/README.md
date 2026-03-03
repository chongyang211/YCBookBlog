

## 理解CAS设计和由来

设计思想总结



核心原理总结

局限性剖析





### 案例3：（指令重排序）


## 四、核心原理：脏数据的三个根源

### 根源1：原子性问题（Atomicity）

> 一组操作要么全部执行完成，要么全部不执行。

CPU指令级别只保证单条指令的原子性。高级语言中一条语句通常编译成多条指令，中间随时可能被线程切换打断。

```
高级语言          CPU指令数
──────────────────────────
i++               3条 (LOAD/ADD/STORE)
i = j             可能2条 (64位变量在32位CPU上)
map[key] = val    几十条
```

### 根源2：可见性问题（Visibility）

> 一个线程对共享变量的修改，另一个线程能否立即看到。

现代CPU架构：

```
┌──────────┐    ┌──────────┐
│  CPU 0   │    │  CPU 1   │
│┌────────┐│    │┌────────┐│
││Register││    ││Register││
│└───┬────┘│    │└───┬────┘│
│┌───┴────┐│    │┌───┴────┐│
││ L1 Cache││    ││ L1 Cache││
│└───┬────┘│    │└───┬────┘│
│┌───┴────┐│    │┌───┴────┐│
││ L2 Cache││    ││ L2 Cache││
│└───┬────┘│    │└───┬────┘│
└────┼─────┘    └────┼─────┘
     └───────┬───────┘
        ┌────┴────┐
        │ L3 Cache│
        └────┬────┘
        ┌────┴────┐
        │Main Memory│
        └─────────┘
```

线程A在CPU0上修改了变量，值可能停留在L1 Cache中，CPU1上的线程B从自己的Cache读到的仍然是旧值。

```cpp
// C++ 示例
bool stop = false;

// 线程A
void Worker() {
  while (!stop) {  // 编译器可能优化为只读一次，永远循环
    DoWork();
  }
}

// 线程B
void RequestStop() {
  stop = true;     // 写入可能滞留在CPU Cache中
}
```

### 根源3：有序性问题（Ordering）

> 程序执行的顺序不一定是代码书写的顺序。

重排序发生在三个层面：
1. **编译器优化**：编译器为了性能调整指令顺序
2. **CPU指令流水线**：CPU乱序执行（Out-of-Order Execution）
3. **内存系统**：Store Buffer和Invalidate Queue导致写读顺序变化

```cpp
// 初始: x = 0, y = 0

// 线程A            // 线程B
x = 1;              y = 1;
r1 = y;             r2 = x;

// 理论上 r1=0 且 r2=0 不可能
// 但在弱内存序的CPU（如ARM）上确实可能发生
```

## 五、语言内存模型深入剖析

### Java内存模型（JMM）

Java从JDK5开始定义了严格的内存模型（JSR-133），核心概念是**happens-before**关系：

```
规则                          保证
─────────────────────────────────────────────
monitor lock                  unlock → lock 可见
volatile                      写 → 读 可见
Thread.start()                start前 → 新线程内 可见
Thread.join()                 线程内 → join后 可见
```

```java
// volatile 解决可见性+有序性（但不解决原子性）
private volatile boolean stop = false;

// synchronized 解决原子性+可见性+有序性（重量级）
synchronized (lock) {
    counter++;
}

// AtomicInteger 解决原子性（CAS，轻量级）
private AtomicInteger counter = new AtomicInteger(0);
counter.incrementAndGet();
```

**JMM的关键设计**：不保证所有操作的全局可见顺序，只保证有happens-before关系的操作之间的顺序。这给了JVM和CPU充分的优化空间。

### C++内存模型（C++11起）

C++11引入了更底层、更灵活的内存模型，提供六种内存序：

```cpp
enum memory_order {
  memory_order_relaxed,    // 无序，仅保证原子性
  memory_order_consume,    // 数据依赖序（很少用）
  memory_order_acquire,    // 获取语义：此后的读写不能重排到此之前
  memory_order_release,    // 释放语义：此前的读写不能重排到此之后
  memory_order_acq_rel,    // acquire + release
  memory_order_seq_cst     // 顺序一致性（默认，最强）
};
```

对比Java和C++的工具对应关系：

```
语义              Java                    C++
──────────────────────────────────────────────────────
原子操作          AtomicXxx               std::atomic<T>
互斥锁           synchronized/Lock        std::mutex + std::lock_guard
可见性屏障        volatile                 atomic + memory_order
条件等待          wait/notify              std::condition_variable
```

C++的典型安全写法：

```cpp
// 1. 互斥锁（最简单，但有开销）
std::mutex mtx;
int counter = 0;

void Increment() {
  std::lock_guard<std::mutex> lock(mtx);
  counter++;
}

// 2. 原子变量（无锁，高性能）
std::atomic<int> counter{0};

void Increment() {
  counter.fetch_add(1, std::memory_order_relaxed);
}

// 3. acquire-release 配对（精确控制）
std::atomic<bool> ready{false};
int data = 0;

// 线程A（生产者）
data = 42;
ready.store(true, std::memory_order_release);  // data=42 对获取方可见

// 线程B（消费者）
while (!ready.load(std::memory_order_acquire));  // 获取屏障
assert(data == 42);  // 保证成立
```

### OC/C的情况

**Objective-C**：依赖GCD的串行队列实现线程安全，`@synchronized`提供互斥，`atomic`属性仅保证属性读写的原子性（不保证业务逻辑原子性）。

```objectivec
// atomic只保证getter/setter不会读到撕裂值，不保证线程安全
@property (atomic, strong) NSMutableArray *list;

// 这仍然不安全：
if (self.list.count > 0) {         // check
    [self.list removeLastObject];   // act（可能此时已被另一线程清空）
}

// GCD串行队列是OC中最常用的并发安全方案
dispatch_queue_t queue = dispatch_queue_create("com.app.serial", DISPATCH_QUEUE_SERIAL);
dispatch_async(queue, ^{ /* 串行执行，天然线程安全 */ });
```

**C语言**：没有内置内存模型（C11之前），完全依赖平台特定的原语（`pthread_mutex`、编译器内建原子操作），是最容易出并发Bug的语言。

### 本项目中的实践

当前项目采用了一种简洁有效的并发模型——**单线程事件循环 + Post**：

```cpp
// 所有LVGL UI操作限定在主线程
Threads::MainThread()->Post([this, result]() {
  // 在主线程中安全执行，无需加锁
  if (current_page_ != PageType::kNetworkDetection) return;
  loading_widget_.Stop();
  ShowSuccessBackground();
});
```

这种模型的本质是**把并发问题转化为串行问题**——异步操作在后台线程执行，结果通过Post投递到主线程队列，所有UI状态的读写都在主线程，从根本上消除了竞态条件。这与Android的`Handler`、iOS的`dispatch_async(main_queue)`、JavaScript的事件循环是相同的思路。

## 总结

| 根源 | 问题 | Java方案 | C++方案 |
|------|------|---------|--------|
| 原子性 | 复合操作被打断 | synchronized / AtomicXxx | std::mutex / std::atomic |
| 可见性 | Cache不一致 | volatile / happens-before | memory_order_acquire/release |
| 有序性 | 指令重排序 | volatile / final | memory_order_seq_cst |

**最根本的原则**：如果能避免共享可变状态，就不需要任何同步机制。消息传递（Post/Channel）、不可变数据、线程局部存储，都是从源头消除并发Bug的方法。
