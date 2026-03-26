

### 核心设计思想





---

### 关闭流程 (`stop.sh`)

```
stop.sh
   │
   ├─ 方法1: PID 文件
   │     读取 /tmp/iotservice.pid → kill $pid
   │
   └─ 方法2: 进程名兜底
         pkill -f "iotservice"
```

**进程收到信号后的处理**（`HandleSignal`）：

```
kill $pid  (发送 SIGTERM)
   │
   └─ 进程收到 SIGTERM
        → asio::signal_set 触发回调 HandleSignal
          → 在主线程 dispatch:
            ① signals->remove(SIGTERM)   // 移除信号监听，恢复默认处理
            ② ::raise(SIGTERM)           // 重新发送信号给自己
               → 默认处理: 进程终止
               → OS 回收资源，自动释放 PID 文件锁
```

**为什么要 remove 再 raise，而不是直接 exit？**

这是一种标准的**优雅退出模式**：

1. 先进入自己的信号处理函数，有机会做清理（日志刷盘、连接断开等）
2. 恢复默认信号处理器
3. 重新 raise 信号 → 以**正确的退出码**终止（告诉父进程/系统"我是被信号杀死的"，而非正常退出）

这让 `start-stop-daemon` 和其他监控工具能正确识别进程的退出原因。

---

### 完整生命周期图

```
┌─────────────────────────────────────────────────────────────┐
│ start.sh                                                    │
│  export LD_LIBRARY_PATH → start-stop-daemon -S iotservice   │
└────────────────────────────┬────────────────────────────────┘
                             │ fork+exec
                             ▼
┌─────────────────────────────────────────────────────────────┐
│ iotservice 进程                                              │
│                                                             │
│  ① ParseArgs (--env, --signal-parent, ...)                  │
│  ② RunGuardCheck                                            │
│     open(/tmp/iotservice.pid) → lockf() → write(PID)        │
│  ③ 注册信号: SIGINT, SIGTERM → HandleSignal                  │
│  ④ 初始化模块 → 进入 asio 事件循环                            │
│                    ↑                                         │
│                    │ 持续运行...                               │
│                    │                                         │
└─────────────────────────────────────┬───────────────────────┘
                                      │
┌─────────────────────────────────────┼───────────────────────┐
│ stop.sh                            │                        │
│  cat /tmp/iotservice.pid → kill PID ┘                       │
│  pkill -f iotservice (兜底)         │                        │
└─────────────────────────────────────┼───────────────────────┘
                                      │ SIGTERM
                                      ▼
┌─────────────────────────────────────────────────────────────┐
│ HandleSignal                                                │
│  ① remove(SIGTERM)  // 恢复默认处理                          │
│  ② raise(SIGTERM)   // 重发信号 → 进程终止                    │
│                                                             │
│  OS 回收进程 → 自动释放 PID 文件锁                            │
└─────────────────────────────────────────────────────────────┘
```

### 一句话总结

**启动**：设置库路径 → `start-stop-daemon` 防重复启动 → 进程内 PID 文件锁双重保护 → 注册信号监听 → 进入异步事件循环。**关闭**：通过 PID 文件或 `pkill` 发送 `SIGTERM` → 信号回调中恢复默认处理并 re-raise → 进程以信号退出码终止，OS 自动释放文件锁。