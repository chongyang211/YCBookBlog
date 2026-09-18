// Copyright © 1998 - 2023 Tencent. All Rights Reserved.

#pragma once

#include <array>
#include <memory>
#include <string_view>
#include <utility>

#include "extensible_thread.h"
#include "thread.h"
#include "thread_pool.h"

namespace facility {

enum GlobalThreadType {
  kMainThread = 0,
  kUiThread = kMainThread,
  kNetworkThread,
  kHeartBeatThread,
  kIotThread,
  kOtaInstallThread,
};

constexpr int kGlobalThreadAll = kOtaInstallThread + 1;

constexpr const char *kGlobalThreadNames[] = {"main", "network", "heartbeat", "iot", "ota_install"};

class Threads {
 private:
  static Threads *Instance();

 public:
  static inline Thread *GlobalThread(GlobalThreadType type) { return Instance()->thrs_[type].get(); }
  static inline Thread *MainThread() { return GlobalThread(kMainThread); }
  static inline Thread *UiThread() { return GlobalThread(kUiThread); }
  static inline Thread *NetworkThread() { return GlobalThread(kNetworkThread); }
  static inline Thread *HeartBeatThread() { return GlobalThread(kHeartBeatThread); }
  static inline Thread *IotThread() { return GlobalThread(kIotThread); }
  static inline Thread *OtaInstallThread() { return GlobalThread(kOtaInstallThread); }

  static inline ThreadPool *Pool() { return ThreadPool::Instance(); }

  // // created thread is managed by Threads
  static Thread *CreateThread(std::string_view name);
  static ExtensibleThread *CreateExtensibleThread(std::string_view name);

  // static void StopThread(Thread *inst);

 private:
  template <typename Func>
  void RunMainThread(Func &&func) {
    thrs_[kMainThread] = std::unique_ptr<Thread>(new Thread());
    thrs_[kMainThread]->SetName(kGlobalThreadNames[kMainThread]);
    thrs_[kMainThread]->StartInCurrentThread(std::move(func));
  }
  friend class Application;

 private:
  void Init();

 private:
  Threads();
  ~Threads();

  std::array<std::unique_ptr<Thread>, static_cast<size_t>(kGlobalThreadAll)> thrs_;
};

}  // namespace facility
