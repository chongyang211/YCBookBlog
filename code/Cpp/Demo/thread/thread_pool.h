// Copyright © 1998 - 2023 Tencent. All Rights Reserved.

#pragma once

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace facility {

// 固定工人数量的线程池：多个工人线程竞争同一个任务队列，
// 适合"短平快、可并行"的任务；需要串行执行的场景请用 Thread。
class ThreadPool {
 public:
  static ThreadPool* Instance();  // 进程内唯一实例（懒初始化）
  ~ThreadPool();                  // 停止并 join 所有工人线程

  void Stop();  // 置停止标记并唤醒所有工人；已入队的任务不再执行

 public:
  static constexpr int kInitalThreadsNum = 2;  // 初始工人线程数

 public:
  // 把任务投递到线程池的工人线程执行
  template <typename Func>
  inline void Post(Func&& func) {
    Enqueue(std::function<void()>(std::forward<Func>(func)));
  }

 private:
  ThreadPool();                          // 私有构造：只能通过 Instance() / Threads 获取
  void Start();                          // 创建并启动全部工人线程（由 Threads::Init 调用）
  void Enqueue(std::function<void()> task);  // 入队（线程安全）；已停止则丢弃
  friend class Threads;                  // 生命周期由 Threads 托管

 private:
  std::mutex mtx_;                              // 保护 tasks_ 与 stopped_
  std::condition_variable cv_;                  // 任务到达或停止时唤醒工人
  std::deque<std::function<void()>> tasks_;     // 共享任务队列（FIFO）
  bool stopped_ = false;                        // 停止标记
  std::vector<std::unique_ptr<std::thread>> thrs_;  // 工人线程集合
};
}  // namespace facility
