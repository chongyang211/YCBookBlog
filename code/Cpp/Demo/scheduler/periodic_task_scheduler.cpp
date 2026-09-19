// Copyright © 1998 - 2026 Tencent. All Rights Reserved.

#include "scheduler/periodic_task_scheduler.h"

#include <algorithm>

namespace palm {

PeriodicTaskScheduler& PeriodicTaskScheduler::Instance() {
  static PeriodicTaskScheduler inst;
  return inst;
}

PeriodicTaskScheduler::~PeriodicTaskScheduler() { Stop(); }

void PeriodicTaskScheduler::SetTickInterval(std::chrono::milliseconds interval) {
  tick_interval_ = interval;
}

void PeriodicTaskScheduler::SetWorkerCount(int n) { worker_count_ = n; }

void PeriodicTaskScheduler::SetTimeSyncGuard(std::function<bool()> guard) {
  time_sync_guard_ = std::move(guard);
}

// ---------- 任务管理 ----------

void PeriodicTaskScheduler::RegisterTask(const std::string& name,
                                         uint32_t frequency_multiplier,
                                         std::function<void()> func,
                                         TaskPriority priority,
                                         TaskExecutionMode mode,
                                         uint32_t initial_offset,
                                         bool requires_time_synced) {
  auto task = std::make_shared<Task>();
  task->name = name;
  task->frequency_multiplier = frequency_multiplier;
  task->func = std::move(func);
  task->priority = priority;
  task->mode = mode;
  task->initial_offset = initial_offset;
  task->requires_time_synced = requires_time_synced;

  std::lock_guard<std::mutex> lock(tasks_mutex_);
  tasks_[name] = std::move(task);  // 同名覆盖（等价于重新注册）
}

void PeriodicTaskScheduler::UnregisterTask(const std::string& name) {
  std::lock_guard<std::mutex> lock(tasks_mutex_);
  tasks_.erase(name);
}

void PeriodicTaskScheduler::SetTaskEnabled(const std::string& name, bool enabled) {
  std::lock_guard<std::mutex> lock(tasks_mutex_);
  auto it = tasks_.find(name);
  if (it != tasks_.end()) it->second->enabled = enabled;
}

void PeriodicTaskScheduler::SetTaskMultiplier(const std::string& name,
                                              uint32_t frequency_multiplier) {
  std::lock_guard<std::mutex> lock(tasks_mutex_);
  auto it = tasks_.find(name);
  if (it != tasks_.end()) it->second->frequency_multiplier = frequency_multiplier;
}

// ---------- 生命周期 ----------

void PeriodicTaskScheduler::Start() {
  if (running_.exchange(true)) return;  // 已启动则忽略

  // 异步工作线程：消费 async_queue_
  for (int i = 0; i < worker_count_; i++) {
    worker_threads_.emplace_back(&PeriodicTaskScheduler::WorkerLoop, this);
  }
  // 心跳线程：按 tick_interval_ 周期触发 ScheduleTasks()
  scheduler_thread_ = std::thread(&PeriodicTaskScheduler::SchedulerLoop, this);
}

void PeriodicTaskScheduler::Stop() {
  if (!running_.exchange(false)) return;  // 未启动则忽略

  loop_cv_.notify_all();     // 唤醒心跳线程
  queue_cv_.notify_all();    // 唤醒工作线程（队列消费完后退出）

  if (scheduler_thread_.joinable()) scheduler_thread_.join();
  for (auto& t : worker_threads_) {
    if (t.joinable()) t.join();
  }
  worker_threads_.clear();
}

// ---------- 线程主循环 ----------

void PeriodicTaskScheduler::SchedulerLoop() {
  while (running_) {
    {
      std::unique_lock<std::mutex> lock(loop_mutex_);
      // wait_for 而非 sleep_for：Stop() 能立刻唤醒，不必等满一个周期
      loop_cv_.wait_for(lock, tick_interval_, [this] { return !running_; });
    }
    if (!running_) break;
    tick_count_++;
    ScheduleTasks();
  }
}

void PeriodicTaskScheduler::WorkerLoop() {
  while (true) {
    std::function<void()> job;
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      queue_cv_.wait(lock, [this] { return !running_ || !async_queue_.empty(); });
      if (async_queue_.empty()) {
        if (!running_) break;   // 停止且队列已排空 → 退出
        continue;
      }
      job = std::move(async_queue_.front());
      async_queue_.pop_front();
    }
    job();
  }
}

// ---------- 调度 ----------

void PeriodicTaskScheduler::ScheduleTasks() {
  std::vector<std::shared_ptr<Task>> ready;
  {
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    for (auto& [name, task] : tasks_) {
      if (!task->enabled) continue;
      task->tick_counter++;
      // ① 跳过错峰期：tick_counter 必须大于 initial_offset
      // ② 倍频判定：错峰之后的计数对 frequency_multiplier 取模，为 0 才执行
      if (task->tick_counter > task->initial_offset &&
          (task->tick_counter - task->initial_offset) % task->frequency_multiplier == 0) {
        ready.push_back(task);  // 持有 shared_ptr，注销也不会悬垂
      }
    }
  }

  // 同一 tick 内按优先级升序执行（kCritical 最先）
  std::sort(ready.begin(), ready.end(),
            [](const std::shared_ptr<Task>& a, const std::shared_ptr<Task>& b) {
              return static_cast<int>(a->priority) < static_cast<int>(b->priority);
            });

  for (auto& task : ready) Dispatch(task);
}

void PeriodicTaskScheduler::Dispatch(const std::shared_ptr<Task>& task) {
  if (task->mode == TaskExecutionMode::kSync) {
    Execute(task);  // 短任务：直接在心跳线程内跑（超过 10ms 会阻塞后续 tick）
    return;
  }
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    async_queue_.push_back([this, task] { Execute(task); });
  }
  queue_cv_.notify_one();
}

void PeriodicTaskScheduler::Execute(const std::shared_ptr<Task>& task) {
  // 时间敏感任务：守卫判定为 false 时跳过本次执行（不计数）
  if (task->requires_time_synced && time_sync_guard_ && !time_sync_guard_()) {
    return;
  }

  auto start = std::chrono::steady_clock::now();
  bool ok = true;
  try {
    task->func();
  } catch (...) {
    ok = false;
  }
  long cost = static_cast<long>(
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start)
          .count());

  std::lock_guard<std::mutex> lock(tasks_mutex_);
  if (ok) {
    task->executions++;
  } else {
    task->failures++;
  }
  task->total_ms += cost;
  task->max_ms = std::max(task->max_ms, cost);
  task->min_ms = (task->min_ms < 0) ? cost : std::min(task->min_ms, cost);
}

// ---------- 观测 ----------

PeriodicTaskScheduler::TaskStats PeriodicTaskScheduler::GetStats(const std::string& name) const {
  TaskStats s;
  std::lock_guard<std::mutex> lock(tasks_mutex_);
  auto it = tasks_.find(name);
  if (it == tasks_.end()) return s;
  const Task& t = *it->second;
  s.executions = t.executions;
  s.failures = t.failures;
  s.total_ms = t.total_ms;
  s.max_ms = std::max(t.max_ms, 0L);
  s.min_ms = t.min_ms < 0 ? 0 : t.min_ms;
  return s;
}

std::vector<std::string> PeriodicTaskScheduler::TaskNames() const {
  std::vector<std::string> names;
  std::lock_guard<std::mutex> lock(tasks_mutex_);
  for (const auto& [name, task] : tasks_) names.push_back(name);
  return names;
}

uint64_t PeriodicTaskScheduler::tick_count() const { return tick_count_; }

}  // namespace palm
