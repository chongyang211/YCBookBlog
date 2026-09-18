// Copyright © 1998 - 2024 Tencent. All Rights Reserved.

#include "extensible_thread.h"

namespace facility {

void ExtensibleThread::SetPluginCtxHandler(std::function<void()>&& handler) {
  std::lock_guard<std::mutex> lock(plugin_mtx_);
  plugin_ctx_handler_ = std::move(handler);
}

void ExtensibleThread::OnTaskDone() {
  std::function<void()> handler;
  {
    std::lock_guard<std::mutex> lock(plugin_mtx_);
    handler = plugin_ctx_handler_;
  }
  if (handler) handler();
}

}  // namespace facility
