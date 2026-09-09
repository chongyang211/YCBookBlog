# OrderSystem —— C语言 订单票务购买系统

《C语言入门精通》综合案例 05 的配套源码：**pthread + C11 atomic** 实现的多线程订单系统——阻塞队列、手写 future、线程池、读写锁、CAS 防超卖、三种下单策略、线程安全日志，一次练遍 C 并发八大核心。

教程地址：`packages/website/11.CodeX/01.C语言入门精通/02.综合案例/05.订单票务购买系统.md`

## 编译运行

```bash
make            # 一键编译 orderapp + bench
./orderapp      # 交互式 CLI
./bench         # 万级并发压测 + 一致性校验
```

⚠️ `-pthread` 不能省，否则链接报 `undefined reference to pthread_create`。

## 一轮 CLI 会话

```text
> 4
iPhone 15 当前库存: 100

> 1          # 普通下单（单线程顺序）
用户 ID: 100
下单次数: 30
[..][T..] 用户 100 提交 30 单，成功 30

> 2          # 秒杀下单（线程池并发）
用户 ID: 200
下单次数: 80          ← 库存只剩 70
[..][T..] 用户 200 提交 80 单，成功 70

> 4
iPhone 15 当前库存: 0

> 3
用户 ID: 100
用户 100 共 30 单：
  订单 #1 - 商品 1 - 数量 1
  ...
```

## 压测验收（1000 用户线程 × 10 单，抢 5000 库存）

```text
总请求数:    10000
成功订单:    5000
剩余库存:    0
QPS:         20000+
一致性校验: 成功(5000) + 剩余(0) == 初始(5000) ? PASS
```

三条验收：**QPS 过万 / 零超卖（atomic CAS）/ 零漏单（rwlock）**。

## 模块结构（与教程章节对应）

| 文件 | 教程阶段 | 职责 |
|---|---|---|
| `src/blocking_queue.h/.c` | ① §02 | 链表 FIFO + mutex + cond + shutdown（生产者-消费者）|
| `src/future.h/.c` | ② §03 | **手写 mini future**：结果槽 + mutex + cond（C++ future 的裸机原理）⭐ |
| `src/thread_pool.h/.c` | ② §03 | 线程池：task = 函数指针 + void*，submit 返回 future |
| `src/order.h/.c` | ③ §04 | OrderManager：`pthread_rwlock` 读写锁 + `atomic_int` ID |
| `src/product.h/.c` | ④ §05 | `atomic_compare_exchange_weak` CAS 循环扣库存，防超卖 ⭐ |
| `src/strategy.h/.c` | ⑤ §06 | 三种下单策略（normal / flash / batch），函数指针表 |
| `src/logger.h/.c` | ⑥ §07 | vsnprintf 锁外格式化 + 锁内一次输出 = 行原子日志 |
| `main.c` | ⑦ §08 | 交互式 CLI |
| `benchmark.c` | ⑦ §08 | 万级并发压测 + 一致性校验 |

## 并发原语速查（教程 §9.2）

| 原语 | 用途 | 本项目位置 |
|---|---|---|
| `pthread_mutex_t` | 互斥锁 | 队列 / 日志 |
| `pthread_rwlock_t` | 多读单写 | 订单管理器 |
| `pthread_cond_t` | 条件等待 | 阻塞队列 / future（**wait 必须套 while**）|
| `atomic_int` | 无锁原子变量 | 库存 / 订单 ID / 计数 |
| `atomic_compare_exchange_weak` | CAS 循环 | 防超卖核心 |

选择决策树：单变量 → atomic；读多写少 → rwlock；跨线程回传结果 → 手写 future；复杂结构 → mutex。

## 已知局限（教程 §11.3 挑战题）

- 未用 C11 `<threads.h>` 重写（挑战 A）
- rwlock 不处理写者饥饿（挑战 B：写者优先）
- 未实现 SPSC 无锁环形队列（挑战 C：acquire/release 内存序）
- 不涉及 memory_order 细节（留给进阶卷）
