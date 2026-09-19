# 通用周期任务调度器（PeriodicTaskScheduler）

零依赖（只用 C++17 标准库）的周期任务调度器：**一个基准 tick 驱动所有任务**。

## 解决什么问题

需要按不同周期执行一批任务：心跳上报、日志上传、配置拉取、证书刷新、指标采集…

朴素做法是"每个任务一个线程 + `sleep` 循环"，问题在于：

| 问题 | 说明 |
|---|---|
| 线程膨胀 | 线程数随任务数线性增长 |
| 惊群 | 所有任务容易在同一时刻集中触发，瞬时抢资源 |
| 无法统一管理 | 启停、改频率、看执行情况各自为政 |

## 核心思路：基准 tick + 倍频取模

只有一个心跳线程按固定间隔打拍（默认 1 秒），每个任务登记 `frequency_multiplier`，
用 tick 计数取模决定是否执行：

```
tick     1  2  3  4  5  6  7  8  9
×1       ✓  ✓  ✓  ✓  ✓  ✓  ✓  ✓  ✓    每拍都执行
×2          ✓     ✓     ✓     ✓       隔一拍执行
×3             ✓        ✓             （叠加 initial_offset=1 则整体后移一拍）
```

好处：

1. 线程数恒定（1 个心跳 + N 个工作线程），与任务数量无关；
2. 频率只能是 tick 的整数倍 → 所有任务共享同一时基，天然便于错峰；
3. 启停、改频率、查统计集中在一处。

## 线程模型

```
心跳线程（1 个）
  └─ 取模判定 + 优先级排序 ──► kSync  任务：就地执行（会占用心跳线程，必须 < 10ms）
                           └─► kAsync 任务：投递队列 ──► 工作线程（默认 2 个）
```

`Stop()` 先停心跳，再等工作线程把队列排空后才返回 —— **不丢已派发的任务**。

## 用法

```cpp
#include "scheduler/periodic_task_scheduler.h"

auto& s = sched::PeriodicTaskScheduler::Instance();
s.SetTickInterval(std::chrono::milliseconds(200));   // 默认 1000ms，测试可设小
s.SetWorkerCount(2);                                  // 异步工作线程数

// 注册：名字 / 倍频 / 函数 / 优先级 / 模式 / 错峰 / 是否时间敏感
s.RegisterTask("heartbeat", 1, [] { /* 每拍执行 */ },
               sched::TaskPriority::kCritical, sched::TaskExecutionMode::kSync);
s.RegisterTask("log_upload", 2, [] { /* 隔拍执行，异步跑 */ },
               sched::TaskPriority::kNormal, sched::TaskExecutionMode::kAsync,
               /*initial_offset=*/0, /*requires_time_synced=*/false);

s.Start();
...
s.Stop();
```

### 动态管理

```cpp
s.SetTaskEnabled("log_upload", false);   // 禁用（保留其它属性）
s.SetTaskMultiplier("log_upload", 5);    // 改频率（每 5 拍一次）
s.UnregisterTask("log_upload");          // 注销
auto st = s.GetStats("log_upload");      // 执行次数 / 失败次数 / 耗时
```

### 时间同步守卫（可注入 = 可测试）

```cpp
s.SetTimeSyncGuard([] { return IsSystemTimeSynced(); });
s.RegisterTask("cert_refresh", 60, RefreshCert,
               sched::TaskPriority::kCritical, sched::TaskExecutionMode::kAsync,
               0, /*requires_time_synced=*/true);   // 时间未同步时跳过本次
```

用注入替代"直接依赖系统/设备模块"：既解耦，又能在测试里模拟"时间未同步"。

## API 速查

| 方法 | 说明 |
|---|---|
| `RegisterTask(name, multiplier, fn, priority, mode, offset, need_time_sync)` | 注册/覆盖任务 |
| `UnregisterTask(name)` | 注销 |
| `SetTaskEnabled(name, bool)` | 启用/禁用（不改其它属性） |
| `SetTaskMultiplier(name, n)` | 动态改频率 |
| `SetTickInterval(ms)` / `SetWorkerCount(n)` | 时基与工作线程数（`Start()` 前） |
| `SetTimeSyncGuard(fn)` | 注入时间同步守卫 |
| `Start()` / `Stop()` / `IsRunning()` | 生命周期 |
| `GetStats(name)` | 执行次数 / 失败次数 / 总/最大/最小耗时 |
| `TaskNames()` / `tick_count()` | 任务清单 / 已发生 tick 数 |

## 设计取舍

| 取舍 | 选择与理由 |
|---|---|
| 单 tick 驱动 vs 每任务一定时器 | 单 tick：线程数恒定、时基统一、便于错峰；代价是频率只能是 tick 整数倍 |
| `kSync` / `kAsync` 两种模式 | 短任务就地跑省一次线程切换；阻塞任务必须异步，否则拖慢整条时基 |
| `shared_ptr<Task>` 而非裸指针 | 任务派发后被注销也不会悬垂（异步队列里仍持有引用） |
| `wait_for` 而非 `sleep_for` | `Stop()` 能立刻唤醒心跳线程，不必等满一个周期 |
| 守卫注入而非直接查设备 | 去掉外部依赖，且可测 |
| 任务异常只记失败 | 单个任务抛异常不会拖垮整个调度器 |

## ⚠️ 生命周期陷阱

任务 lambda 捕获的对象必须活到**任务被注销之后**。捕获函数局部变量引用是典型错误 ——
函数返回即悬垂，下次 `Start()` 就会踩空（表现为随机崩溃）。

```cpp
// ❌ 错误：n 是局部变量，函数返回后引用悬垂
std::atomic<int> n{0};
s.RegisterTask("job", 1, [&] { n++; });

// ✅ 正确：用 shared_ptr 持有状态，并在退出前注销
auto n = std::make_shared<std::atomic<int>>(0);
s.RegisterTask("job", 1, [n] { (*n)++; });
...
s.Stop();
s.UnregisterTask("job");
```

## 运行演示

```bash
cd code/Cpp/Demo
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
./build/demo scheduler    # 只跑调度器（4 个场景：倍频/动态管理/时间守卫/统计）
./build/demo              # 线程库 + 网络库 + 调度器全部
```
