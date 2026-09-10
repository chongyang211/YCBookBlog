# OrderSystem —— Java 多线程订单系统与线程池

《Java入门精通》综合案例 05 的配套源码：**synchronized / ReentrantLock / volatile / Atomic / BlockingQueue / ExecutorService / CompletableFuture / ReadWriteLock 八件套全开**，含**自实现 BlockingQueue + 自实现线程池** 与 5 个造 BUG 现场演示，约 1700 行纯 JDK 代码（JDK 17）。

教程地址：`packages/website/11.CodeX/03.Java入门精通/02.综合案例/05.订单票务购买系统.md`

## 编译运行

```bash
make            # 编译到 out/
make run        # 端到端系统：CLI 生产者 + 双线程池 + QPS 监控
make demo-race      # 阶段②：竞态条件（ID 重复 + 丢数据）
make demo-deadlock  # 阶段④：死锁现场 + 两套修复
make demo-queue     # 阶段⑤：if vs while 虚假唤醒对比
make demo-pool      # 阶段⑥：自实现线程池（流转/异常不饿死/Future）
make demo-batch     # 阶段⑧：allOf 批量 + exceptionally
make demo-bench     # 阶段⑦：互斥锁 vs 读写锁 vs CHM 三方压测
```

## 一轮端到端会话（make run）

```text
命令格式：<productId> <qty>，例如 'P1 3'。输入 'quit' 退出，'auto' 启动压测
P1 3
[监控] QPS=1 已下单=1 库存P1=997 P2=1000 P3=1000
auto
[监控] QPS=347 已下单=350 库存P1=884 P2=883 P3=883
[监控] QPS=523 已下单=873 库存P1=710 P2=710 P3=710
quit
开始优雅关闭...
已退出。共下单 1004
```

## 模块结构（与教程阶段对应）

| 包 | 类 | 教程阶段 | 职责 |
|---|---|---|---|
| `entity` | `Order`（volatile status）/ `Product`（record）/ `OrderStatus` | ① §02 | 不可变值对象 |
| `service` | `OrderManager` | ①③ §02-04 | 双小锁 synchronized（私有 final 锁 + 细粒度块）|
| `service` | `Inventory` 接口 + `Sync`/`RWLock`/`Concurrent` 三实现 | ⑦ §08 | 三方性能对比（CHM+CAS 最优）|
| `service` | `Account`/`Bank` + `AccountLockable`/`BankLockable` | ④ §05 | 死锁 buggy / 固定锁序 / tryLock 三版 |
| `concurrent` | `MyBlockingQueueV1`（if 错）/ `MyBlockingQueue`（while）/ `MyBlockingQueueLock`（双 Condition）| ⑤ §06 | 自造阻塞队列三版本 |
| `concurrent` | `MyThreadPool` | ⑥ §07 | 七参数 + 4 拒绝 + Worker try-catch + 优雅关停 |
| `pool` | `RejectPolicy` | ⑥ §07 | 4 种拒绝策略（BiConsumer 策略模式）|
| `async` | `OrderPipeline` | ⑧ §09 | 四阶段流水线 + exceptionally |
| `cli` | `Main` + 6 个 Demo | ⑨ §10 | 端到端 + 分阶段演示 |

## 与教程原代码的差异（修复的 3 处问题）

1. **`MyThreadPool.shutdown()` 挂起 bug**：原版只置 `volatile` 标志不 interrupt——core worker 永久阻塞在 `take()` 上，`awaitTermination` 必超时，**JVM 进程无法退出**。修复：shutdown 时 interrupt 唤醒所有 worker；Worker 的 `InterruptedException` 处理改为"shutdown 且队列跑空才退出，否则清中断标志继续 drain"。
2. **`Main` 的关停标志无 volatile 语义**：原版 `boolean[] running` 数组元素不具备可见性保证。修复：改 `AtomicBoolean`。
3. **`Inventory` 接口缺失**：教程压测代码引用 `Inventory[]` 但未定义接口。补接口 + 三实现各自独立文件（教程原为同文件包级类）。

另将教程内联压测代码补全为 `InventoryBenchmark.runStress`（100 线程 × 10000 操作，读:写=100:1，剩余库存三方一致性校验）。

## 已知局限（教程 §11.3 挑战题）

- 单 JVM 内存版（分布式锁 / NIO / ForkJoin 不做）
- `discardOldest` 简化实现
- 虚拟线程（JDK 21）重写留作挑战 C
