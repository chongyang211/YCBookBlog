





## 启动与关闭的核心原理分析


---

### 启动流程 (`start.sh`)

```
start.sh
   │
   ├─ ① 设置动态库搜索路径
   │     export LD_LIBRARY_PATH="${SCRIPT_DIR}/lib:${LD_LIBRARY_PATH}"
   │
   └─ ② 通过 start-stop-daemon 启动进程
         start-stop-daemon -S -q --exec "${APP_BINARY}"
```

**原理拆解**：

**① `LD_LIBRARY_PATH`**

iotservice 依赖的 boost、protobuf、openssl 等共享库（`.so`）都在 `lib/` 目录下。Linux 加载器默认只搜索 `/lib`、`/usr/lib` 等系统路径，所以必须通过 `LD_LIBRARY_PATH` 告诉加载器额外的搜索路径。

```
进程启动 → 动态链接器(ld-linux.so)
  → 读取 ELF 的 DT_NEEDED 段（需要哪些 .so）
  → 按 LD_LIBRARY_PATH → /lib → /usr/lib 顺序搜索
  → 找到并加载 → 完成符号解析 → 进入 main()
```

**② `start-stop-daemon`**

这是嵌入式 Linux（BusyBox/OpenWrt）常用的进程管理工具：

| 参数 | 含义 |
|------|------|
| `-S` | Start 模式 |
| `-q` | 安静模式，不输出多余信息 |
| `--exec` | 指定要执行的二进制路径 |

它的核心能力是：**启动前自动检测是否已有同名进程在运行，避免重复启动**。

---

### 应用内部初始化（`Application::Run`）

```
main()
  → Application::Run(argc, argv)
    │
    ├─ ① Setup: 解析命令行参数（--env, --version 等）
    │
    ├─ ② RunGuardCheck: 单实例保护
    │     open("/tmp/iotservice.pid")
    │     lockf(pid_file, F_TLOCK)    ← 尝试加文件锁
    │       ├─ 成功 → 写入当前 PID → 继续启动
    │       └─ 失败 → 说明已有实例在运行 → 退出
    │
    └─ ③ RunMainThread: 进入主事件循环
          │
          ├─ 注册信号处理: SIGINT + SIGTERM
          │     signals->async_wait(HandleSignal)
          │
          ├─ 初始化各模块: Logger → Threads → Device → PaasShadow
          │
          └─ 进入 asio::io_context 事件循环（阻塞，直到收到退出信号）
```

**单实例保护的原理**（`RunGuardCheck`）：

```cpp
int pid_file = ::open("/tmp/iotservice.pid", O_CREAT | O_RDWR, 0660);
if (::lockf(pid_file, F_TLOCK, 0) < 0) {
  // 文件锁被占用 → 另一个实例正在运行 → 退出
  return false;
}
// 锁成功 → 写入当前 PID
::write(pid_file, pid.c_str(), pid.length());
// 故意不关闭文件 → 进程退出时 OS 自动释放锁
```

这比单纯检查 PID 文件是否存在更可靠——进程崩溃后 OS 会自动释放文件锁，不会留下"僵尸" PID 文件导致无法重启。

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