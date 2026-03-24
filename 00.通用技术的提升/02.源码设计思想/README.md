





### 五、异常匹配规则

---

### 六、跨线程异常传递

线程异常的最大难题：**子线程的异常如何传回主线程？**

```
主线程                          子线程
  │                               │
  │  启动子线程 ───────────────▶  │
  │                               │  throw/panic
  │                               │  异常只在子线程栈内传播
  │                               │  主线程完全无感知！
  │  join() ◀─────────────────── │
  │  此时才能获取异常
```

| 语言 | 传递机制 | 原理 |
|------|---------|------|
| **Java** | `Future.get()` 抛 `ExecutionException` | 线程池捕获异常 → 存入 Future 的 outcome 字段 → get() 时重新抛出 |
| **C++** | `std::future.get()` 重抛 | `exception_ptr` 保存异常 → `rethrow_exception()` 在调用线程重抛 |
| **Go** | channel 传递 error | 子 goroutine `defer-recover` 捕获 panic → 通过 channel 发送给主 goroutine |
| **Rust** | `JoinHandle.join()` 返回 `Result` | `thread::spawn` 的返回值是 `Result<T, Box<dyn Any>>` |
| **Python** | `Future.result()` 重抛 | `concurrent.futures` 内部捕获异常 → 存储 → `result()` 时重新 raise |
| **JavaScript** | `Promise.catch()` / `worker.onerror` | rejection 在 Promise 链传播；Worker 异常通过 error 事件通知主线程 |

---

### 七、总结：异常处理的统一模型
