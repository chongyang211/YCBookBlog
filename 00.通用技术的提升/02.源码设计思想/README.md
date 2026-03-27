









---

### 3. 信号量（std::counting_semaphore / C++20）

**原理**：内部维护一个计数器（基于 futex/原子操作）。`acquire()` 计数减 1（为 0 则阻塞），`release()` 计数加 1 并唤醒等待者。用于限制并发访问数。

```cpp
#include <semaphore>
#include <thread>
#include <vector>
#include <iostream>

std::counting_semaphore<3> sem(3);  // 最多3个线程同时执行

void worker(int id) {
  sem.acquire();       // 获取许可，超过3个则阻塞
  std::cout << "Thread " << id << " working\n";
  std::this_thread::sleep_for(std::chrono::seconds(1));
  sem.release();       // 归还许可
}

int main() {
  std::vector<std::thread> threads;
  for (int i = 0; i < 10; ++i) {
    threads.emplace_back(worker, i);
  }
  for (auto& t : threads) t.join();
}
```

**C++17 替代**：用 `mutex + condition_variable + int counter` 手动实现。

---

### 4. 屏障（std::barrier / C++20）

**原理**：内部用原子计数器 + futex。每个线程调用 `arrive_and_wait()` 时计数减 1 并阻塞，最后一个到达时计数归零，唤醒所有线程并执行回调，然后重置计数进入下一轮。

```cpp
#include <barrier>
#include <thread>
#include <vector>
#include <iostream>

std::barrier sync_point(3, [] noexcept {
  std::cout << "--- All threads arrived, new phase ---\n";  // 每轮最后一个到达时执行
});

void worker(int id) {
  for (int phase = 0; phase < 2; ++phase) {
    std::cout << "Thread " << id << " phase " << phase << " done\n";
    sync_point.arrive_and_wait();  // 阻塞直到3个线程都到
  }
}

int main() {
  std::vector<std::thread> threads;
  for (int i = 0; i < 3; ++i) {
    threads.emplace_back(worker, i);
  }
  for (auto& t : threads) t.join();
}
```

**vs latch**：`std::latch` 一次性（CountDownLatch），`std::barrier` 可重复使用。

---

### 5. 消息队列（手动实现线程安全队列）

**原理**：用 `mutex` 保护队列 + `condition_variable` 做阻塞通知。`push` 加锁入队并通知，`pop` 加锁等待直到队列非空。经典生产者-消费者模式。

```cpp
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template <typename T>
class MessageQueue {
 public:
  void Push(T value) {
    std::lock_guard<std::mutex> lk(mtx_);
    queue_.push(std::move(value));
    cv_.notify_one();
  }

  T Pop() {
    std::unique_lock<std::mutex> lk(mtx_);
    cv_.wait(lk, [this] { return !queue_.empty(); });
    T val = std::move(queue_.front());
    queue_.pop();
    return val;
  }

 private:
  std::queue<T> queue_;
  std::mutex mtx_;
  std::condition_variable cv_;
};

// 生产者: queue.Push(42);
// 消费者: int val = queue.Pop();  // 空时阻塞
```

---

### 6. 管道（pipe / socketpair — POSIX）

**原理**：管道是内核中的**环形缓冲区**（默认 64KB）。`write()` 向缓冲区写入，`read()` 从缓冲区读取。缓冲区满时 `write` 阻塞，空时 `read` 阻塞。跨进程通信的基础。

```cpp
#include <unistd.h>
#include <thread>
#include <iostream>
#include <cstring>

int main() {
  int pipefd[2];
  pipe(pipefd);  // pipefd[0]=读端, pipefd[1]=写端

  std::thread writer([&] {
    const char* msg = "hello from writer";
    write(pipefd[1], msg, strlen(msg) + 1);
    close(pipefd[1]);
  });

  std::thread reader([&] {
    char buf[64];
    read(pipefd[0], buf, sizeof(buf));  // 阻塞直到有数据
    std::cout << "Received: " << buf << "\n";
    close(pipefd[0]);
  });

  writer.join();
  reader.join();
}
```

**适用**：线程间也可用，但更常见于父子进程通信。线程间通常优先用消息队列。

---

### 7. 共享内存（POSIX shm / mmap）

**原理**：多个进程/线程映射**同一块物理内存**到各自虚拟地址空间，直接读写无需内核拷贝。是 IPC 中最快的方式，但需要额外同步机制（mutex/semaphore）保护。

```cpp
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <atomic>
#include <iostream>

struct SharedData {
  std::atomic<bool> ready{false};
  char message[256];
};

int main() {
  // 线程间共享（也可用 shm_open 跨进程）
  auto* shared = new SharedData();

  std::thread writer([shared] {
    strcpy(shared->message, "data via shared memory");
    shared->ready.store(true, std::memory_order_release);
  });

  std::thread reader([shared] {
    while (!shared->ready.load(std::memory_order_acquire)) {}
    std::cout << "Read: " << shared->message << "\n";
  });

  writer.join();
  reader.join();
  delete shared;
}
```

**跨进程版本**：用 `shm_open` + `mmap` 创建命名共享内存段，配合 `sem_open` 做进程间同步。

---

### 8. 信号（signal / sigaction）

**原理**：信号是 OS 级别的**异步通知**机制。内核在进程返回用户态时检查信号队列，有待处理信号则中断当前执行流，跳转到信号处理函数。处理完后恢复原执行流。

