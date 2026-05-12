#include "TimeConverter.h"
#include <stdexcept>
#include <string>

//
// Created by 杨充 on 2025/9/5.
//
/**
 * @brief 时间转换工具类
 */
long TimeConverter::toNanoseconds(long duration, TimeUnit unit) {
    switch (unit) {
        case TimeUnit::NANOSECONDS: return duration;
        case TimeUnit::MICROSECONDS: return duration * 1000L;
        case TimeUnit::MILLISECONDS: return duration * 1000000L;
        case TimeUnit::SECONDS: return duration * 1000000000L;
        case TimeUnit::MINUTES: return duration * 60000000000L;
        default: throw std::invalid_argument("Invalid time unit");
    }
}

std::string TimeConverter::unitToString(TimeUnit unit) {
    switch (unit) {
        case TimeUnit::NANOSECONDS: return "ns";
        case TimeUnit::MICROSECONDS: return "us";
        case TimeUnit::MILLISECONDS: return "ms";
        case TimeUnit::SECONDS: return "s";
        case TimeUnit::MINUTES: return "min";
        default: return "unknown";
    }
}



