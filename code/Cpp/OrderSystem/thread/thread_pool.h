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
class ThreadPool {
 public:
  static ThreadPool* Instance();
  ~ThreadPool();

  void Stop();

 public:
  static constexpr int kInitalThreadsNum = 2;

 public:
  // 把任务投递到线程池的工人线程执行
  template <typename Func>
  inline void Post(Func&& func) {
    Enqueue(std::function<void()>(std::forward<Func>(func)));
  }

 private:
  ThreadPool();
  void Start();
  void Enqueue(std::function<void()> task);
  friend class Threads;

 private:
  std::mutex mtx_;
  std::condition_variable cv_;
  std::deque<std::function<void()>> tasks_;
  bool stopped_ = false;
  std::vector<std::unique_ptr<std::thread>> thrs_;
};
}  // namespace facility
