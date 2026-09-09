/* stress（§12.4 并发压测）：8 线程 × 10 万次混合读写
 * 验收三条：无 segfault（rwlock 扛住）/ final size == 100 / 多次运行方差小 */
#include <pthread.h>
#include <stdio.h>
#include <time.h>

#include "mkv/store.h"

#define N_THREADS 8
#define N_OPS     100000

static mkv_store_t g_store;

static void *worker(void *arg) {
    long id = (long)arg;
    char key[16];
    mkv_value_t v;
    for (int j = 0; j < N_OPS; j++) {
        snprintf(key, sizeof key, "k%d", j % 100);
        store_set(&g_store, key, mkv_int(id));              /* 写 */
        if (store_get(&g_store, key, &v)) mkv_value_free(&v);   /* 读 */
    }
    return NULL;
}

int main(void) {
    store_init(&g_store);
    pthread_t ths[N_THREADS];

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (long i = 0; i < N_THREADS; i++)
        pthread_create(&ths[i], NULL, worker, (void *)i);
    for (int i = 0; i < N_THREADS; i++)
        pthread_join(ths[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &t1);

    double ms = (t1.tv_sec - t0.tv_sec) * 1000.0 + (t1.tv_nsec - t0.tv_nsec) / 1e6;
    printf("%d ops in %.0f ms = %.0f ops/s\n",
           N_THREADS * N_OPS * 2, ms, N_THREADS * N_OPS * 2.0 / ms * 1000);
    printf("final size = %zu (expect 100)\n", store_size(&g_store));
    int ok = store_size(&g_store) == 100;
    store_destroy(&g_store);
    return ok ? 0 : 1;
}
