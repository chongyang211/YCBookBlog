/* 线程安全 + 时间戳 + 行原子 Logger
 * 裸 printf 分多次拼一行会被并发穿插撕成乱码；黄金结构：
 *   ① 锁外：vsnprintf 先把整行格式化到栈缓冲（vsnprintf 可能耗时，别占锁）
 *   ② 锁内：只做一次输出——整行一个临界区 */
#include "logger.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

static pthread_mutex_t g_log_mtx = PTHREAD_MUTEX_INITIALIZER;

/* 生成 "HH:MM:SS.mmm" 时间戳 */
static void timestamp(char buf[16]) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm tmv;
    localtime_r(&tv.tv_sec, &tmv);            /* ⭐ _r 后缀：可重入版 */
    snprintf(buf, 16, "%02d:%02d:%02d.%03d",
             tmv.tm_hour, tmv.tm_min, tmv.tm_sec,
             (int)(tv.tv_usec / 1000));
}

void logger_info(const char *fmt, ...) {
    char line[256];
    char ts[16];

    /* ⭐ ① 锁外：先把整行格式化到栈缓冲 */
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(line, sizeof line, fmt, ap);
    va_end(ap);
    timestamp(ts);

    /* ⭐ ② 锁内：只做一次输出——行原子 */
    pthread_mutex_lock(&g_log_mtx);
    fprintf(stdout, "[%s][T%lu] %s\n",
            ts, (unsigned long)pthread_self() % 100000, line);
    fflush(stdout);
    pthread_mutex_unlock(&g_log_mtx);
}
