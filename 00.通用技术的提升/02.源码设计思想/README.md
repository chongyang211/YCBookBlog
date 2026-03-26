




---

### 3. BlockingQueue — 封装好的生产者-消费者

**原理**：内部用 `ReentrantLock` + 两个 `Condition` 实现。`put()` 满时阻塞，`take()` 空时阻塞。开发者无需手动管理锁。

```java
BlockingQueue<Integer> queue = new ArrayBlockingQueue<>(5);

// 生产者
new Thread(() -> {
  for (int i = 0; i < 10; i++) {
    queue.put(i);                    // 满了自动阻塞
    System.out.println("Produced: " + i);
  }
}).start();

// 消费者
new Thread(() -> {
  for (int i = 0; i < 10; i++) {
    int val = queue.take();          // 空了自动阻塞
    System.out.println("Consumed: " + val);
  }
}).start();
```

**底层**：`ArrayBlockingQueue` = 数组 + `ReentrantLock` + `notEmpty Condition` + `notFull Condition`。

---

### 4. CountDownLatch — 等待 N 个线程完成

**原理**：内部维护一个 `int` 计数器（基于 AQS），每次 `countDown()` 减 1，`await()` 阻塞直到计数器归零。**一次性**，归零后不可重置。

```java
CountDownLatch latch = new CountDownLatch(3);

for (int i = 0; i < 3; i++) {
  int id = i;
  new Thread(() -> {
    System.out.println("Task " + id + " done");
    latch.countDown();               // 完成一个，计数减1
  }).start();
}

latch.await();                       // 主线程阻塞，直到3个任务全完成
System.out.println("All tasks completed");
```

---

### 5. CyclicBarrier — 多线程在某个点同步

**原理**：基于 `ReentrantLock` + `Condition`，到达屏障的线程阻塞等待，最后一个到达时全部唤醒。**可重用**（cyclic）。

```java
CyclicBarrier barrier = new CyclicBarrier(3, () -> {
  System.out.println("All threads arrived, proceed!");  // 最后一个到达时执行
});

for (int i = 0; i < 3; i++) {
  int id = i;
  new Thread(() -> {
    System.out.println("Thread " + id + " reached barrier");
    barrier.await();                 // 阻塞，直到3个线程都到
    System.out.println("Thread " + id + " continues");
  }).start();
}
```

**vs CountDownLatch**：CountDownLatch 一次性，CyclicBarrier 可重复使用。

---

### 6. Semaphore — 控制并发数

**原理**：基于 AQS，维护许可数。`acquire()` 获取许可（无许可则阻塞），`release()` 归还许可。底层是 CAS 操作。

```java
Semaphore semaphore = new Semaphore(2);  // 最多2个线程同时执行

for (int i = 0; i < 5; i++) {
  int id = i;
  new Thread(() -> {
    semaphore.acquire();             // 获取许可，超过2个则阻塞
    System.out.println("Thread " + id + " working");
    Thread.sleep(1000);
    semaphore.release();             // 归还许可
  }).start();
}
```

---

### 7. Future / CompletableFuture — 异步结果通信

**原理**：`Future` 内部用 AQS 状态机（NEW → COMPLETING → NORMAL），`get()` 阻塞等待状态变为完成。`CompletableFuture` 支持链式回调。

```java
// Future：阻塞获取结果
ExecutorService pool = Executors.newFixedThreadPool(2);
Future<Integer> future = pool.submit(() -> {
  Thread.sleep(1000);
  return 42;
});
int result = future.get();           // 阻塞直到结果就绪

// CompletableFuture：非阻塞链式
CompletableFuture.supplyAsync(() -> fetchData())
  .thenApply(data -> process(data))  // 上一步完成后自动执行
  .thenAccept(result -> save(result));
```

---

### 8. Exchanger — 两个线程交换数据

**原理**：基于 CAS + park/unpark。第一个到达的线程阻塞，第二个到达时两者交换数据后同时继续。

