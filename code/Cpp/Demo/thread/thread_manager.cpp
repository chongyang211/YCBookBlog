// Copyright © 1998 - 2023 Tencent. All Rights Reserved.

#include "thread_manager.h"

namespace facility {

// 编译期校验：线程名数量必须与全局线程数量一致，避免漏配/多配
static_assert(sizeof(kGlobalThreadNames) / sizeof(const char *) == static_cast<size_t>(kGlobalThreadAll),
              "all global threads must have names");

ThreadManager *ThreadManager::Instance() {
  static ThreadManager inst;
  return &inst;
}

Thread *ThreadManager::CreateThread(std::string_view name) {
  auto thr = new Thread;
  thr->SetName(name);
  thr->Start();
  return thr;
}

ExtensibleThread *ThreadManager::CreateExtensibleThread(std::string_view name) {
  auto thr = new ExtensibleThread;
  thr->SetName(name);
  thr->Start();
  return thr;
}

ThreadManager::ThreadManager() { Init(); }

ThreadManager::~ThreadManager() = default;  // thrs_ 中 unique_ptr 析构时各自 Stop + join

void ThreadManager::Init() {
  for (int i = kMainThread; i < kGlobalThreadAll; i++) {
    thrs_[i] = std::unique_ptr<Thread>(new Thread());
    thrs_[i]->SetName(kGlobalThreadNames[i]);
    thrs_[i]->Start();  // 逐个启动；Start 内部会等线程初始化完成
  }
  ThreadPool::Instance()->Start();  // 全局线程就绪后再启动线程池
}

}  // namespace facility
