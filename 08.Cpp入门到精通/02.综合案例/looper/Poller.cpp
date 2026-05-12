//
// Created by 杨充 on 2025/9/5.
//

#include "Poller.h"

#include <future>
#include <iostream>

#include "TimeConverter.h"
#include "TimeUnit.h"

void Poller::run() {
    while (running_) {
        //设置间隔时间。
        long next_interval = 0;
        {
            //根据不同的策略设置时间
            std::unique_lock<std::mutex> lock(mutex_);
            //计算下一次执行间隔时间
            next_interval = strategy_->getNextInterval();
            // 等待条件：1) 到达间隔时间 2) 被唤醒 3) 暂停状态
            auto now = std::chrono::steady_clock::now();
            auto wake_time = now + std::chrono::nanoseconds(next_interval);
            while (running_ &&std::chrono::steady_clock::now() < wake_time &&!paused_) {
                cond_.wait_until(lock, wake_time);
            }
            // 检查是否暂停
            if (paused_) {
                cond_.wait(lock, [this] { return !paused_ || !running_; });
                continue;
            }
            // 检查是否停止
            if (!running_) {
                break;
            }
        }
        // 执行任务
        executeAction();
    }
}

void Poller::executeAction() {
    auto start_time = std::chrono::steady_clock::now();
    TaskResult result(false, "Unknown result", std::chrono::steady_clock::now());
    try {
        if (task_timeout_ns_ > 0) {
            // 带超时的任务执行
            std::future<TaskResult> future = std::async(std::launch::async, action_);
            auto status = future.wait_for(std::chrono::nanoseconds(task_timeout_ns_));
            if (status == std::future_status::ready) {
                result = future.get();
            } else {
                result = TaskResult(false, "Task timed out", std::chrono::steady_clock::now());
            }
        } else {
            // 不带超时的任务执行
            result = action_();
        }
    } catch (const std::exception& e) {
        result = TaskResult(false, std::string("Exception: ") + e.what(),
                  std::chrono::steady_clock::now());
    } catch (...) {
        result = TaskResult(false, "Unknown exception", std::chrono::steady_clock::now());
    }
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    // 记录结果并更新策略
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (result.success) {
            strategy_->recordSuccess();
        } else {
            strategy_->recordFailure();
        }
    }
    // 输出执行信息
    std::cout << "Action executed in " << duration << "ms. Result: "
              << (result.success ? "SUCCESS" : "FAILURE") << " - " << result.message << std::endl;
}

Poller::~Poller() {
    stop();
}


void Poller::start() {
    if (running_) {
        return;
    }
    running_ = true;
    worker_thread_ = std::thread(&Poller::run,this);
    std::cout << "Poller started with strategy: " << strategy_->getConfigInfo() << std::endl;
}

void Poller::stop() {
    if (!running_) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
        paused_ = false;
    }
    cond_.notify_all();
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
    std::cout << "Poller stopped." << std::endl;
}

// 暂停轮询
void Poller::pause() {
    std::lock_guard<std::mutex> lock(mutex_);
    paused_ = true;
    cond_.notify_all();
    std::cout << "Poller paused." << std::endl;
}

void Poller::resume() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        paused_ = false;
    }
    cond_.notify_all();
    std::cout << "Poller resumed." << std::endl;
}

// 更新轮询策略
void Poller::updateStrategy(std::unique_ptr<IPollingStrategy> new_strategy) {
    std::lock_guard<std::mutex> lock(mutex_);
    strategy_ = std::move(new_strategy);
    strategy_->reset();
    std::cout << "Strategy updated: " << strategy_->getConfigInfo() << std::endl;
}

// 打印统计信息
void Poller::printStatistics() const {
    // stats_.printSummary();
}

// 手动触发任务
void Poller::triggerNow() {
    std::lock_guard<std::mutex> lock(mutex_);
    cond_.notify_all();
}

// 设置任务超时时间
void Poller::setTaskTimeout(long timeout, TimeUnit unit) {
    std::lock_guard<std::mutex> lock(mutex_);
    task_timeout_ns_ = TimeConverter::toNanoseconds(timeout, unit);
    std::cout << "Task timeout set to: " << task_timeout_ns_ << " ns" << std::endl;
}

