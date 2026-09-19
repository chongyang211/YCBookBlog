// 通用周期任务调度器（Periodic Task Scheduler，零依赖）
//
// 【解决什么问题】
//   需要按不同周期执行一批任务：心跳上报、日志上传、配置拉取、证书刷新、指标采集…
//   朴素做法是"每个任务一个线程 + sleep 循环"，问题是：
//     · 线程数随任务数线性增长；
//     · 所有任务容易在同一时刻集中触发（惊群、瞬时资源竞争）；
//     · 启停 / 改频率 / 看执行情况都得各自为政，没有统一入口。
//
// 【核心思路：一个基准 tick 驱动所有任务】
//   只用一个心跳线程按固定间隔打拍（默认 1 秒）；每个任务登记 frequency_multiplier，
//   用 tick 计数取模决定是否执行：
//
//       tick    1  2  3  4  5  6  7  8  9
//       ×1      ✓  ✓  ✓  ✓  ✓  ✓  ✓  ✓  ✓   每拍都执行
//       ×2         ✓     ✓     ✓     ✓      隔一拍执行
//       ×3            ✓        ✓            （再叠加 initial_offset=1 则整体后移）
//
//   好处：① 线程数恒定（1 个心跳 + N 个工作线程），与任务数量无关；
//        ② 频率只能是 tick 的整数倍 → 所有任务共享同一时基，天然便于错峰；
//        ③ 启停、改频率、查统计都集中在一处。
//
// 【其它设计点】
//   · initial_offset   错峰：前 N 拍跳过，避免同频任务同一时刻抢资源
//   · TaskPriority     同一 tick 内按优先级升序执行（kCritical 最先）
//   · TaskExecutionMode
//       kSync   在心跳线程内执行 —— 只适合 < 10ms 的短任务，否则会拖慢整条时基
//       kAsync  投递到工作线程池 —— 网络 I/O、可能阻塞的任务必须用它
//   · TimeSyncGuard    可注入的时间守卫：返回 false 时跳过"时间敏感"任务。
//                      用注入替代"直接依赖系统/设备模块"，既解耦又可在测试里模拟
//   · 统计             每任务累计执行次数 / 失败次数 / 耗时，慢任务与失败任务一眼可见。
//                      任务抛异常只记失败，不会拖垮调度器
//
// 【线程模型】
//   心跳线程（1 个）
//     └─ 取模判定 + 优先级排序 ──► kSync  任务：就地执行（会占用心跳线程）
//                              └─► kAsync 任务：投递队列 ──► 工作线程（默认 2 个）
//   Stop() 会先停心跳，再等工作线程把队列排空后才返回（不丢已派发的任务）。
//
// 【用法】
//   auto& s = sched::PeriodicTaskScheduler::Instance();
//   s.RegisterTask("heartbeat", 1, [] { /* ... */ },
//                  sched::TaskPriority::kCritical, sched::TaskExecutionMode::kSync);
//   s.Start();
//   ... 运行 ...
//   s.Stop();
//   完整示例见 Demo/main.cpp：./build/demo scheduler
//
// ⚠️ 生命周期：任务 lambda 捕获的对象必须活到"任务被注销"之后。
//    捕获函数局部变量引用是典型错误——函数返回即悬垂，下次 Start() 就会踩空。
//    建议用 shared_ptr 持有状态，并在退出前 UnregisterTask。

#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace sched {

/** @brief 任务优先级：数值越小优先级越高 */
enum class TaskPriority : int {
  kCritical = 0,
  kHigh = 1,
  kNormal = 2,
  kLow = 3,
};

/** @brief 任务执行模式 */
enum class TaskExecutionMode {
  kSync,   // 在调度线程内执行（只适合 < 10ms 的短任务，否则会阻塞心跳）
  kAsync,  // 投递到工作线程池执行（网络 I/O、可能阻塞的任务）
};

/**
 * @brief 周期性任务调度器（单例）
 *
 * 用法：
 *   auto& s = PeriodicTaskScheduler::Instance();
 *   s.RegisterTask("heartbeat", 1, [] { ... }, TaskPriority::kCritical, TaskExecutionMode::kSync);
 *   s.Start();
 *   ...
 *   s.Stop();
 */
class PeriodicTaskScheduler {
 public:
  /** @brief 默认基准 tick 间隔（1 秒一次心跳） */
  static constexpr std::chrono::milliseconds kDefaultTickInterval{1000};
  /** @brief 默认异步工作线程数 */
  static constexpr int kDefaultWorkerCount{2};

  static PeriodicTaskScheduler& Instance();
  ~PeriodicTaskScheduler();

