





---

### 三、异常如何处理

#### 通用处理模型

```
异常发生
    │
    ▼
┌──────────────────┐
│  查找当前栈帧的    │──找到──▶ 执行 catch/except/recover
│  异常处理器        │         │
└────────┬─────────┘         ▼
         │找不到          处理完成？──是──▶ 继续执行
         ▼                    │
    栈展开(Stack Unwind)       否（重新抛出）
    逐帧向上查找               │
         │                    ▼
         ▼               传播到上层
    到达线程栈顶？
    ├──是──▶ 未捕获异常处理器(UncaughtExceptionHandler)
    │           │
    │           ▼
    │       有handler？──是──▶ 执行handler
    │           │
    │           否
    │           ▼
    │       线程终止（可能导致进程终止）
    └──否──▶ 继续向上展开
```

#### 各语言处理差异

| 处理阶段 | Java | C++ | JavaScript | Python | Go | Rust |
|---------|------|-----|-----------|--------|-----|------|
| 捕获语法 | `try-catch-finally` | `try-catch`+RAII | `try-catch-finally` | `try-except-finally` | `defer-recover` | `Result<T,E>`/`panic-catch_unwind` |
| 资源清理 | `finally`/try-with-resources | RAII 析构函数 | `finally` | `finally`/`with` | `defer` | `Drop` trait (RAII) |
| 未捕获处理 | `UncaughtExceptionHandler` | `std::terminate()` | `unhandledrejection` | `threading.excepthook` | 进程崩溃(打印栈) | 进程 abort |
| 跨线程传递 | `Future.get()` 抛 `ExecutionException` | `std::future` 重抛 | `Promise.catch()` | `Future.result()` 重抛 | channel 传 error | `JoinHandle` 返回 `Result` |

---

### 四、异常整体设计框架

#### 4.1 Java 异常体系（最典型的层次设计）

```
Throwable
├── Error（不应捕获，JVM级）
│   ├── OutOfMemoryError
│   ├── StackOverflowError
│   └── VirtualMachineError
└── Exception
    ├── RuntimeException（Unchecked，运行时）
    │   ├── NullPointerException
    │   ├── IndexOutOfBoundsException
    │   ├── ConcurrentModificationException
    │   └── IllegalStateException
    └── Checked Exception（编译器强制处理）
        ├── IOException
        ├── InterruptedException  ← 线程中断专用
        └── ExecutionException    ← 线程池异常传递专用
```

#### 4.2 C++ 异常体系

```
std::exception
├── std::logic_error
│   ├── std::invalid_argument
│   └── std::out_of_range
├── std::runtime_error
│   ├── std::overflow_error
│   └── std::system_error  ← 线程/系统调用错误
└── 任意类型（C++允许 throw 任何类型，包括 int, string）

信号层（与异常独立）：
SIGSEGV → 非法内存访问
SIGFPE  → 除零
SIGABRT → abort() 调用
```

#### 4.3 设计哲学对比

| 设计理念 | 代表语言 | 核心思路 |
|---------|---------|---------|
| **分层异常体系** | Java、C#、Python | 异常按严重程度分层，checked 强制处理 |
| **值即错误** | Go、Rust | 错误是普通返回值（`error`/`Result<T,E>`），强制在调用处处理 |
| **异常+RAII** | C++ | 异常做控制流转移，RAII 保证资源安全 |
| **事件驱动错误** | JavaScript | 单线程无传统线程异常，通过 Promise rejection 链传播 |
| **panic vs error** | Go、Rust | 区分可恢复错误（error/Result）和不可恢复错误（panic） |

---

### 五、异常可以监听并处理吗

**可以**，每种语言都提供了线程异常监听机制：

| 语言 | 监听机制 | 粒度 | 示例 |
|------|---------|------|------|
| **Java** | `Thread.setUncaughtExceptionHandler()` | 单线程 | `thread.setUncaughtExceptionHandler((t, e) -> log(e))` |
| | `Thread.setDefaultUncaughtExceptionHandler()` | 全局 | Android 常用于全局崩溃捕获 |
| | `ThreadPoolExecutor.afterExecute()` | 线程池级 | 重写钩子方法 |
| **C++** | `std::set_terminate()` | 全局 | 替换默认 terminate handler |
| | 信号处理 `signal(SIGSEGV, handler)` | 进程级 | 捕获段错误等 |
| **JavaScript** | `window.onerror` / `addEventListener('error')` | 全局同步 | 捕获未处理错误 |
| | `window.onunhandledrejection` | 全局异步 | 捕获未处理的 Promise rejection |
| | `worker.onerror` | Worker级 | 监听 Worker 线程异常 |
| **Python** | `threading.excepthook` (3.8+) | 全局线程 | `threading.excepthook = custom_hook` |
| | `sys.excepthook` | 全局主线程 | 替换默认异常打印 |
| **Go** | `defer + recover()` | 当前goroutine | 只能捕获当前goroutine的panic |
| **Rust** | `std::panic::set_hook()` | 全局 | 自定义 panic 信息输出 |
| | `std::panic::catch_unwind()` | 调用级 | 捕获 panic，类似 try-catch |
| **C#** | `AppDomain.UnhandledException` | 应用域 | 全局未处理异常 |
| | `TaskScheduler.UnobservedTaskException` | Task级 | 未观察的 Task 异常 |

