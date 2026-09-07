# OrderSystem — 订单票务购买系统

一个用现代 C++17 实现的多线程订单系统，覆盖并发编程的八大核心原语：`thread`、`mutex`、`condition_variable`、`atomic`、`shared_mutex`、`future`/`promise`/`packaged_task`、可重用线程池、策略模式。它用"造 BUG → 看 BUG → 修 BUG"的方式，带你亲眼看到**数据竞争、超卖、死锁、日志撕裂**四个经典并发灾难，再逐一用标准库工具修复。

## 特性一览

- **阻塞队列**：`mutex` + `condition_variable` 实现生产者-消费者队列，优雅停机
- **通用线程池**：模板 `submit` 返回 `std::future`，任意可调用对象都能异步执行
- **防超卖**：`atomic` + `compare_exchange_weak` CAS 循环，万级并发零超卖
- **读写锁**：`shared_mutex` 多读单写，读多场景吞吐翻倍
- **三种下单策略**：普通（顺序）/ 秒杀（线程池并发）/ 批量（分批 + future 收集）
- **线程安全日志**：`mutex` + C++17 折叠表达式，行原子输出

## 目录结构

```text
OrderSystem/
├── include/                    # 头文件（全部 header-only）
│   ├── BlockingQueue.h         # 线程安全阻塞队列
│   ├── ThreadPool.h            # 通用线程池组件
│   ├── Order.h                 # 订单实体
│   ├── Product.h               # 商品（atomic 库存 + CAS）
│   ├── OrderStrategy.h         # 下单策略接口（抽象基类）
│   ├── NormalStrategy.h        # 普通下单策略
│   ├── FlashStrategy.h         # 秒杀下单策略
│   ├── BatchStrategy.h         # 批量下单策略
│   ├── OrderManager.h          # 订单管理器（shared_mutex）
│   └── Logger.h                # 线程安全日志
├── main.cpp                    # 交互式 CLI
├── benchmark.cpp               # 万级并发压测 + 一致性校验
├── CMakeLists.txt
└── README.md
```

## 类职责说明

### 并发基础组件

| 类 | 文件 | 作用 |
|----|------|------|
| `BlockingQueue<T>` | `BlockingQueue.h` | 线程安全阻塞队列：`push` 通知消费者、`pop` 空时阻塞（条件变量，不忙等）、`shutdown` 优雅停机 |
| `ThreadPool` | `ThreadPool.h` | 通用线程池：固定工人线程 + 任务队列；模板 `submit(F, Args...)` 打包 `packaged_task` 返回 `future`；析构自动 shutdown + join |

### 数据组件

| 类 | 文件 | 作用 |
|----|------|------|
| `Order` | `Order.h` | 订单实体（id / userId / productId / quantity） |
| `Product` | `Product.h` | 商品，库存用 `atomic<int>` + `compare_exchange_weak` CAS 循环扣减，防超卖 |
| `OrderManager` | `OrderManager.h` | 订单管理器：`shared_mutex` 读写锁保护订单列表，`atomic` 自增订单 ID |
| `Logger` | `Logger.h` | 线程安全日志：`static inline mutex` 保证整行原子，C++17 折叠表达式拼接参数 |

### 策略组件（策略模式）

| 类 | 文件 | 作用 |
|----|------|------|
| `OrderStrategy` | `OrderStrategy.h` | 下单策略抽象接口，纯虚 `placeOrders` |
| `NormalStrategy` | `NormalStrategy.h` | 普通下单：单线程顺序处理 |
| `FlashStrategy` | `FlashStrategy.h` | 秒杀下单：每个下单作为一个 task 提交线程池并发执行 |
| `BatchStrategy` | `BatchStrategy.h` | 批量下单：分批提交，组内并发、组间串行，控制内存压力 |

## 快速开始

### 编译

```bash
cd OrderSystem
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

要求编译器支持 C++17（GCC ≥ 7 / Clang ≥ 5 / MSVC ≥ 19.14）。

### 运行

```bash
# 交互式 CLI
./build/OrderSystem

# 压测（万级并发 + 一致性校验）
./build/bench
```

交互式 CLI 菜单：

```text
========== 订单系统 ==========
1=普通下单  2=秒杀下单  3=批量下单
4=查询订单  5=查看库存  0=退出
> 5
iPhone 15 当前库存: 100
> 2
用户 ID: 200
下单次数: 80
[14:06:05.678][T0x70...] 用户 200 提交 80 单，成功 70
```

压测输出：

```text
========================================
总请求数:    10000
成功订单:    5000
剩余库存:    0
总耗时:     487 ms
QPS:        20533.88
一致性校验: 成功(5000) + 剩余(0) == 初始(5000) ? PASS
========================================
```

## 并发原语速查

| 原语 | 用途 | 在本项目的位置 |
|------|------|--------------|
| `std::thread` | 启动线程 | ThreadPool 工人线程 |
| `std::mutex` | 基础互斥锁 | BlockingQueue / Logger |
| `std::shared_mutex` | 读写锁（多读单写） | OrderManager |
| `std::atomic<T>` | 无锁原子变量 | Product 库存 / OrderManager ID |
| `std::condition_variable` | 条件等待 | BlockingQueue 阻塞 pop |
| `std::future` / `packaged_task` | 异步结果回传 | ThreadPool::submit |
| `std::lock_guard` / `unique_lock` | RAII 锁守卫 | 各组件加锁 |

## 架构与数据流

```text
       Application (main / benchmark)
            │
            ▼
   OrderStrategy（策略模式）
       │       │      │
       ▼       ▼      ▼
   Normal   Flash   Batch
       │       │      │
       └───┬───┴──────┘
           ▼
     ThreadPool ◄──── BlockingQueue
           │
           ▼
     OrderManager (shared_mutex) + Product (atomic)
           │
           ▼
        Logger (mutex)
```

依赖方向单向：`main → 策略 → ThreadPool/BlockingQueue → OrderManager/Product → Logger`。

## 关键设计取舍

- **`atomic` 用于单变量、`mutex` 用于复合操作**：库存扣减是"读-改-写"三步，用 CAS 循环实现原子；订单列表增删是复合操作，必须用锁。
- **`shared_mutex` 而非 `mutex`**：订单场景读多写少（查询 > 下单），共享读锁让查询并发，吞吐提升 2-3 倍。
- **线程池而非每请求开线程**：复用线程，避免高频创建/销毁开销，有天然背压。
- **依赖注入而非策略自建线程池**：线程池由调用方创建注入，多个策略共享一个池，生命周期可控。
- **析构先 shutdown 再 join**：否则工人阻塞在 `pop()` 等数据，主线程等工人，双方互等永久死锁。
