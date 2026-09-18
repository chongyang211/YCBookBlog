// Copyright © 1998 - 2023 Tencent. All Rights Reserved.

#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

namespace facility {

class Thread {
 public:
  Thread& operator=(const Thread&) = delete;
  Thread(const Thread&) = delete;
  virtual ~Thread();

  inline std::string GetName() { return name_; }
  void Stop();

 public:
  // 把一个可调用对象投递到本线程执行（线程安全）
  template <typename Func>
  inline void Post(Func&& func) {
    Enqueue(std::function<void()>(std::forward<Func>(func)));
  }

  inline std::string Name() { return name_; }
  unsigned long int Id() { return id_; }  // NOLINT

 protected:
  Thread();
  void Start();
  void SetupThreadLocal();
  inline void SetName(std::string_view name) { name_ = name; }

  template <typename Func>
  void StartInCurrentThread(Func&& func) {
    SetupThreadLocal();
    Post(std::move(func));
    RunCtx();
  }

  friend class Threads;

 protected:
  // 每执行完一个任务后调用，子类可重写做扩展（默认空实现）
  virtual void OnTaskDone();

  // 线程主循环：不断从任务队列取任务并执行，直到 Stop()
  virtual void RunCtx();

 private:
  void Enqueue(std::function<void()> task);

 private:
  unsigned long int id_ = 0;  // NOLINT
  std::string name_;
  std::unique_ptr<std::thread> thr_ = nullptr;

  std::mutex mtx_;
  std::condition_variable cv_;
  std::deque<std::function<void()>> tasks_;
  bool stopped_ = false;
};

/**
 * 主要调用this_thread命名空间下的函数即可
 *
 * 举例1：
 * this_thread::Post([]() {
 *  printf("detached run");
 * });
 */
namespace this_thread {
Thread* Handle();

template <typename Func>
inline void Post(Func&& func) {
  Handle()->Post(std::forward<Func>(func));
}
}  // namespace this_thread
}  // namespace facility
