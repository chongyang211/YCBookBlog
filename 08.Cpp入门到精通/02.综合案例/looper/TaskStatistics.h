//
// Created by 杨充 on 2025/9/5.
//

#ifndef TASKSTATISTICS_H
#define TASKSTATISTICS_H
#include <chrono>
#include <deque>
#include <string>

// ===========================================
// 轮询任务和统计系统
// ===========================================
struct TaskResult {
    bool success;
    std::string message;
    std::chrono::steady_clock::time_point timestamp;

    TaskResult(bool s, const std::string& m, std::chrono::steady_clock::time_point t)
        : success(s), message(m), timestamp(t) {}
};

class TaskStatistics {
private:
    int total;
    int success;
    int failure;
    std::deque<TaskResult> recentResults;
    std::string formatTime(const std::chrono::steady_clock::time_point& tp) const;
public:
    TaskStatistics() : total(0), success(0), failure(0) {}
    void recordResult(const TaskResult& result);
    void printSummary() const;
};



#endif //TASKSTATISTICS_H
