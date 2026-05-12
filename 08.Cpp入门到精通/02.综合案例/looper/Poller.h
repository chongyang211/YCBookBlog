//
// Created by 杨充 on 2025/9/5.
//

#ifndef POLLER_H
#define POLLER_H
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include "IPollingStrategy.h"
#include "TaskStatistics.h"
#include "TimeUnit.h"


class Poller {
public:
    typedef std::function<TaskResult()> ActionFunc;
public:
    /**
     * @brief 构造函数
     * @param strategy 轮询策略
     * @param action 要执行的动作
     */
    Poller(std::unique_ptr<IPollingStrategy> strategy, ActionFunc action)
        : strategy_(std::move(strategy)),
          action_(action),
          running_(false),
          paused_(false),
          task_timeout_ns_(0) {}
    ~Poller();
    // 启动轮询器
    void start();
    // 停止轮询器
    void stop();
    // 暂停轮询
    void pause();
    // 继续轮询
    void resume();
    // 更新轮询策略
    void updateStrategy(std::unique_ptr<IPollingStrategy> new_strategy);
    // 打印统计信息
    void printStatistics() const;
    // 手动触发任务
    void triggerNow();
    // 设置任务超时时间
    void setTaskTimeout(long timeout, TimeUnit unit);
private:
    std::unique_ptr<IPollingStrategy> strategy_;
    ActionFunc action_;
    std::atomic<bool> running_;
    bool paused_;
    std::thread worker_thread_;
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    TaskStatistics stats_;
    long task_timeout_ns_;
    // 轮询线程主循环
    void run();
    // 执行任务
    void executeAction();
};



#endif //POLLER_H
