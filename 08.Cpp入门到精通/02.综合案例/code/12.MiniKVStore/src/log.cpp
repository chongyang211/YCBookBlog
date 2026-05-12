// log.cpp —— 日志辅助函数实现
//
// 跨平台 localtime：Windows 用 localtime_s，POSIX 用 localtime_r。

#include "mkv/log.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace mkv {

const char* levelName(LogLevel l) noexcept {
    switch (l) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info:  return "INFO ";
        case LogLevel::Warn:  return "WARN ";
        case LogLevel::Error: return "ERROR";
    }
    return "?????";
}

std::string nowStr() {
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
