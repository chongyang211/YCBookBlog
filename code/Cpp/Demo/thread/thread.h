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

// 带任务队列的独立线程：投递的任务在本线程内按 FIFO 串行执行，
// 调用 Stop() 后主循环处理完剩余任务并退出。
class Thread {
 public:
  Thread& operator=(const Thread&) = delete;
  Thread(const Thread&) = delete;
  virtual ~Thread();  // 析构前先 Stop() 再 join，保证线程安全退出

  inline std::string GetName() { return name_; }
  void Stop();  // 置停止标记并唤醒主循环；不打断正在执行的任务

 public:
  // 把一个可调用对象投递到本线程执行（线程安全）
  template <typename Func>
  inline void Post(Func&& func) {
    Enqueue(std::function<void()>(std::forward<Func>(func)));
  }

  inline std::string Name() { return name_; }
  unsigned long int Id() { return id_; }  // NOLINT  // 线程唯一 ID（由 thread::id 哈希而来）

 protected:
  Thread();    // 只允许 Threads / 子类创建
  void Start();  // 新建系统线程，并等待其初始化（SetupThreadLocal）完成后返回
  void SetupThreadLocal();  // 绑定线程局部指针、生成 id_、设置系统线程名
  inline void SetName(std::string_view name) { name_ = name; }  // 需在 Start() 前调用

  // 在当前线程（通常是主线程）内跑主循环，不新建系统线程
  template <typename Func>
  void StartInCurrentThread(Func&& func) {
    SetupThreadLocal();
    Post(std::move(func));
    RunCtx();
  }

  friend class ThreadManager;  // 全局线程的创建与生命周期由 ThreadManager 统一管理

 protected:
  // 每执行完一个任务后调用，子类可重写做扩展（默认空实现）
  virtual void OnTaskDone();

  // 线程主循环：不断从任务队列取任务并执行，直到 Stop()
  virtual void RunCtx();

 private:
  void Enqueue(std::function<void()> task);  // 入队（线程安全）；已停止则丢弃任务

 private:
  unsigned long int id_ = 0;  // NOLINT                  // 线程 ID
  std::string name_;                                    // 线程名（便于调试/打点）
  std::unique_ptr<std::thread> thr_ = nullptr;           // 底层系统线程；为空表示复用当前线程

  std::mutex mtx_;                                      // 保护 tasks_ 与 stopped_
  std::condition_variable cv_;                          // 有新任务或停止时唤醒主循环
  std::deque<std::function<void()>> tasks_;             // 待执行任务队列（FIFO）
  bool stopped_ = false;                                // 停止标记
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
Thread* Handle();  // 当前线程的 Thread 指针；非本框架创建的线程返回 nullptr

template <typename Func>
inline void Post(Func&& func) {
  Handle()->Post(std::forward<Func>(func));
}
}  // namespace this_thread
}  // namespace facility
