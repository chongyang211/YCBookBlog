/* KV_LOG 宏（§09）：时间戳 + 级别 + 文件位置 + 条件编译
 * 必须是宏不能是函数：__FILE__/__LINE__/__func__ 在宏展开处提供，
 * 函数拿到的永远是函数自己所在的行 */
#ifndef MKV_LOG_H
#define MKV_LOG_H

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

enum { KV_LOG_DEBUG = 0, KV_LOG_INFO = 1, KV_LOG_WARN = 2, KV_LOG_ERROR = 3 };

/* 编译期可覆盖：make CFLAGS+="-DKV_LOG_LEVEL=2" 只留 WARN+ERROR */
#ifndef KV_LOG_LEVEL
#define KV_LOG_LEVEL 1                    /* 默认 INFO 起步 */
#endif

/* "HH:MM:SS.mmm"（localtime_r 线程安全版）*/
static inline void mkv_now_str(char buf[16]) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm tmv;
    localtime_r(&tv.tv_sec, &tmv);
    snprintf(buf, 16, "%02d:%02d:%02d.%03d",
             tmv.tm_hour, tmv.tm_min, tmv.tm_sec, (int)(tv.tv_usec / 1000));
}

/* ⭐ do-while(0)：宏体永远是"一条语句"，防 else 错配（§9.2 造 BUG）*/
#define KV_LOG_IMPL(lvl, lvname, fmt, ...)                                  \
    do {                                                                    \
        if ((lvl) >= KV_LOG_LEVEL) {                                        \
            char _ts[16];                                                   \
            mkv_now_str(_ts);                                               \
            fprintf(stderr, "[%s] [%s] [%s:%d %s] " fmt "\n",               \
                    _ts, lvname, __FILE__, __LINE__, __func__,              \
                    ##__VA_ARGS__);                                         \
        }                                                                   \
    } while (0)

#define KV_LOG_DEBUG(fmt, ...) KV_LOG_IMPL(KV_LOG_DEBUG, "DEBUG", fmt, ##__VA_ARGS__)
#define KV_LOG_INFO(fmt, ...)  KV_LOG_IMPL(KV_LOG_INFO,  "INFO ", fmt, ##__VA_ARGS__)
#define KV_LOG_WARN(fmt, ...)  KV_LOG_IMPL(KV_LOG_WARN,  "WARN ", fmt, ##__VA_ARGS__)
#define KV_LOG_ERROR(fmt, ...) KV_LOG_IMPL(KV_LOG_ERROR, "ERROR", fmt, ##__VA_ARGS__)

#endif
