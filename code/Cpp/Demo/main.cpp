// Demo — facility 通用线程库使用示例
//
// 编译运行：
//   cd Cpp/Demo && ./run.sh
// 或者：
//   cmake -B build -DCMAKE_BUILD_TYPE=Release
//   cmake --build build -j
//   ./build/thread_demo
//
// 核心模型：每个 Thread 内部有一个「任务队列」，Post() 把任务塞进队列，
// 线程主循环（RunCtx）不断取出并执行。于是：
//   - 投递到「同一个线程」的任务天然串行执行（FIFO），不需要加锁；
//   - 投递到「不同线程」的任务并行执行（各自跑各自的队列）。
//
// 主要 API 一览：
//   Threads::IotThread()/NetworkThread()/...  -> 取全局命名线程
//   Thread::Post(callable)                    -> 把任务投递到该线程
//   Thread::GetName()/Name()/Id()             -> 线程标识
//   this_thread::Handle()                     -> 在任务内部拿到"当前线程"
//   this_thread::Post(callable)               -> 把任务排到当前线程队列尾部
//   Threads::Pool()->Post(callable)           -> 投递任务到线程池（多工人线程并行）
//   Threads::CreateThread(name)               -> 动态创建线程（调用方持有）
//   Threads::CreateExtensibleThread(name)     -> 可扩展线程（支持任务后回调）
//   ExtensibleThread::SetPluginCtxHandler(fn) -> 设置"每执行完一个任务"的回调

#include "thread/threads.h"

#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

// Demo 自带的极简线程安全日志（用法与 OrderSystem 的 Logger 一致）
struct Logger {
    static std::mutex& mtx() {
        static std::mutex m;
        return m;
    }

    template <typename... Args>
    static void info(Args&&... args) {
        std::lock_guard<std::mutex> lock(mtx());          // 整行原子输出
        std::cout << "[" << std::this_thread::get_id() << "] ";
        (std::cout << ... << std::forward<Args>(args));   // C++17 折叠表达式
        std::cout << "\n";
    }
};

// 工具：把任务投递到 target（Thread 或 ThreadPool 均可），阻塞等待其完成并取回返回值。
// 原理：用 packaged_task 把"返回值回传"这件事故化，future.get() 负责等待。
template <typename Target, typename Func>
auto PostAndWait(Target* target, Func&& func) {
    using R = std::invoke_result_t<Func&>;
    auto task = std::make_shared<std::packaged_task<R()>>(std::forward<Func>(func));
    auto fut  = task->get_future();
    target->Post([task]() { (*task)(); });   // 任务体：执行 packaged_task
    return fut.get();                        // 阻塞直到目标线程执行完（异常也会在此重抛）
}

// ---------------------------------------------------------------------------
// 1. 全局命名线程：Threads::XxxThread()->Post()
// ---------------------------------------------------------------------------
void demo_global_threads() {
    Logger::info("=== 1. 全局命名线程 ===");

    // 取到线程对象后可以查看它的名字和 id
    facility::Thread* iot = facility::Threads::IotThread();
    Logger::info("Iot 线程: name=", iot->GetName(), ", id=", iot->Id());

    // 1.1 不同线程并行工作：任务最终在各自所属的线程上执行
    facility::Threads::NetworkThread()->Post([]() {
        Logger::info("[network] 收到网络包，当前线程 = ", facility::this_thread::Handle()->Name());
    });
    facility::Threads::HeartBeatThread()->Post([]() {
        Logger::info("[heartbeat] 发送心跳，当前线程 = ", facility::this_thread::Handle()->Name());
    });
    facility::Threads::OtaInstallThread()->Post([]() {
        Logger::info("[ota_install] 开始安装固件，当前线程 = ", facility::this_thread::Handle()->Name());
    });

    // 1.2 同一线程串行：投递到 Iot 的 5 个任务严格按 FIFO 依次执行，
    //     所以这里不用任何锁，也不会有数据竞争（顺序一定是 1..5）
    for (int i = 1; i <= 5; ++i) {
        facility::Threads::IotThread()->Post([i]() {
            Logger::info("[iot] 第 ", i, " 个任务");
        });
    }

    // 1.3 需要结果就用 PostAndWait：它等任务跑完并把返回值带回来
    int sum = PostAndWait(iot, []() {
        int s = 0;
        for (int i = 1; i <= 100; ++i) s += i;
        return s;
    });
    Logger::info("Iot 线程上计算 1+2+...+100 = ", sum);
}