  PeriodicTaskScheduler(const PeriodicTaskScheduler&) = delete;
  PeriodicTaskScheduler& operator=(const PeriodicTaskScheduler&) = delete;

  // ---------- 配置（需在 Start() 前调用） ----------

  /** @brief 设置基准 tick 间隔（测试时可设小一点，如 200ms，加快演示） */
  void SetTickInterval(std::chrono::milliseconds interval);
  /** @brief 设置异步工作线程数 */
  void SetWorkerCount(int n);
  /**
   * @brief 设置时间同步守卫：返回 false 时，requires_time_synced 的任务会被跳过
   *
   * 原实现依赖 Device::CheckTimeSyncedOrSkipLog（需要设备模块）；
   * 改成注入回调后，既去掉依赖，又能在测试里模拟"时间未同步"。
   */
  void SetTimeSyncGuard(std::function<bool()> guard);

  // ---------- 任务管理 ----------

  /**
   * @brief 注册（或覆盖）一个周期任务
   * @param name 唯一标识（用于注销/启停/改频率/查统计）
   * @param frequency_multiplier 多少个 tick 执行一次（1 = 每个 tick 都执行）
   * @param func 任务函数
   * @param priority 优先级（同 tick 内按此排序）
   * @param mode 执行模式
   * @param initial_offset 前 N 个 tick 跳过执行（错峰用）
   * @param requires_time_synced 为 true 时，时间未同步则跳过本次执行
   */
  void RegisterTask(const std::string& name,
                    uint32_t frequency_multiplier,
                    std::function<void()> func,
                    TaskPriority priority = TaskPriority::kNormal,
                    TaskExecutionMode mode = TaskExecutionMode::kAsync,
                    uint32_t initial_offset = 0,
                    bool requires_time_synced = false);

  void UnregisterTask(const std::string& name);
  /** @brief 启用/禁用任务（不影响已注册的其它属性） */
  void SetTaskEnabled(const std::string& name, bool enabled);
  /** @brief 动态修改执行频率（不影响启用状态） */
  void SetTaskMultiplier(const std::string& name, uint32_t frequency_multiplier);

  // ---------- 生命周期 ----------

  void Start();
  void Stop();
  bool IsRunning() const { return running_; }

  // ---------- 观测 ----------

  /** @brief 单任务统计 */
  struct TaskStats {
    uint64_t executions = 0;
    uint64_t failures = 0;
    long total_ms = 0;
    long max_ms = 0;
    long min_ms = 0;
  };

  TaskStats GetStats(const std::string& name) const;
  std::vector<std::string> TaskNames() const;
  /** @brief 已执行的总 tick 数（便于验证倍频触发是否正确） */
  uint64_t tick_count() const;

 private:
  PeriodicTaskScheduler() = default;

  struct Task {
    std::string name;
    uint32_t frequency_multiplier = 1;
    uint32_t tick_counter = 0;
    std::function<void()> func;
    TaskPriority priority = TaskPriority::kNormal;
    TaskExecutionMode mode = TaskExecutionMode::kAsync;
    uint32_t initial_offset = 0;
    bool enabled = true;
    bool requires_time_synced = false;

    // 统计（由 Execute 在锁内更新）
    uint64_t executions = 0;
    uint64_t failures = 0;
    long total_ms = 0;
    long max_ms = 0;
    long min_ms = -1;  // -1 表示尚未执行过
  };

  void SchedulerLoop();   // 心跳线程：每 tick_interval_ 调度一次
  void WorkerLoop();      // 工作线程：消费异步任务队列
  void ScheduleTasks();   // 取模判定 + 优先级排序 + 派发
  void Dispatch(const std::shared_ptr<Task>& task);
  void Execute(const std::shared_ptr<Task>& task);

  std::chrono::milliseconds tick_interval_ = kDefaultTickInterval;
  int worker_count_ = kDefaultWorkerCount;
  std::function<bool()> time_sync_guard_;

  // tasks_ 保存 shared_ptr：任务派发后即使被注销，也不会出现悬垂指针
  std::map<std::string, std::shared_ptr<Task>> tasks_;
  mutable std::mutex tasks_mutex_;

  std::deque<std::function<void()>> async_queue_;
  std::mutex queue_mutex_;
  std::condition_variable queue_cv_;

  std::mutex loop_mutex_;
  std::condition_variable loop_cv_;

  std::atomic<bool> running_{false};
  std::atomic<uint64_t> tick_count_{0};
  std::thread scheduler_thread_;
  std::vector<std::thread> worker_threads_;
};

}  // namespace sched
