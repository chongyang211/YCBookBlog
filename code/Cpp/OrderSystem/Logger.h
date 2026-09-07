#pragma once

#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <ctime>

// 线程安全日志：mutex 保证整行原子输出，C++17 折叠表达式拼接参数
class Logger {
private:
    static inline std::mutex mtx;        // C++17 inline static：头文件里就能初始化

    // 生成 "HH:MM:SS.mmm" 格式时间戳
    static std::string timestamp() {
        auto now = std::chrono::system_clock::now();
        auto t   = std::chrono::system_clock::to_time_t(now);
        auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) % 1000;
        std::tm local{};
#if defined(_WIN32)
        localtime_s(&local, &t);
#else
        localtime_r(&t, &local);         // 线程安全版本
#endif
        std::ostringstream oss;
        oss << std::put_time(&local, "%H:%M:%S")
            << "." << std::setw(3) << std::setfill('0') << ms.count();
        return oss.str();
    }

public:
    template <typename... Args>
    static void info(Args&&... args) {
        std::lock_guard<std::mutex> lock(mtx);                       // 整行原子
        std::cout << "[" << timestamp() << "][T"
                  << std::this_thread::get_id() << "] ";
        (std::cout << ... << std::forward<Args>(args));              // C++17 折叠表达式
        std::cout << "\n";
    }
};
