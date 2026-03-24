



---

### 二、底层核心机制：栈展开（Stack Unwinding）

这是所有语言异常处理的核心原理。

#### 调用栈的结构

```
内存高地址
┌──────────────┐
│  main()      │  栈帧：局部变量、返回地址、保存的寄存器
├──────────────┤
│  funcA()     │
├──────────────┤
│  funcB()     │
├──────────────┤
│  funcC()     │  ← 异常在这里发生
└──────────────┘
内存低地址 (栈顶)
```

**栈展开就是从 funcC 开始，逐帧向上回退，直到找到 catch/recover：**

```
步骤1：检查 funcC 有没有匹配的 handler → 没有 → 销毁 funcC 的局部对象 → 弹出帧
步骤2：检查 funcB 有没有匹配的 handler → 没有 → 销毁 funcB 的局部对象 → 弹出帧
步骤3：检查 funcA 有没有匹配的 handler → 有 catch → 跳转到 handler 执行
```

---

### 三、各语言具体如何实现

#### 1. Java / JVM：异常表驱动

```
每个方法编译后都附带一张 ExceptionTable：

ExceptionTable:
  from   to    target  type
  4      10    13      IOException      // 字节码4-10行，IOException 跳到13
  4      10    20      Exception        // 字节码4-10行，Exception 跳到20
  4      23    26      any              // finally 块（匹配任何异常）
```

**处理流程**：

```
athrow 指令执行
  │
  ▼
① 取当前 PC（程序计数器）
  │
  ▼
② 在当前方法的 ExceptionTable 中查找：
   PC 是否在 [from, to) 范围内？
   异常对象是否是 type 的实例（instanceof）？
  │
  ├── 匹配 → 跳转到 target 位置执行（catch 块）
  │
  └── 不匹配 → 弹出当前栈帧，取调用方方法的 ExceptionTable 继续查
                │
                └── 到栈顶仍无匹配 → Thread.dispatchUncaughtException()
```

**finally 的实现**：编译器把 finally 块的代码复制到三个位置——try 正常出口、catch 出口、异常出口。保证无论如何都执行。

#### 2. C++：两阶段展开 + RAII

C++ 的异常处理是所有语言中最复杂的：

**阶段一：搜索（Search Phase）**

```
从 throw 点开始，只查找，不执行任何清理：
  → 查 .eh_frame 表（DWARF 格式），里面记录了：
    - 每个 PC 范围对应的 catch 类型
    - 每个帧需要执行的清理动作（析构函数列表）
  → 找到匹配的 catch → 记住位置，进入阶段二
  → 找不到任何匹配 → 调用 std::terminate()，进程终止
```

**阶段二：清理（Cleanup Phase）**

```
从 throw 点重新开始展开：
  → 逐帧执行析构函数（RAII 的核心保障）
    帧1：~unique_ptr() → 释放堆内存
    帧2：~lock_guard() → 释放互斥锁
    帧3：~fstream()    → 关闭文件
  → 到达目标 catch 块（landing pad）→ 跳转执行
```

**为什么要两阶段**：如果第一阶段发现没有任何 catch，就直接 terminate，不做任何清理。这是 C++ 标准要求的——保留崩溃现场，便于调试。

#### 3. Go：defer 链表遍历

```
panic 触发
  │
  ▼
① 创建 _panic 对象，挂到当前 goroutine 的 panic 链表
  │
  ▼
② 遍历当前 goroutine 的 _defer 链表（后注册的先执行）
  │
  ├── defer 函数内调用了 recover()？
  │     → 停止 panic
  │     → 返回 panic 的值
  │     → 从 defer 注册点之后继续正常执行
  │
  └── defer 函数没调用 recover()？
        → 执行完该 defer，继续下一个
          │
          └── 所有 defer 都执行完仍无 recover
              → fatalpanic → 打印栈 → exit(2)
```

**与 try-catch 的本质区别**：Go 没有"搜索匹配的 handler"这一步。它就是线性遍历 defer 链表，碰到 recover 就停。

#### 4. JavaScript / V8：隐藏类 + 异常栈

```
throw error
  │
  ▼
① V8 创建异常对象，捕获调用栈（Error.stack）
  │
  ▼
② 检查当前执行上下文是否有 try-catch
  │
  ├── 有 → 跳转到 catch 块
  │
  └── 没有 → 弹出当前上下文，检查调用方
        │
        └── 到达栈顶 → 触发 uncaughtException 事件

Promise rejection 是独立机制：
  reject(error)
    │
    ▼
  微任务队列末尾检查：有 .catch() handler 吗？
    ├── 有 → 下一个 microtick 执行 handler
    └── 没有 → 触发 unhandledrejection 事件
```

#### 5. Python / CPython：block 栈

```
// 编译器为每个 try 生成 SETUP_EXCEPT 字节码

字节码执行流：
  SETUP_EXCEPT target=28    // 把 handler 地址压入 block 栈
  ... try 块的代码 ...
  POP_BLOCK                 // 正常结束，弹出 handler

异常发生时（raise 或 C 层 PyErr_SetObject）：
  ① 检查 block 栈顶有没有 EXCEPT handler
  ② 有 → 跳转到 target 地址（except 块）
  ③ 没有 → 弹出当前帧，检查调用方的 block 栈
  ④ 到栈顶 → sys.excepthook → 打印 traceback
```

#### 6. Rust：两种模式

```
模式A：panic = "unwind"（默认）
  → 同 C++ 的 .eh_frame 两阶段展开
  → 逐帧调用 Drop::drop()（Rust 的 RAII）
  → catch_unwind() 可捕获

模式B：panic = "abort"
  → 直接调用 abort()，不做任何展开
  → 二进制更小，无展开表开销
  → 适合嵌入式 / 对体积敏感的场景
```

---

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
