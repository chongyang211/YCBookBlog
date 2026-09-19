// Copyright © 1998 - 2023 Tencent. All Rights Reserved.

#include "thread_pool.h"

#include <pthread.h>

namespace facility {

namespace {

// 设置当前线程名（不同平台签名不同）
void SetCurrentThreadName(const std::string& name) {
#if defined(__APPLE__)
  ::pthread_setname_np(name.c_str());
#else
  ::pthread_setname_np(::pthread_self(), name.c_str());
#endif
}

}  // namespace

ThreadPool* ThreadPool::Instance() {
  static ThreadPool pool;  // 函数内静态变量：C++11 起初始化线程安全
  return &pool;
}

ThreadPool::~ThreadPool() {
  Stop();
  for (auto& thr : thrs_) {
    if (thr && thr->joinable()) thr->join();
  }
}

void ThreadPool::Stop() {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    stopped_ = true;
  }
  cv_.notify_all();  // 唤醒所有阻塞中的工人
}

ThreadPool::ThreadPool() : thrs_(kInitalThreadsNum) {}

void ThreadPool::Enqueue(std::function<void()> task) {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    if (stopped_) return;
    tasks_.push_back(std::move(task));
  }
  cv_.notify_one();  // 唤醒任意一个空闲工人
}

void ThreadPool::Start() {
  for (size_t i = 0; i < thrs_.size(); i++) {
    thrs_[i] = std::make_unique<std::thread>([i, this]() {
      SetCurrentThreadName("thr_p_" + std::to_string(i));
      while (true) {  // 工人主循环：等任务 → 取任务 → 执行
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock(mtx_);
          cv_.wait(lock, [this]() { return stopped_ || !tasks_.empty(); });  // 防虚假唤醒
          if (tasks_.empty()) {
            if (stopped_) return;  // 停止且队列已空 → 工人退出
            continue;
          }
          task = std::move(tasks_.front());
          tasks_.pop_front();
        }
        if (task) task();  // 锁外执行，避免阻塞其他工人取任务
      }
    });
  }
}
}  // namespace facility
