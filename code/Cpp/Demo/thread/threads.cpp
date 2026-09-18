// Copyright © 1998 - 2023 Tencent. All Rights Reserved.

#include "threads.h"

namespace facility {

static_assert(sizeof(kGlobalThreadNames) / sizeof(const char *) == static_cast<size_t>(kGlobalThreadAll),
              "all global threads must have names");

Threads *Threads::Instance() {
  static Threads inst;
  return &inst;
}

Thread *Threads::CreateThread(std::string_view name) {
  auto thr = new Thread;
  thr->SetName(name);
  thr->Start();
  return thr;
}

ExtensibleThread *Threads::CreateExtensibleThread(std::string_view name) {
  auto thr = new ExtensibleThread;
  thr->SetName(name);
  thr->Start();
  return thr;
}

// void Threads::StopThread(Thread *inst) {}

Threads::Threads() { Init(); }

Threads::~Threads() = default;

void Threads::Init() {
  for (int i = kMainThread; i < kGlobalThreadAll; i++) {
    thrs_[i] = std::unique_ptr<Thread>(new Thread());
    thrs_[i]->SetName(kGlobalThreadNames[i]);
    thrs_[i]->Start();
  }
  ThreadPool::Instance()->Start();
}

}  // namespace facility
