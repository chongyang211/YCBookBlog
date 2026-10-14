#pragma once

#include "BlockingQueue.h"   // 复用阻塞队列作任务队列（header-only 组件链）
#include <thread>            // std::thread：工人线程
#include <vector>            // std::vector：工人列表
#include <functional>        // std::function：类型擦除的任务包装
#include <future>            // packaged_task / future：异步结果回传
#include <atomic>            // std::atomic<bool>：停机标志
#include <memory>            // std::make_shared / shared_ptr

// ============================================================================
// 【组件定位】ThreadPool —— 通用线程池：固定工人 + 阻塞任务队列 + future 回传
// ----------------------------------------------------------------------------
// 【设计思路】
//   1. 为什么不用"每任务一个 thread"：线程创建/销毁开销大（栈分配、内核
//      登记），万级请求会直接资源耗尽 —— 池化复用是工业标配。
//   2. 任务队列装 std::function<void()>：把"任意签名"的任务擦除成统一
//      "无参无返回"形态，队列只需一个模板参数。
//   3. submit 的三大坑（教程 Step 2.3）：packaged_task 不可拷贝 / future
//      必须在 push 前拿 / shared_ptr 桥接 —— 见函数内注释。
//   4. 生命周期：析构自动 shutdown + join —— 忘 join 的线程会在对象
//      析构后访问悬空 this（UB），RAII 强制收尾。
// ============================================================================

// 通用线程池：固定数量的工人线程 + 阻塞任务队列
// submit 返回 std::future，调用方可以异步拿结果
class ThreadPool {
private:
    std::vector<std::thread>             workers;
    // 【为什么存 thread 而非 jthread】教学环境 C++17（jthread 是 C++20）；
    // 手动 join 的责任由析构函数承担
    BlockingQueue<std::function<void()>> tasks;     // 任务 = 无返回的函数
    // 【类型擦除】任何 lambda/bind/函数指针都塞进 function<void()>：
    // 队列只见"可调用物"，不关心原签名 —— 模板 submit 到普通队列的桥。
    std::atomic<bool>                    stopped{false};
    // atomic 的原因：shutdown 可能被多线程同时调（用户调 + 析构调），
    // exchange 是原子的"读旧值写新值"，防重复关停竞态。

public:
    // 默认工人数量 = hardware_concurrency（CPU 核数：本机最优起点）
    explicit ThreadPool(size_t n = std::thread::hardware_concurrency()) {
        for (size_t i = 0; i < n; ++i) {
            workers.emplace_back([this]{
                // ===== 工人主循环：取活 → 干活 → 循环 =====
                while (true) {
                    auto task = tasks.pop();        // 阻塞等活（无活时挂起，不耗 CPU）
                    if (!task) break;               // shutdown 信号
                    // pop 返回 nullopt 仅在"队列空且已停机"—— 工人的下班暗号
                    try { (*task)(); }
                    // task 是 optional<function>，* 解出 function 再调用
                    catch (...) { /* 工作线程必须吞异常 */ }
                    // 【铁律】异常若从工人线程函数逃出 → std::terminate 全进程崩溃。
                    // 任务里的异常应经 packaged_task 存进 future（调用方
                    // get() 时重抛）—— 这里吞掉的是"未经 future 通道"的漏网异常。
                }
            });
        }
    }

    // RAII：析构自动优雅停机（等所有工人结束）—— 使用方忘调 shutdown 也不泄漏线程
    ~ThreadPool() { shutdown(); }

    // ⭐ 万能 submit：接受任意可调用对象 + 任意参数，返回 future<R>
    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>
    // 【返回类型】invoke_result_t<F, Args...>：编译期推导"f 以 args 调用"的
    // 返回类型（比 C++11 result_of 更准，能正确处理重载 operator() 的 lambda）
    {
        using R = std::invoke_result_t<F, Args...>;

        // 1. 把 f + args 打包成 packaged_task
        //    用 shared_ptr 是因为 packaged_task 不可拷贝（只能移动），
        //    但 std::function 要求 callable 可拷贝 → 套一层 shared_ptr 解决
        auto task = std::make_shared<std::packaged_task<R()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        // 【三大坑逐个看】
        //  坑① packaged_task 只可移动（内部持 promise，promise 独占承诺权）——
        //     直接塞 function<void()> 编译不过（function 要求可拷贝）。
        //     解法：shared_ptr 持有 task（shared_ptr 本身可拷贝），lambda 拷贝
        //     的只是"指针"，任务本体始终一份 —— 桥接"只移动"与"要拷贝"。
        //  坑② bind + forward：万能引用完美转发 —— 左值拷贝、右值移动，
        //     参数的 const/引用属性全程保留（写成 F f 就退化成拷贝了）。
        //  坑③ R() 签名：bind 把"f(args...)"固化成"无参可调用"，
        //     packaged_task<R()> 的 R 就是调用方要的返回类型。

        // 2. 提前拿到 future（必须在 push 之前）
        std::future<R> fut = task->get_future();
        // 【顺序铁律】若先 push 后 get_future：工人可能瞬间把 task 执行完并
        // 销毁（shared_ptr 归零）—— get_future 就悬空了。先拿 future 再发任务，
        // "凭据"永远在手。

        // 3. 把"调用 task 的 lambda"扔进队列
        tasks.push([task]{ (*task)(); });
        // lambda 按值捕 shared_ptr（引用计数 +1）：队列里的任务与 submit 返回
        // 后的局部 task 共享所有权 —— 谁后释放谁收尾，task 生命周期自动管理。
        return fut;
    }

    void shutdown() {
        if (stopped.exchange(true)) return;     // 防重复 shutdown
        // exchange：原子地"读旧值 + 写 true"——两个线程同时调 shutdown，
        // 只有第一个拿到 false 继续关停，第二个见旧值 true 直接返回（幂等）。
        tasks.shutdown();       // 队列置停机标志 + 叫醒所有睡眠工人
        for (auto& w : workers) {
            if (w.joinable()) w.join();
            // joinable 检查：防对已 join/未 start 的线程重复 join（UB）；
            // 必须等工人把"存量任务排干"后自然退出（BlockingQueue 的温和停机语义）
        }
    }

    size_t pending() const { return tasks.size(); }    // 积压任务数（背压观测口）
};
