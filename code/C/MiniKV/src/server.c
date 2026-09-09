/* TTL 后台清理线程（§11）：手动版 jthread
 * 惰性删除（store_get 内）兜不住"没人读的过期 key"，这里定期扫描补刀 */
#include "mkv/server.h"

#include <unistd.h>

#include "mkv/log.h"

static void *purger_main(void *arg) {
    mkv_server_t *sv = arg;
    KV_LOG_INFO("ttl purger thread started");
    while (!atomic_load(&sv->stop)) {           /* ⭐ 协作式停止 */
        usleep(100 * 1000);                     /* 每 100ms 巡一次 */
        store_purge_expired(sv->store);         /* 内部自己拿写锁 */
    }
    KV_LOG_INFO("ttl purger thread stopped");
    return NULL;
}

bool server_start(mkv_server_t *sv, mkv_store_t *store) {
    sv->store = store;                          /* 借用指针，不拷贝（防浅拷贝陷阱）*/
    atomic_init(&sv->stop, false);
    sv->started = pthread_create(&sv->thread, NULL, purger_main, sv) == 0;
    return sv->started;
}

void server_stop(mkv_server_t *sv) {
    if (!sv->started) return;
    atomic_store(&sv->stop, true);              /* 先说"停下" */
    pthread_join(sv->thread, NULL);             /* 再等它走完 */
    sv->started = false;
}
