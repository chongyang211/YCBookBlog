#pragma once

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace mkv {

enum class LogLevel { Debug, Info, Warn, Error };

inline const char* levelName(LogLevel l) noexcept {
    switch (l) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info:  return "INFO ";
        case LogLevel::Warn:  return "WARN ";
        case LogLevel::Error: return "ERROR";
    }
    return "?????";
}

inline std::string nowStr() {
    auto t  = std::chrono::system_clock::now();
    auto tt = std::chrono::system_clock::to_time_t(t);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &tt);
#else
    localtime_r(&tt, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    return oss.str();
}

}  // namespace mkv

// 关键：宏要写在 namespace 外
#ifndef KV_LOG_LEVEL
#define KV_LOG_LEVEL 1                  // 默认 Info 起步
#endif

#define KV_LOG_IMPL(level, lvname, ...)                                     \
    do {                                                                    \
        if (static_cast<int>(level) >= KV_LOG_LEVEL) {                      \
            std::cerr << "[" << ::mkv::nowStr() << "] "                     \
                      << "[" << lvname << "] "                              \
                      << "[" << __FILE__ << ":" << __LINE__ << " "          \
                      << __func__ << "] "                                   \
                      << __VA_ARGS__ << std::endl;                          \
        }                                                                   \
    } while (0)

#define KV_LOG_DEBUG(...) KV_LOG_IMPL(::mkv::LogLevel::Debug, "DEBUG", __VA_ARGS__)
#define KV_LOG_INFO(...)  KV_LOG_IMPL(::mkv::LogLevel::Info,  "INFO ", __VA_ARGS__)
#define KV_LOG_WARN(...)  KV_LOG_IMPL(::mkv::LogLevel::Warn,  "WARN ", __VA_ARGS__)
#define KV_LOG_ERROR(...) KV_LOG_IMPL(::mkv::LogLevel::Error, "ERROR", __VA_ARGS__)