---

### 六、异常如何捕获以及捕获原理

#### 6.1 两大底层实现机制

**机制一：基于表的零开销异常（Table-Based / Zero-Cost Exception）—— C++、Rust**

```
编译期：
  编译器为每个函数生成 .eh_frame 表（DWARF格式）
  记录：PC范围 → 清理动作(cleanup) → landing pad(catch块地址)

运行时（异常抛出时）：
  1. throw → __cxa_throw() → libunwind
  2. 从当前 PC 开始，查 .eh_frame 表
  3. 逐帧回溯（unwind），找到匹配的 catch 块
  4. 两阶段：
     Phase 1（搜索）：找到匹配的 handler，不执行清理
     Phase 2（清理）：逐帧执行析构函数/cleanup，跳转到 handler

优点：无异常时零开销（没有额外指令）
缺点：抛异常时开销大（查表+回溯），二进制体积增大
```

**机制二：基于 setjmp/longjmp（传统 C 方式）**

```
try → setjmp() 保存当前寄存器/栈帧到 jmp_buf
throw → longjmp() 恢复到保存点，跳转到 catch 块

优点：实现简单
缺点：try 块有运行时开销（每次都要 setjmp），且无法自动调用析构函数
```

**机制三：语言运行时异常（Java/Python/JS/C#）**

```
JVM 为例：
  1. 每个方法有异常表（Exception Table）：
     [startPC, endPC, handlerPC, catchType]
  2. 异常发生时：
     a. JVM 创建异常对象，记录栈轨迹（fillInStackTrace）
     b. 在当前方法的异常表中查找匹配项
     c. 找到 → 跳转到 handlerPC
     d. 未找到 → 弹出当前帧，在调用方法中继续查找
     e. 到达栈顶仍未找到 → UncaughtExceptionHandler
  3. finally 在编译时被复制到 try 和 catch 的所有出口
```

#### 6.2 各语言捕获原理对比

| 语言 | 底层机制 | 无异常开销 | 抛异常开销 | 栈展开方式 |
|------|---------|-----------|-----------|-----------|
| **Java** | JVM 异常表 + 对象创建 | 近零（只是表存在） | 中（`fillInStackTrace` 最贵） | JVM 内部逐帧回溯 |
| **C++** | .eh_frame 表 + libunwind | 零（zero-cost） | 高（两阶段 unwind） | DWARF 表驱动 |
| **JavaScript** | V8 内部：类似异常表 | 低 | 中 | 引擎内部实现 |
| **Python** | CPython 字节码 + 异常栈 | 低（设置 handler 有开销）| 中 | 解释器逐帧回退 |
| **Go** | `defer` 链表 + `runtime.gopanic` | defer 有微小开销 | 中（遍历 defer 链） | 遍历 G 栈的 defer 链 |
| **Rust** | 同 C++ (.eh_frame) | 零 | 高（同 C++ unwind） | libunwind |
| **C** | 无内建异常 | N/A | N/A | `setjmp/longjmp` 或信号 |

---

### 七、哪些异常是必须抛出的

#### "必须抛出"有两层含义：

**A. 语言强制你处理的（Checked Exception，仅 Java）**

| 异常 | 场景 | 为何强制 |
|------|------|---------|
| `InterruptedException` | `Thread.sleep()`、`wait()`、`BlockingQueue.take()` | 线程中断是协作机制，必须响应 |
| `IOException` | 文件/网络 I/O | I/O 天然不可靠 |
| `ExecutionException` | `Future.get()` | 子线程异常必须被调用方感知 |
| `TimeoutException` | `Future.get(timeout)` | 超时是常见异常路径 |
| `ClassNotFoundException` | 反射加载类 | 类可能不存在 |

**B. 运行时/系统必然抛出的（所有语言）**

