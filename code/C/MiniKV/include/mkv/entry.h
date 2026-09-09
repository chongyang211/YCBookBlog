/* Entry + TTL（§04）：value 的"包装层"，管过期与版本 */
#ifndef MKV_ENTRY_H
#define MKV_ENTRY_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "mkv/value.h"

/* 单调时钟毫秒数：C 版 steady_clock
 * ⭐ 不是 CLOCK_REALTIME：墙上时钟会被改系统时间，TTL 会乱套 */
static inline int64_t mkv_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

typedef struct {
    mkv_value_t value;
    bool        has_expire;      /* C 的 "optional"：显式 bool 标志 */
    int64_t     expire_ms;       /* 绝对过期点（MONOTONIC 毫秒）*/
    int64_t     created_ms;
    uint64_t    version;         /* 每次 SET 覆写 +1，将来做 CAS */
} mkv_entry_t;

static inline bool mkv_entry_expired(const mkv_entry_t *e) {
    return e->has_expire && mkv_now_ms() >= e->expire_ms;
}

#endif
