//
// Created by 杨充 on 2025/9/5.
//

#include "TaskStatistics.h"
#include <iomanip>
#include <iostream>
#include <sstream>

// ===========================================
// 轮询任务和统计系统
// ===========================================

std::string TaskStatistics::formatTime(const std::chrono::steady_clock::time_point &tp) const {
    // 使用相对时间而不是绝对时间
    auto duration = tp.time_since_epoch();
    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration % std::chrono::hours(1));
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration % std::chrono::minutes(1));
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration % std::chrono::seconds(1));

    std::stringstream ss;
    ss << std::setfill('0')
       << std::setw(2) << hours.count() % 24 << ":"
       << std::setw(2) << minutes.count() << ":"
       << std::setw(2) << seconds.count() << "."
       << std::setw(3) << milliseconds.count();
    return ss.str();
}

void TaskStatistics::recordResult(const TaskResult &result) {
    total++;
    if (result.success) {
        success++;
    } else {
        failure++;
    }
    // 保留最近100条结果
    recentResults.push_back(result);
    if (recentResults.size() > 100) {
        recentResults.pop_front();
    }
}


void TaskStatistics::printSummary() const {
    std::cout << "=== Task Statistics ===" << std::endl;
    std::cout << "Total runs: " << total << std::endl;
    std::cout << "Success: " << success << " ("
              << (total ? (success * 100.0 / total) : 0.0) << "%)" << std::endl;
    std::cout << "Failure: " << failure << " ("
              << (total ? (failure * 100.0 / total) : 0.0) << "%)" << std::endl;

    if (!recentResults.empty()) {
        std::cout << "\nRecent results (last " << recentResults.size() << "):" << std::endl;
        for (const auto& res : recentResults) {
            auto timeStr = formatTime(res.timestamp);
            std::cout << "[" << timeStr << "] "
                      << (res.success ? "SUCCESS" : "FAILURE")
                      << ": " << res.message << std::endl;
        }
    }
}