| 异常类型 | Java | C++ | JS | Python | Go | Rust |
|---------|------|-----|-----|--------|-----|------|
| 空指针/nil | `NullPointerException` | SIGSEGV(未定义行为) | `TypeError` | `AttributeError` | panic: nil pointer | 编译期禁止 |
| 数组越界 | `IndexOutOfBoundsException` | 未定义行为 | 返回undefined | `IndexError` | panic: index out of range | panic |
| 栈溢出 | `StackOverflowError` | SIGSEGV | `RangeError` | `RecursionError` | runtime: goroutine stack overflow | stack overflow |
| 内存不足 | `OutOfMemoryError` | `std::bad_alloc` | 引擎崩溃 | `MemoryError` | runtime: out of memory | abort |
| 除零 | `ArithmeticException` | SIGFPE(整数) | `Infinity`(浮点) | `ZeroDivisionError` | panic: divide by zero | panic(整数) |
| 类型错误 | `ClassCastException` | `std::bad_cast` | `TypeError` | `TypeError` | 编译期 | 编译期 |
| 死锁 | JVM不检测(挂起) | 不检测(挂起) | 不适用 | 不检测 | runtime检测(fatal) | 不检测 |

---

### 八、每种语言异常处理手段和原理

#### 8.1 Java

```java
// 1. try-catch-finally
try {
    riskyOperation();
} catch (SpecificException e) {
    handle(e);
} finally {
    cleanup(); // 一定执行
}

// 2. try-with-resources（自动关闭 AutoCloseable）
try (var stream = new FileInputStream("f")) {
    // 异常时自动调用 stream.close()
}

// 3. 线程未捕获异常
thread.setUncaughtExceptionHandler((t, e) -> {
    log.error("Thread " + t.getName() + " died", e);
});

// 4. 线程池异常：submit 的异常封装在 Future 中
Future<?> f = executor.submit(task);
try {
    f.get(); // 这里抛出 ExecutionException
} catch (ExecutionException e) {
    Throwable cause = e.getCause(); // 原始异常
}

// 5. CompletableFuture 异常链
CompletableFuture.supplyAsync(() -> riskyOp())
    .exceptionally(ex -> fallback)
    .thenAccept(result -> use(result));
```

**原理**：JVM 异常表驱动。`throw` → 创建异常对象（最耗时的是 `fillInStackTrace()` 遍历所有栈帧）→ 查异常表 → 匹配 catch type（支持继承匹配）→ 跳转。`finally` 在编译时被复制插入到所有正常/异常出口。

#### 8.2 C++

```cpp
// 1. try-catch
try {
    throw std::runtime_error("fail");
} catch (const std::runtime_error& e) {
    // 处理
} catch (...) {
    // 捕获所有
}

// 2. RAII 是核心（比 finally 更强大）
{
    std::lock_guard<std::mutex> lock(mtx); // 异常时析构函数自动释放锁
    std::unique_ptr<Foo> p(new Foo());      // 异常时自动 delete
    // 即使这里 throw，lock 和 p 都会被正确释放
}

// 3. noexcept 声明（C++11）
void safe_func() noexcept {
    // 如果这里 throw，直接 std::terminate()，不做栈展开
}

// 4. 线程异常：std::thread 未捕获 → std::terminate()
// 必须在线程函数内自行 catch
std::thread t([]() {
    try {
        throw std::runtime_error("oops");
    } catch (...) {
        // 必须自己处理，否则进程终止
    }
});

// 5. 通过 future 跨线程传递异常
auto fut = std::async(std::launch::async, []() -> int {
    throw std::runtime_error("async error");
});
try {
    fut.get(); // 重新抛出异常
} catch (const std::runtime_error& e) {
    // 在调用线程捕获子线程异常
}

// 6. exception_ptr 手动传递
std::exception_ptr eptr;
std::thread t([&eptr]() {
    try { throw std::runtime_error("err"); }
    catch (...) { eptr = std::current_exception(); }
});
t.join();
if (eptr) std::rethrow_exception(eptr); // 主线程重抛
```

**原理**：两阶段栈展开。Phase1：从 throw 点沿调用链查 `.eh_frame` 表，找匹配的 catch（类型匹配用 RTTI `type_info`）。Phase2：逐帧调用析构函数（RAII 的保障），最终跳转到 catch 块（landing pad）。`noexcept` 函数抛异常则直接调 `std::terminate()`，不展开。

#### 8.3 JavaScript

