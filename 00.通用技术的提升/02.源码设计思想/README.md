



## 3. 两类上下文切换

### 3.1 进程级切换（重量级）

### 3.2 线程级切换（轻量级）

## 4. 底层实现（以Linux x86-64为例）

Linux内核中，上下文切换的核心在 `__switch_to` 函数：

```
schedule() 
  → context_switch()
    → switch_mm()          // 切换内存描述符（进程切换时换页表）
    → switch_to()          // 切换寄存器上下文
      → __switch_to()      // 架构相关的汇编实现
```

关键步骤的伪汇编（x86-64简化版）：

```asm
; 保存prev任务的寄存器到其task_struct->thread
mov [prev + THREAD_RSP], rsp    ; 保存栈指针
mov [prev + THREAD_RBP], rbp    ; 保存帧指针
mov [prev + THREAD_RBX], rbx    ; 保存callee-saved寄存器
; ... 保存r12-r15等

; 恢复next任务的寄存器
mov rsp, [next + THREAD_RSP]    ; 恢复栈指针
mov rbp, [next + THREAD_RBP]
mov rbx, [next + THREAD_RBX]
; ... 恢复r12-r15等

; 切换完成，从next的栈上返回，等价于跳转到next上次被切走的位置
ret
```

这里有个精妙之处：**`ret`指令从新栈上弹出返回地址**，而这个地址正是next线程上次调用switch_to时保存的返回点，所以next线程就像"从sleep中醒来"一样从上次中断处继续执行。

### 页表切换（进程切换时）

```c
// switch_mm() 简化逻辑
void switch_mm(struct mm_struct *prev_mm, struct mm_struct *next_mm) {
    if (prev_mm != next_mm) {
        // 写入CR3寄存器，硬件自动刷新TLB
        load_cr3(next_mm->pgd);  
    }
    // PCID优化：现代CPU支持给TLB条目打标签，避免全量刷新
}
```

## 5. 切换时机（何时触发）

| 触发条件 | 类型 | 说明 |
|----------|------|------|
| **时间片耗尽** | 抢占式 | 时钟中断触发，调度器决定切换 |
| **系统调用阻塞** | 自愿 | read/write/sleep/mutex_lock等，主动让出CPU |
| **中断处理返回** | 抢占式 | 中断处理完后检查是否需要调度 |
| **高优先级任务就绪** | 抢占式 | 实时任务被唤醒，抢占当前任务 |
| **yield** | 自愿 | 主动让出（如sched_yield、Java的Thread.yield） |

## 6. 各语言的实现差异

### C/C++：直接映射OS线程

```cpp
// C++ std::thread → pthread → OS内核线程
// 上下文切换完全由操作系统内核完成
std::thread t([]{ /* ... */ });
// 线程切换 = 内核态 context_switch()
```

C/C++的线程是1:1模型（一个语言线程对应一个内核线程），切换开销完全取决于OS。

### Java：OS线程 + JVM层

```java
// Java线程 = OS原生线程（HotSpot实现）
// 切换时除了OS寄存器，JVM还需要处理：
// - JIT编译后的本地栈帧
// - GC安全点（safepoint）检查
// - monitor锁状态
Thread t = new Thread(() -> { /* ... */ });
```

Java 21引入了**虚拟线程（Virtual Thread）**，采用M:N模型：
- 大量虚拟线程映射到少量平台线程
- 虚拟线程阻塞时，JVM在用户态将其"卸载"，换另一个虚拟线程上来
- 本质上是**用户态上下文切换**，避免了内核态切换的开销

### JavaScript：单线程 + 事件循环（无传统切换）

```javascript
// JS是单线程模型，没有传统意义的上下文切换
// 通过事件循环(event loop) + 回调/Promise/async-await实现并发
async function fetchData() {
    const data = await fetch(url);  // 不阻塞线程，注册回调
    process(data);                   // 回调时从这里继续
}
```

JS的"切换"发生在**协作式调度**层面：
- async/await的suspend/resume由引擎（V8）管理
- 不涉及CPU寄存器保存，而是保存**闭包+执行位置**
- 本质是**状态机转换**，编译器将async函数拆分成多段

### Go：goroutine（用户态调度）

```go
// goroutine = 用户态轻量级线程
// Go运行时实现M:N调度（M个goroutine映射到N个OS线程）
go func() { /* ... */ }()
```

Go的上下文切换极其轻量：
- 只保存少量寄存器（PC、SP和少数callee-saved）
- goroutine栈初始只有几KB（OS线程通常1-8MB）
- 调度器在用户态运行，无需陷入内核

## 7. 性能开销量化

| 切换类型 | 典型耗时 | 主要开销来源 |
|----------|---------|-------------|
| 进程切换 | 3-10μs | TLB刷新 + 缓存失效 + 寄存器保存 |
| 线程切换 | 1-5μs | 寄存器保存 + 内核态切换 |
| goroutine切换 | ~200ns | 少量寄存器 + 用户态调度 |
| 协程/虚拟线程 | 50-200ns | 栈帧保存 + 调度 |
| JS async切换 | ~10ns | 状态机跳转（无真正的栈切换） |

**隐性开销**往往比直接开销更大：
- **CPU缓存污染**：切换后的任务访问的数据不在L1/L2/L3缓存中
- **TLB miss**：进程切换后虚拟地址翻译变慢
- **分支预测器失效**：新任务的分支模式不同

## 8. 设计思想演进

```
内核态线程切换（重）
    ↓ 开销太大
用户态线程/协程（轻）
    ↓ 需要配合异步IO
M:N混合调度（Go/Java虚拟线程）
    ↓ 进一步优化
事件驱动/状态机（JS/Rust async）
    ↓ 编译器参与
无栈协程（零分配，编译时转状态机）
```

核心设计思想：
1. **分治**：将"保存/恢复全部CPU状态"这个重操作，按需裁剪到最小集合
2. **分层**：内核管物理资源，运行时管逻辑并发，语言管抽象模型
3. **协作优于抢占**：能协作式调度（yield/await）就不用时钟中断抢占，减少不必要的切换
4. **减少共享**：从共享内存（需要锁）→ 消息传递（CSP/Actor），从根源上减少切换引起的竞争

## 9. 总结

上下文切换的本质是：**在有限的物理执行资源上，通过时分复用支持逻辑上的并发，代价是保存和恢复执行状态**。整个技术演进的方向就是不断降低这个代价——从进程到线程到协程到状态机，保存的状态越来越少，切换越来越快，最终趋向于零开销抽象。




