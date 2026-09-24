#pragma once
#include <cstdio>
#include <cstdarg>

namespace mm {

enum class LogLv { TRACE, INFO, WARN, ERR };
inline LogLv g_lv = LogLv::INFO;

inline void log(LogLv lv, const char* tag, const char* fmt, ...) {
    if (lv < g_lv) return;
    const char* lvs[] = {"TRACE", "INFO ", "WARN ", "ERROR"};
    std::fprintf(stderr, "[%s][%s] ", lvs[static_cast<int>(lv)], tag);
    std::va_list ap; va_start(ap, fmt);
    std::vfprintf(stderr, fmt, ap);
    va_end(ap);
    std::fputc('\n', stderr);
}

}  // namespace mm

#define MM_TRACE(tag, ...) ::mm::log(::mm::LogLv::TRACE, tag, __VA_ARGS__)
#define MM_INFO(tag, ...)  ::mm::log(::mm::LogLv::INFO,  tag, __VA_ARGS__)
#define MM_WARN(tag, ...)  ::mm::log(::mm::LogLv::WARN,  tag, __VA_ARGS__)
#define MM_ERR(tag, ...)   ::mm::log(::mm::LogLv::ERR,   tag, __VA_ARGS__)
