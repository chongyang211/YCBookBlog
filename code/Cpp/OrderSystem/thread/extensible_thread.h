// Copyright © 1998 - 2024 Tencent. All Rights Reserved.

#pragma once

#include <functional>
#include <mutex>

#include "thread.h"

namespace facility {
class ExtensibleThread : public Thread {
 public:
  void SetPluginCtxHandler(std::function<void()>&& handler);

 protected:
  using Thread::Thread;
  ~ExtensibleThread() override = default;

  void OnTaskDone() override;

 private:
  std::mutex plugin_mtx_;
  std::function<void()> plugin_ctx_handler_;
};
}  // namespace facility
