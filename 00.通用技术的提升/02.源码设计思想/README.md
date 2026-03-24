




### 六、异常如何捕获以及捕获原理

#### 6.1 两大底层实现机制

#### 6.2 各语言捕获对比

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

#### 8.2 C++

#### 8.3 JavaScript

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







