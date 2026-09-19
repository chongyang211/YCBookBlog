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

// 预置的全局线程类型（每项对应一条常驻线程）
enum GlobalThreadType {
  kMainThread = 0,
  kUiThread = kMainThread,  // UI 线程复用主线程
  kNetworkThread,
  kHeartBeatThread,
  kIotThread,
  kOtaInstallThread,
};

constexpr int kGlobalThreadAll = kOtaInstallThread + 1;  // 全局线程总数（用于数组维度与遍历）

constexpr const char *kGlobalThreadNames[] = {"main", "network", "heartbeat", "iot", "ota_install"};

// 全局线程与线程池的统一入口：负责创建、启动与销毁顺序
class Threads {
 private:
  static Threads *Instance();  // 单例（函数内静态）

 public:
  static inline Thread *GlobalThread(GlobalThreadType type) { return Instance()->thrs_[type].get(); }
  static inline Thread *MainThread() { return GlobalThread(kMainThread); }
  static inline Thread *UiThread() { return GlobalThread(kUiThread); }
  static inline Thread *NetworkThread() { return GlobalThread(kNetworkThread); }
  static inline Thread *HeartBeatThread() { return GlobalThread(kHeartBeatThread); }
  static inline Thread *IotThread() { return GlobalThread(kIotThread); }
  static inline Thread *OtaInstallThread() { return GlobalThread(kOtaInstallThread); }

  static inline ThreadPool *Pool() { return ThreadPool::Instance(); }

  // 创建独立线程（返回裸指针，由调用方持有并在合适时机释放）
  // // created thread is managed by Threads
  static Thread *CreateThread(std::string_view name);
  static ExtensibleThread *CreateExtensibleThread(std::string_view name);

  // static void StopThread(Thread *inst);

 private:
  // 把主线程包装成 Thread：在 main 线程内直接跑主循环，不新建系统线程
  template <typename Func>
  void RunMainThread(Func &&func) {
    thrs_[kMainThread] = std::unique_ptr<Thread>(new Thread());
    thrs_[kMainThread]->SetName(kGlobalThreadNames[kMainThread]);
    thrs_[kMainThread]->StartInCurrentThread(std::move(func));
  }
  friend class Application;  // 由 Application 启动主线程循环

 private:
  void Init();  // 创建并启动所有全局线程，随后启动线程池

 private:
  Threads();
  ~Threads();

  std::array<std::unique_ptr<Thread>, static_cast<size_t>(kGlobalThreadAll)> thrs_;  // 全局线程表（按类型索引）
};

}  // namespace facility