```cpp
#include <csignal>
#include <atomic>
#include <thread>
#include <iostream>

std::atomic<bool> should_stop{false};

void SignalHandler(int sig) {
  should_stop.store(true, std::memory_order_relaxed);  // 信号处理函数中只能做async-signal-safe操作
}

int main() {
  signal(SIGUSR1, SignalHandler);

  std::thread worker([] {
    while (!should_stop.load(std::memory_order_relaxed)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "Worker stopped by signal\n";
  });

  // 给自己发信号
  std::this_thread::sleep_for(std::chrono::seconds(1));
  raise(SIGUSR1);  // 或 pthread_kill(worker.native_handle(), SIGUSR1)
  worker.join();
}
```

**注意**：信号处理函数中**只能调用 async-signal-safe 函数**，不能用 `malloc`、`printf`、`mutex` 等。通常只设置一个 `atomic` 标志。

---

### 9. 事件（eventfd — Linux / 手动实现事件对象）

**原理**：`eventfd` 是内核提供的轻量级通知机制，内部维护一个 64 位计数器。`write` 加值，`read` 取值并归零（`EFD_SEMAPHORE` 模式每次减 1）。可配合 `epoll` 实现高效事件驱动。

```cpp
#include <sys/eventfd.h>
#include <unistd.h>
#include <thread>
#include <iostream>

int main() {
  int efd = eventfd(0, 0);  // 初始计数为0

  std::thread notifier([efd] {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    uint64_t val = 1;
    write(efd, &val, sizeof(val));  // 通知：计数+1
    std::cout << "Event sent\n";
  });

  std::thread waiter([efd] {
    uint64_t val;
    read(efd, &val, sizeof(val));   // 阻塞直到计数>0，读取后归零
    std::cout << "Event received, val=" << val << "\n";
  });

  notifier.join();
  waiter.join();
  close(efd);
}
```

**跨平台替代**：用 `condition_variable` + `bool flag` 实现等效事件对象。

---

### 10. 线程局部存储（thread_local）

**原理**：编译器为每个线程分配独立的变量副本。底层通过 TLS（Thread-Local Storage）段实现——每个线程有自己的 TLS 块，通过 `fs`/`gs` 段寄存器快速索引。**不需要同步**，天然线程安全。

```cpp
#include <thread>
#include <iostream>

thread_local int request_id = 0;  // 每个线程有自己的副本

void HandleRequest(int id) {
  request_id = id;             // 只影响当前线程
  // 后续调用链中任何地方都可以直接读取 request_id
  std::cout << "Thread " << std::this_thread::get_id()
            << " request_id=" << request_id << "\n";
}

int main() {
  std::thread t1(HandleRequest, 100);
  std::thread t2(HandleRequest, 200);
  t1.join();
  t2.join();
  // 输出各自的值，互不影响
}
```

**适用**：线程上下文（请求 ID、日志 context）、线程缓存（避免竞争）。

---

### 11. 读写锁（std::shared_mutex / C++17）

**原理**：内部用原子计数器区分读者和写者。多个读者可以同时持有共享锁（读锁），写者独占。写锁等待所有读者释放后获取。适合**读多写少**场景。

```cpp
#include <shared_mutex>
#include <thread>
#include <vector>
#include <iostream>

std::shared_mutex rw_mtx;
int shared_data = 0;

void reader(int id) {
  std::shared_lock<std::shared_mutex> lk(rw_mtx);  // 共享锁，多个reader并行
  std::cout << "Reader " << id << " reads: " << shared_data << "\n";
}

void writer(int val) {
  std::unique_lock<std::shared_mutex> lk(rw_mtx);  // 独占锁，排斥所有reader和writer
  shared_data = val;
  std::cout << "Writer set: " << val << "\n";
}

int main() {
  std::vector<std::thread> threads;
  threads.emplace_back(writer, 42);
  for (int i = 0; i < 5; ++i) {
    threads.emplace_back(reader, i);
  }
  for (auto& t : threads) t.join();
}
```

**vs mutex**：`mutex` 读写互斥，`shared_mutex` 读读并行。读多写少时性能优势明显。

---

### 全景总结

| 机制 | 底层原理 | 适用场景 | 标准 |
|------|---------|---------|------|
| mutex + condition_variable | pthread_mutex + pthread_cond (futex) | 通用锁通信、等待/唤醒 | C++11 |
| std::atomic | CPU CAS 指令 + 内存屏障 | 标志位、计数器、无锁数据结构 | C++11 |
| std::counting_semaphore | futex / 原子操作 | 限流、资源池 | C++20 |
| std::barrier / std::latch | 原子计数 + futex | 多线程同步汇合点 | C++20 |
| 消息队列（手动） | mutex + condition_variable + queue | 生产者-消费者 | C++11 |
| pipe | 内核环形缓冲区 | 进程/线程间字节流 | POSIX |
| 共享内存 (shm/mmap) | 物理内存直接映射 | 高性能大数据共享 | POSIX |
| signal | 内核信号队列 + 中断用户态 | 异步通知（优雅退出等） | POSIX |
| eventfd | 内核 64 位计数器 | 轻量事件通知、配合 epoll | Linux |
| thread_local | TLS 段 + 段寄存器索引 | 线程隔离数据（无需同步） | C++11 |
| shared_mutex | 原子计数器（读者计数+写者标志） | 读多写少的共享资源保护 | C++17 |

**依赖链**：

```
应用层:  消息队列 / barrier / semaphore / 读写锁
           │
           ▼
同步原语:  mutex + condition_variable / atomic
           │
           ▼
OS层:      futex (Linux) / pthread_mutex + pthread_cond
           │
           ▼
硬件层:    CAS 指令 (cmpxchg) + 内存屏障 (mfence/lfence/sfence)
```
