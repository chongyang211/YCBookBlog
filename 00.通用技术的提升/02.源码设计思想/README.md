





### 四、资源清理机制对比

这是异常处理中**最关键**的部分——不是 catch，而是清理：

| 语言 | 清理机制 | 原理 | 是否自动 |
|------|---------|------|---------|
| **C++** | RAII（析构函数） | 栈展开时编译器自动调用每个局部对象的析构函数 | 完全自动 |
| **Rust** | Drop trait | 同 C++，展开时自动调用 `drop()` | 完全自动 |
| **Java** | finally / try-with-resources | finally 被编译器复制到所有出口；TWR 编译为 finally + close() | 半自动 |
| **Python** | finally / with(上下文管理器) | finally 通过 block 栈保证执行；with 调用 `__exit__()` | 半自动 |
| **Go** | defer | defer 函数注册到链表，panic 时逆序执行 | 半自动（需手动 defer） |
| **JavaScript** | finally | 引擎保证 finally 块在 try/catch 之后执行 | 半自动 |
| **C** | 无内建机制 | 需手动 goto cleanup 或 setjmp/longjmp | 完全手动 |

**RAII vs finally 的本质区别**：

```
RAII：资源的生命周期绑定到对象的生命周期，对象销毁时资源自动释放。
      你不需要记得清理，编译器帮你做。

finally：你必须记得写 finally 块，忘了就泄漏。
         但它比 RAII 灵活——可以做任意清理逻辑，不限于单个对象。
```

---

### 五、异常匹配规则

| 语言 | 匹配方式 | 多 catch 顺序 | 泛匹配 |
|------|---------|-------------|--------|
| **Java** | `instanceof` 类型检查（支持继承） | 从上到下，第一个匹配生效；子类必须在父类前面 | `catch (Exception e)` |
| **C++** | RTTI `type_info` 比较（支持继承） | 从上到下；`catch(...)` 匹配所有 | `catch (...)` |
| **Python** | `isinstance()` 检查（支持继承+元组） | 从上到下 | `except Exception:` / `except:` |
| **JavaScript** | 无类型匹配，只有一个 catch | 单一 catch，内部用 `instanceof` 判断 | `catch (e)` 捕获一切 |
| **Go** | 无匹配，`recover()` 捕获当前 goroutine 的 panic | N/A | `recover()` 捕获一切 |
| **Rust** | `catch_unwind` 捕获所有 panic；`Result` 通过模式匹配 | `match` 穷举 | `catch_unwind(\|\| ...)` |

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