```java
Exchanger<String> exchanger = new Exchanger<>();

new Thread(() -> {
  String data = exchanger.exchange("from-A");  // 发出"from-A"，收到B的数据
  System.out.println("A received: " + data);
}).start();

new Thread(() -> {
  String data = exchanger.exchange("from-B");  // 发出"from-B"，收到A的数据
  System.out.println("B received: " + data);
}).start();
// 输出: A received: from-B, B received: from-A
```

---

### 9. Phaser — 分阶段同步（CyclicBarrier 增强版）

**原理**：基于 64 位 state（高32位=phase，低位=参与者数），支持动态注册/注销参与者，可分多阶段执行。

```java
Phaser phaser = new Phaser(3);       // 3个参与者

for (int i = 0; i < 3; i++) {
  int id = i;
  new Thread(() -> {
    System.out.println("Thread " + id + " phase-0 done");
    phaser.arriveAndAwaitAdvance();   // 等所有人完成第0阶段

    System.out.println("Thread " + id + " phase-1 done");
    phaser.arriveAndAwaitAdvance();   // 等所有人完成第1阶段

    phaser.arriveAndDeregister();     // 退出
  }).start();
}
```

---

### 10. volatile — 可见性通信

**原理**：JMM 保证 volatile 写时插入 **StoreLoad 内存屏障**，读时插入 **LoadLoad 屏障**，确保一个线程的写入对其他线程立即可见。不保证原子性。

```java
class Flag {
  private volatile boolean running = true;

  // 线程A
  public void stop() { running = false; }    // 写入立即对B可见

  // 线程B
  public void work() {
    while (running) {              // 每次读都从主存获取最新值
      // do work
    }
  }
}
```

---

### 11. LockSupport.park() / unpark() — 最底层的线程阻塞

**原理**：直接调用 `Unsafe.park/unpark`，底层是 OS 的 `pthread_cond_wait/signal`。AQS、CountDownLatch、Semaphore 等都基于它实现。

```java
Thread worker = new Thread(() -> {
  System.out.println("Worker parking...");
  LockSupport.park();              // 阻塞当前线程
  System.out.println("Worker unparked!");
});
worker.start();

Thread.sleep(1000);
LockSupport.unpark(worker);        // 唤醒指定线程（可以先unpark再park）
```

**vs wait/notify**：不需要持有锁，且 `unpark` 可以在 `park` 之前调用（许可机制）。

---

### 12. Thread.join() — 等待线程结束

**原理**：内部调用 `wait()`，目标线程结束时 JVM 自动调用 `notifyAll()` 唤醒所有等待者。

```java
Thread t = new Thread(() -> {
  Thread.sleep(1000);
  System.out.println("Task done");
});
t.start();
t.join();                          // 主线程阻塞，直到t结束
System.out.println("Continue after t");
```

---

### 全景总结

| 机制 | 底层原理 | 适用场景 |
|------|---------|---------|
| wait/notify | Monitor（OS mutex + cond） | 基础锁通信 |
| Condition | ReentrantLock + AQS | 多条件精细控制 |
| BlockingQueue | Lock + 2个Condition | 生产者-消费者 |
| CountDownLatch | AQS（共享模式） | 等N个任务完成 |
| CyclicBarrier | Lock + Condition | 多线程汇合点（可重用） |
| Semaphore | AQS（共享模式）+ CAS | 限流/资源池 |
| Future/CompletableFuture | AQS 状态机 / Treiber Stack | 异步结果 |
| Exchanger | CAS + park/unpark | 两线程数据交换 |
| Phaser | 64位state + CAS | 分阶段同步 |
| volatile | 内存屏障 | 状态标志可见性 |
| LockSupport | Unsafe.park（pthread_cond） | 最底层阻塞/唤醒 |
| Thread.join | wait/notifyAll | 等待线程结束 |

**依赖链**：

```
应用层:  BlockingQueue / CountDownLatch / Semaphore / CyclicBarrier
           │
           ▼
框架层:  AQS (AbstractQueuedSynchronizer)
           │
           ▼
基础层:  LockSupport.park/unpark → Unsafe
           │
           ▼
OS层:    pthread_mutex + pthread_cond (futex on Linux)
           │
           ▼
硬件层:  CAS指令 + 内存屏障 (MFENCE/LFENCE)
```