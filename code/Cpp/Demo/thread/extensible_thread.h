// Copyright © 1998 - 2024 Tencent. All Rights Reserved.

#pragma once

#include <functional>
#include <mutex>

#include "thread.h"

namespace facility {

// 可插桩的线程：在 Thread 的基础上，允许外部注入一个"插件上下文"回调，
// 该回调会在【每个任务执行完成后】被调用（例如驱动脚本引擎的一轮 tick）。
class ExtensibleThread : public Thread {
 public:
  // 设置插件回调（线程安全）；传空函数表示取消回调
  void SetPluginCtxHandler(std::function<void()>&& handler);

 protected:
  using Thread::Thread;
  ~ExtensibleThread() override = default;

  void OnTaskDone() override;  // 重写钩子：任务执行完后触发插件回调

 private:
  std::mutex plugin_mtx_;                 // 保护插件回调的读写（可能与投递方并发）
  std::function<void()> plugin_ctx_handler_;  // 任务完成后的回调（可为空）
};
}  // namespace facility
