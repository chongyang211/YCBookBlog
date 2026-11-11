// src/core/log.cpp —— 跨平台日志实现
//
// 输出通道：文件 sink（追加）+ 宿主回调 sink（转发 logcat/OSLog/...）。
// 线程安全：文件写入与 sink 列表由互斥锁保护；**回调在无锁状态调用**，
//          宿主回调里再调 mp_log_write 不会死锁。
// 时间戳：本地时区，毫秒精度，格式 YYYY-MM-DD HH:MM:SS.mmm。
#include "mp_log.h"

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>

#if defined(_WIN32)
  #include <windows.h>
  #define MP_LOCALTIME(t, tm) localtime_s(tm, t)
#else
  #include <ctime>
  #define MP_LOCALTIME(t, tm) localtime_r(t, tm)
#endif

namespace {

std::mutex g_mu;
std::FILE* g_file = nullptr;              /* 文件 sink（可空） */
int32_t    g_level = MP_LOG_INFO;
std::vector<std::pair<mp_log_sink_cb, void*>> g_sinks;
bool       g_init = false;

const char* level_name(int32_t level) {
    switch (level) {
        case MP_LOG_TRACE: return "TRACE";
        case MP_LOG_DEBUG: return "DEBUG";
        case MP_LOG_INFO:  return "INFO";
        case MP_LOG_WARN:  return "WARN";
        case MP_LOG_ERROR: return "ERROR";
        default:           return "?????";
    }
}

/* 格式化用户消息；超过栈缓冲自动转堆 */
void format_message(const char* fmt, va_list ap,
                    char* stack_buf, size_t stack_size,
                    char** heap_buf, std::string& out) {
    *heap_buf = nullptr;
    va_list copy;
    va_copy(copy, ap);
    const int need = std::vsnprintf(stack_buf, stack_size, fmt, ap);
    if (need < 0) { out = "(log format error)"; va_end(copy); return; }
    if (static_cast<size_t>(need) < stack_size) {
        out.assign(stack_buf, static_cast<size_t>(need));
        va_end(copy);
        return;
    }
    /* 超长：按实际长度分配堆缓冲重试一次 */
    *heap_buf = static_cast<char*>(std::malloc(static_cast<size_t>(need) + 1));
    if (!*heap_buf) { out = "(log oom)"; va_end(copy); return; }
    std::vsnprintf(*heap_buf, static_cast<size_t>(need) + 1, fmt, copy);
    out.assign(*heap_buf, static_cast<size_t>(need));
    va_end(copy);
}

}  // namespace

extern "C" {

int32_t mp_log_init(const char* file_path, int32_t level) {
    if (level < MP_LOG_TRACE || level > MP_LOG_OFF) return MP_ERR_ARGS;

    std::lock_guard<std::mutex> g(g_mu);
    if (g_file) { std::fclose(g_file); g_file = nullptr; }
    if (file_path && *file_path) {
        g_file = std::fopen(file_path, "a");
        if (!g_file) { g_init = true; g_level = level; return MP_ERR_IO; }
    }
    g_level = level;
    g_init = true;
    return MP_OK;
}

int32_t mp_log_set_level(int32_t level) {
    if (level < MP_LOG_TRACE || level > MP_LOG_OFF) return MP_ERR_ARGS;
    std::lock_guard<std::mutex> g(g_mu);
    g_level = level;
    return MP_OK;
}

int32_t mp_log_add_sink(mp_log_sink_cb cb, void* user_data) {
    if (!cb) return MP_ERR_ARGS;
    std::lock_guard<std::mutex> g(g_mu);
    g_sinks.emplace_back(cb, user_data);
    return MP_OK;
}

int32_t mp_log_write(int32_t level, const char* tag, const char* fmt, ...) {
    if (!fmt) return MP_ERR_ARGS;
    if (level < MP_LOG_TRACE || level > MP_LOG_ERROR) return MP_ERR_ARGS;

    /* 快照配置（级别/句柄/sink 列表），锁外执行格式化与回调 */
    std::FILE* file = nullptr;
    std::vector<std::pair<mp_log_sink_cb, void*>> sinks;
    {
        std::lock_guard<std::mutex> g(g_mu);
        if (!g_init || level < g_level) return MP_OK;
        file = g_file;
        sinks = g_sinks;
    }

    /* 1. 用户消息格式化 */
    char stack_buf[1024];
    char* heap_buf = nullptr;
    std::string msg;
    va_list ap;
    va_start(ap, fmt);
    format_message(fmt, ap, stack_buf, sizeof(stack_buf), &heap_buf, msg);
    va_end(ap);

    /* 2. 时间戳（本地时区 + 毫秒） */
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    const int ms = static_cast<int>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count() % 1000);
    std::tm tmv{};
    MP_LOCALTIME(&t, &tmv);

    char line_head[80];
    std::snprintf(line_head, sizeof(line_head),
                  "%04d-%02d-%02d %02d:%02d:%02d.%03d [%s] [%s] ",
                  tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
                  tmv.tm_hour, tmv.tm_min, tmv.tm_sec, ms,
                  level_name(level), tag ? tag : "-");

    /* 3. 文件 sink（追加 + flush） */
    if (file) {
        std::fputs(line_head, file);
        std::fputs(msg.c_str(), file);
        std::fputc('\n', file);
        std::fflush(file);
    }

    /* 4. 宿主回调 sink（无锁调用） */
    for (auto& s : sinks) {
        s.first(level, tag ? tag : "-", msg.c_str(), s.second);
    }

    std::free(heap_buf);
    return MP_OK;
}

void mp_log_shutdown(void) {
    std::lock_guard<std::mutex> g(g_mu);
    if (g_file) { std::fclose(g_file); g_file = nullptr; }
    g_sinks.clear();
    g_init = false;
}

}  // extern "C"
