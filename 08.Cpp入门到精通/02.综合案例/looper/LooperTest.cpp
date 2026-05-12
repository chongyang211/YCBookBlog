//
// Created by 杨充 on 2025/9/5.
//

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <atomic>
#include <queue>
#include <vector>
#include <memory>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <random>
#include <deque>
#include <future>
#include <ctime>

#include "Looper.h"
#include "IPollingStrategy.h"
#include "Poller.h"
#include "TaskStatistics.h"
#include "TimeConverter.h"
#include "TimeUnit.h"
// ===========================================
// 示例使用场景
// ===========================================
//g++ -std=c++11 LooperTest.cpp TaskStatistics.cpp Poller.cpp Looper.cpp TimeConverter.cpp
int main(int argc, char *argv[]) {
    // 示例任务函数
    auto sampleAction = []() -> TaskResult {
        // 模拟任务执行，有30%的失败率
        static int counter = 0;
        counter++;

        if (counter % 10 < 3) {
            // 模拟失败
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
            return TaskResult(false, "Simulated failure #" + std::to_string(counter),
                             std::chrono::steady_clock::now());
        }

        // 模拟成功
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return TaskResult(true, "Operation succeeded #" + std::to_string(counter),
                         std::chrono::steady_clock::now());
    };

    // 创建轮询器 - 初始使用固定间隔策略
    Poller poller(
        std::unique_ptr<IPollingStrategy>(
            new FixedIntervalStrategy(1000, TimeUnit::MILLISECONDS)),
        sampleAction
    );

    // 设置任务超时
    poller.setTaskTimeout(100, TimeUnit::MILLISECONDS);

    // 启动轮询器
    poller.start();

    // 运行一段时间
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // 暂停轮询
    poller.pause();
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 继续轮询
    poller.resume();

    // 运行一段时间
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // 切换到指数退避策略
    poller.updateStrategy(
        std::unique_ptr<IPollingStrategy>(
            new ExponentialBackoffStrategy(500, 5000, TimeUnit::MILLISECONDS))
    );

    // 再运行一段时间
    std::this_thread::sleep_for(std::chrono::seconds(8));

    // 手动触发一次任务
    std::cout << "Manually triggering task..." << std::endl;
    poller.triggerNow();

    // 再运行一段时间
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 停止轮询器并打印统计信息
    poller.stop();
    poller.printStatistics();

    return 0;
}