```javascript
// 1. try-catch-finally
try {
    JSON.parse(invalid);
} catch (e) {
    if (e instanceof SyntaxError) { /* ... */ }
} finally {
    cleanup();
}

// 2. Promise 异常链（关键！JS 线程异常的主要形式）
fetch(url)
    .then(resp => resp.json())
    .catch(err => console.error(err)); // 捕获链上任意一环的异常

// 3. async/await + try-catch
async function work() {
    try {
        const data = await fetch(url);
    } catch (e) {
        // 捕获异步异常
    }
}

// 4. 全局未捕获
window.addEventListener('error', (e) => { /* 同步错误 */ });
window.addEventListener('unhandledrejection', (e) => { /* Promise rejection */ });

// 5. Web Worker 异常
const worker = new Worker('task.js');
worker.onerror = (e) => {
    console.error('Worker error:', e.message);
    // Worker 内部未捕获的异常会触发这里
};

// 6. Node.js
process.on('uncaughtException', (err) => { /* 同步 */ });
process.on('unhandledRejection', (reason) => { /* Promise */ });
```

**原理**：V8 引擎内部维护 try-catch 栈。`throw` → 沿执行上下文栈查找 handler。Promise rejection 是独立机制：rejection 存入微任务队列，若在当前 tick 结束时无 `.catch()` 处理，触发 `unhandledrejection`。**JS 单线程模型意味着没有传统线程异常问题，异步异常通过 Promise 链传播。**

#### 8.4 Python

```python
# 1. try-except-else-finally
try:
    result = risky()
except ValueError as e:
    handle(e)
except (TypeError, KeyError):
    pass
else:
    # 无异常时执行
finally:
    cleanup()

# 2. with 语句（上下文管理器，类似 RAII）
with open('file') as f:  # 异常时自动调用 f.__exit__()
    data = f.read()

# 3. 线程异常
import threading

def worker():
    raise RuntimeError("thread error")

t = threading.Thread(target=worker)
t.start()
t.join()
# 默认：异常打印到 stderr，主线程无感知！

# 4. 线程异常钩子（Python 3.8+）
def custom_hook(args):
    print(f"Thread {args.thread.name} exception: {args.exc_value}")
threading.excepthook = custom_hook

# 5. concurrent.futures 跨线程传递
from concurrent.futures import ThreadPoolExecutor
with ThreadPoolExecutor() as pool:
    future = pool.submit(risky_func)
    try:
        result = future.result()  # 子线程异常在这里重新抛出
    except Exception as e:
        handle(e)

# 6. 异常链
try:
    connect()
except ConnectionError as e:
    raise RuntimeError("failed") from e  # __cause__ 链
```

**原理**：CPython 解释器为每个 `try` 块在字节码中插入 `SETUP_EXCEPT` 指令，将 handler 地址压入 block 栈。异常发生时（`raise` 或 C 层 `PyErr_SetObject`），解释器从 block 栈弹出 handler 跳转。**GIL 确保同一时刻只有一个线程执行字节码，所以异常处理本身是线程安全的**。线程异常默认不传播到其他线程，`excepthook` 是唯一的全局监听点。

#### 8.5 Go

```go
// Go 没有 try-catch，区分 error（可恢复）和 panic（不可恢复）

// 1. error 返回值（正常错误处理，推荐方式）
result, err := doWork()
if err != nil {
    return fmt.Errorf("doWork failed: %w", err) // 错误包装
}

// 2. defer-panic-recover（类似 try-catch，仅用于真正的异常）
func safeGoroutine() {
    defer func() {
        if r := recover(); r != nil {
            log.Printf("recovered panic: %v", r)
            debug.PrintStack()
        }
    }()
    panic("something terrible") // 触发 panic
}

// 3. goroutine 异常：未 recover 的 panic 导致整个进程崩溃！
go func() {
    // 如果这里 panic 且没有 recover，整个程序退出
    // 必须在每个 goroutine 内部 defer-recover
    defer func() { recover() }()
    riskyWork()
}()

// 4. 通过 channel 传递错误
errCh := make(chan error, 1)
go func() {
    defer func() {
        if r := recover(); r != nil {
            errCh <- fmt.Errorf("panic: %v", r)
        }
    }()
    errCh <- doWork()
}()
if err := <-errCh; err != nil {
    // 主 goroutine 处理子 goroutine 的错误
}

// 5. errgroup（官方推荐的并发错误处理）
g, ctx := errgroup.WithContext(context.Background())
g.Go(func() error { return task1(ctx) })
g.Go(func() error { return task2(ctx) })
if err := g.Wait(); err != nil {
    // 任一 goroutine 返回 error，这里都能收到
}
```

