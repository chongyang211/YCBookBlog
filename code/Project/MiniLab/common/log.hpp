// common/log.hpp — 彩色日志宏（基于 fmt）
#pragma once
#include "fmt_shim.hpp"

#define LOG_INFO(...)  ::fmt::print(::fmt::fg(::fmt::color::cyan),         "[INFO ] " __VA_ARGS__)
#define LOG_WARN(...)  ::fmt::print(::fmt::fg(::fmt::color::yellow),       "[WARN ] " __VA_ARGS__)
#define LOG_ERR(...)   ::fmt::print(::fmt::fg(::fmt::color::red),          "[ERROR] " __VA_ARGS__)
#define LOG_TRACE(...) ::fmt::print(::fmt::fg(::fmt::color::dim_gray),     "[TRACE] " __VA_ARGS__)
#define LOG_OK(...)    ::fmt::print(::fmt::fg(::fmt::color::green),        "[ OK  ] " __VA_ARGS__)
