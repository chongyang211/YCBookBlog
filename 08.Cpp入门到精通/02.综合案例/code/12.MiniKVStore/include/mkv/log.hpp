// mkv/log.hpp —— KV_LOG 宏 + 日志级别
//
// 关键设计（详见 12.迷你KV存储引擎.md §09）：
// - 用 do{...}while(0) 包整个体，避免 if-else 错位；
// - 宏体里所有标识符前缀 ::mkv::，避免在用户 namespace 里被截胡；
// - 用 << 拼接而不是 printf，类型安全；
// - 通过 KV_LOG_LEVEL 编译期常量裁剪，optimizer 会消除整段 if。

#pragma once

#include <iostream>
#include <string>

namespace mkv {

enum class LogLevel { Debug = 0, Info = 1, Warn = 2, Error = 3 };

const char* levelName(LogLevel l) noexcept;

// 当前时间字符串（HH:MM:SS）
std::string nowStr();

}  // namespace mkv

// 注意：宏写在 namespace 外
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

#define KV_LOG_DEBUG(...) KV_LOG_IMPL(0, "DEBUG", __VA_ARGS__)
#define KV_LOG_INFO(...)  KV_LOG_IMPL(1, "INFO ", __VA_ARGS__)
#define KV_LOG_WARN(...)  KV_LOG_IMPL(2, "WARN ", __VA_ARGS__)
#define KV_LOG_ERROR(...) KV_LOG_IMPL(3, "ERROR", __VA_ARGS__)
