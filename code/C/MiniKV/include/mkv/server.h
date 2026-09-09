/* TTL 后台清理线程（§11）：手动版 jthread
 * pthread + atomic_bool 协作式停止 + join 收尾
 * 收尾顺序（C 版的"析构顺序"）：server_stop → aof_close → store_destroy */
#ifndef MKV_SERVER_H
#define MKV_SERVER_H

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include "mkv/store.h"

typedef struct {
    pthread_t     thread;
    mkv_store_t  *store;        /* 借用：生命周期由 main 的收尾顺序保证 */
    atomic_bool   stop;         /* ⭐ 协作式停止标志（C 版 stop_token）*/
    bool          started;
} mkv_server_t;

bool server_start(mkv_server_t *sv, mkv_store_t *store);
void server_stop(mkv_server_t *sv);     /* 置 stop + join：必须调！*/

#endif
