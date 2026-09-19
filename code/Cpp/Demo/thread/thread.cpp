// Copyright © 1998 - 2023 Tencent. All Rights Reserved.

#include "thread.h"

#include <future>
#include <pthread.h>

namespace facility {

static thread_local Thread* this_thr = nullptr;  // 当前线程对应的 Thread 对象（线程局部）

namespace {

// 设置当前线程名（不同平台签名不同）
void SetCurrentThreadName(const std::string& name) {
  if (name.empty()) return;
#if defined(__APPLE__)
  ::pthread_setname_np(name.c_str());
#else
  ::pthread_setname_np(::pthread_self(), name.c_str());
#endif
}

}  // namespace

Thread::~Thread() {
  Stop();
  if (thr_ && thr_->joinable()) thr_->join();
}

Thread::Thread() = default;

void Thread::Start() {
  // 等新线程跑完 SetupThreadLocal（填好 this_thr / id_）再返回，
  // 这样调用方在 Start() 之后立刻用 Id() 也能拿到正确的值。
  auto started = std::make_shared<std::promise<void>>();
  auto ready = started->get_future();
  thr_ = std::make_unique<std::thread>([this, started]() {
    SetupThreadLocal();
    started->set_value();
    RunCtx();
  });
  ready.wait();
}

void Thread::Stop() {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    stopped_ = true;
  }
  cv_.notify_all();  // 唤醒阻塞在 wait 上的主循环
}

void Thread::SetupThreadLocal() {
  this_thr = this;
  id_ = static_cast<unsigned long int>(std::hash<std::thread::id>{}(std::this_thread::get_id()));  // NOLINT
  SetCurrentThreadName(name_);
}

void Thread::Enqueue(std::function<void()> task) {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    if (stopped_) return;
    tasks_.push_back(std::move(task));
  }
  cv_.notify_one();  // 只唤醒一个等待者即可（一次取一个任务）
}

void Thread::OnTaskDone() {}

void Thread::RunCtx() {
  SetupThreadLocal();
  while (true) {
    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lock(mtx_);
      cv_.wait(lock, [this]() { return stopped_ || !tasks_.empty(); });  // 防虚假唤醒
      if (tasks_.empty()) {
        if (stopped_) return;  // 停止且任务已排空 → 退出主循环
        continue;
      }
      task = std::move(tasks_.front());
      tasks_.pop_front();
    }
    if (task) task();  // 锁外执行任务，避免持锁运行用户代码
    OnTaskDone();      // 任务完成钩子（子类可扩展）
  }
}

Thread* this_thread::Handle() { return this_thr; }

}  // namespace facility