**原理**：`panic` 调用 `runtime.gopanic()`，沿当前 goroutine 的 `_defer` 链表逆序执行 defer 函数。若某个 defer 调用了 `recover()`，`gopanic` 停止展开，恢复正常执行。若遍历完所有 defer 仍无 `recover`，调用 `runtime.fatalpanic()` → 打印所有 goroutine 栈 → `exit(2)`。**关键：recover 只能捕获当前 goroutine 的 panic，无法跨 goroutine。**

#### 8.6 Rust

```rust
// Rust 区分两层：Result（可恢复）和 panic（不可恢复）

// 1. Result<T, E>（主要错误处理方式）
fn read_file() -> Result<String, io::Error> {
    let content = std::fs::read_to_string("file.txt")?; // ? 操作符自动传播
    Ok(content)
}

// 2. ? 操作符链式传播
fn process() -> Result<Data, Box<dyn Error>> {
    let raw = read_file()?;          // 错误自动 return Err
    let parsed = parse(raw)?;
    Ok(transform(parsed))
}

// 3. panic（不可恢复，类似 abort）
// 数组越界、unwrap() 失败等会 panic

// 4. catch_unwind 捕获 panic（类似 try-catch，少用）
use std::panic;
let result = panic::catch_unwind(|| {
    panic!("boom");
});
match result {
    Ok(val) => println!("ok: {}", val),
    Err(_) => println!("caught a panic"),
}

// 5. 线程 panic：不会影响其他线程
let handle = std::thread::spawn(|| {
    panic!("thread panic");
});
match handle.join() {
    Ok(_) => println!("thread ok"),
    Err(e) => println!("thread panicked: {:?}", e), // 在这里捕获
}

// 6. 自定义 panic hook
panic::set_hook(Box::new(|info| {
    eprintln!("Custom panic: {}", info);
}));
```

**原理**：`Result<T,E>` 是普通枚举，零运行时开销，编译器强制处理（不处理有 warning）。`panic!` 触发栈展开（同 C++ 的 `.eh_frame` 机制）或直接 abort（可通过 `panic = "abort"` 配置）。**Rust 的 `Send`/`Sync` trait 在编译期确保跨线程数据安全，从根源上消除了大量线程异常场景（如数据竞争）。**

#### 8.7 C

```c
// C 没有内建异常机制，完全依赖以下手段：

// 1. 返回错误码（最基本）
int result = open("file", O_RDONLY);
if (result == -1) {
    perror("open failed");  // errno 全局变量
}

// 2. setjmp/longjmp（模拟 try-catch）
#include <setjmp.h>
jmp_buf env;
if (setjmp(env) == 0) {
    // "try" 块
    if (error) longjmp(env, 1);  // "throw"
} else {
    // "catch" 块
}
// 问题：不会调用析构函数，不会释放资源

// 3. 信号处理（硬件异常）
#include <signal.h>
void handler(int sig) {
    // SIGSEGV, SIGFPE, SIGABRT 等
    // 注意：信号处理函数内能做的事非常有限（async-signal-safe）
}
signal(SIGSEGV, handler);

// 4. 线程取消
pthread_cancel(thread);  // 发送取消请求
// 被取消线程在取消点（cancellation point）响应
// cleanup handler：
pthread_cleanup_push(cleanup_func, arg);
// ... work ...
pthread_cleanup_pop(1);  // 执行清理
```

**原理**：C 在语言层面不支持异常。`setjmp` 保存 CPU 寄存器和栈指针到 `jmp_buf`，`longjmp` 恢复，直接跳转回保存点。**致命缺陷：不执行中间帧的任何清理代码**。信号处理基于 OS 内核：硬件异常 → CPU 陷入内核 → 内核向进程发信号 → 用户态信号处理函数执行。线程取消通过 `pthread_cancel` 设置标志位，线程在下一个取消点检查并响应。

---

### 核心设计总结

| 设计维度 | 传统异常派(Java/C++/C#/Python) | 值错误派(Go/Rust) |
|---------|------------------------------|------------------|
| 错误传递 | 抛出异常对象，沿栈展开 | 返回 error/Result，调用方显式处理 |
| 强制处理 | Java checked exception | Go `if err != nil`（惯例）；Rust `Result` 编译器 warning |
| 不可恢复 | Error/terminate/abort | panic（Go 崩进程，Rust 可 catch_unwind） |
| 跨线程 | Future 封装重抛 | channel 传 error / JoinHandle 返回 Result |
| 资源清理 | finally / RAII / with | defer / Drop(RAII) |
| 性能哲学 | 无异常时低开销，抛异常时可接受 | 零异常开销（错误就是普通返回值） |