// ---------------------------------------------------------------------------
// 2. this_thread：任务内部再投递任务（把后续动作排回当前线程）
// ---------------------------------------------------------------------------
void demo_this_thread() {
    Logger::info("=== 2. this_thread（当前线程上下文）===");

    auto              done = std::make_shared<std::promise<void>>();
    std::future<void> fut  = done->get_future();

    facility::Threads::IotThread()->Post([done]() {
        Logger::info("[iot] 任务 A 开始，当前线程 = ", facility::this_thread::Handle()->Name());
        // 把任务 B 排到"当前线程"队列的尾部，等 A 结束后由同一个线程执行。
        // 这种写法常用于：把耗时的后续步骤拆开，避免长时间占住队列。
        facility::this_thread::Post([done]() {
            Logger::info("[iot] 任务 B（在 A 内排队，同一线程串行执行）");
            done->set_value();
        });
        Logger::info("[iot] 任务 A 结束（B 紧随其后）");
    });

    fut.wait();  // 等 A、B 都执行完
}

// ---------------------------------------------------------------------------
// 3. 线程池：Threads::Pool()，多个工人线程并发消费同一个队列
// ---------------------------------------------------------------------------
void demo_thread_pool() {
    Logger::info("=== 3. 线程池 ===");

    facility::ThreadPool* pool = facility::Threads::Pool();
    Logger::info("线程池工人线程数 = ", facility::ThreadPool::kInitalThreadsNum);

    const int         n       = 8;
    auto              pending = std::make_shared<std::atomic<int>>(n);
    auto              done    = std::make_shared<std::promise<void>>();
    std::future<void> fut     = done->get_future();

    // Post：投递无返回值的任务，8 个任务会被 2 个工人线程并发消费
    for (int i = 0; i < n; ++i) {
        pool->Post([i, pending, done]() {
            Logger::info("[pool] 任务 ", i, " 由线程 ", std::this_thread::get_id(), " 执行");
            std::this_thread::sleep_for(std::chrono::milliseconds(30));  // 模拟耗时
            if (pending->fetch_sub(1) == 1) done->set_value();           // 最后一个任务唤醒主线程
        });
    }
    fut.wait();  // 等 8 个任务全部完成
    Logger::info("[pool] 8 个任务全部完成");
}

// ---------------------------------------------------------------------------
// 4. 动态创建线程：Threads::CreateThread(name)
// ---------------------------------------------------------------------------
void demo_dynamic_thread() {
    Logger::info("=== 4. 动态创建线程 ===");

    // CreateThread 返回裸指针，所有权归调用方 —— 用 unique_ptr 托管，
    // 析构时会自动 Stop() + join()，不会泄漏。
    std::unique_ptr<facility::Thread> thr(facility::Threads::CreateThread("worker"));
    Logger::info("已创建线程 name=", thr->GetName(), ", id=", thr->Id());

    thr->Post([]() { Logger::info("[worker] 任务 1"); });
    thr->Post([]() { Logger::info("[worker] 任务 2"); });

    int r = PostAndWait(thr.get(), []() { return 6 * 7; });
    Logger::info("[worker] 返回结果 = ", r);
}

// ---------------------------------------------------------------------------
// 5. 可扩展线程：ExtensibleThread + SetPluginCtxHandler
//    每执行完一个任务，框架回调 handler —— 适合做插件轮询 / 事件循环
// ---------------------------------------------------------------------------
void demo_extensible_thread() {
    Logger::info("=== 5. 可扩展线程（插件回调）===");

    // 注意：ExtensibleThread 的析构函数是 protected，所以要用基类指针 Thread*
    // 托管（Thread 是虚析构），才能被 unique_ptr 正确释放。
    facility::ExtensibleThread* ext = facility::Threads::CreateExtensibleThread("plugin");
    std::unique_ptr<facility::Thread> thr(ext);
    auto ticks = std::make_shared<std::atomic<int>>(0);

    auto              done = std::make_shared<std::promise<void>>();
    std::future<void> fut  = done->get_future();

    ext->SetPluginCtxHandler([ticks, done]() {
        int n = ticks->fetch_add(1) + 1;
        Logger::info("[plugin] 任务后回调，tick=", n,
                     "，当前线程 = ", facility::this_thread::Handle()->Name());
        if (n == 3) done->set_value();
    });

    for (int i = 1; i <= 3; ++i) {
        ext->Post([i]() { Logger::info("[plugin] 执行业务任务 ", i); });
    }

    fut.wait();  // 等 3 个任务 + 3 次回调执行完
    Logger::info("[plugin] 演示结束");
}

int main() {
    Logger::info("########## facility 线程库用法演示 开始 ##########");
    demo_global_threads();
    demo_this_thread();
    demo_thread_pool();
    demo_dynamic_thread();
    demo_extensible_thread();
    Logger::info("########## facility 线程库用法演示 结束 ##########");
    return 0;
}
