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
    handler = plugin_ctx_handler_;  // 拷一份出来，保证在锁外执行
  }
  if (handler) handler();  // 锁外调用：避免持锁执行用户代码（可能死锁或长时间阻塞）
}

}  // namespace facility
