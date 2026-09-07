#pragma once

#include "BlockingQueue.h"
#include <thread>
#include <vector>
#include <functional>
#include <future>
#include <atomic>
#include <memory>

// 通用线程池：固定数量的工人线程 + 阻塞任务队列
// submit 返回 std::future，调用方可以异步拿结果
class ThreadPool {
private:
    std::vector<std::thread>             workers;
    BlockingQueue<std::function<void()>> tasks;     // 任务 = 无返回的函数
    std::atomic<bool>                    stopped{false};

public:
    explicit ThreadPool(size_t n = std::thread::hardware_concurrency()) {
        for (size_t i = 0; i < n; ++i) {
            workers.emplace_back([this]{
                while (true) {
                    auto task = tasks.pop();
                    if (!task) break;               // shutdown 信号
                    try { (*task)(); }
                    catch (...) { /* 工作线程必须吞异常 */ }
                }
            });
        }
    }

    ~ThreadPool() { shutdown(); }

    // ⭐ 万能 submit：接受任意可调用对象 + 任意参数，返回 future<R>
    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>
    {
        using R = std::invoke_result_t<F, Args...>;

        // 1. 把 f + args 打包成 packaged_task
        //    用 shared_ptr 是因为 packaged_task 不可拷贝（只能移动），
        //    但 std::function 要求 callable 可拷贝 → 套一层 shared_ptr 解决
        auto task = std::make_shared<std::packaged_task<R()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        // 2. 提前拿到 future（必须在 push 之前）
        std::future<R> fut = task->get_future();

        // 3. 把"调用 task 的 lambda"扔进队列
        tasks.push([task]{ (*task)(); });
        return fut;
    }

    void shutdown() {
        if (stopped.exchange(true)) return;     // 防重复 shutdown
        tasks.shutdown();
        for (auto& w : workers) {
            if (w.joinable()) w.join();
        }
    }

    size_t pending() const { return tasks.size(); }
};
