/* 万级并发压测 + 一致性校验（教程阶段⑦·最终验收）
 * 1000 个用户线程 × 每人 10 单 = 10000 请求抢 5000 库存
 * 验收三条：QPS 过万 / 零超卖（atomic CAS）/ 零漏单（rwlock）*/
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "src/order.h"
#include "src/product.h"
#include "src/strategy.h"
#include "src/thread_pool.h"

#define TOTAL_USERS     1000
#define ORDERS_PER_USER 10
#define INITIAL_STOCK   5000        /* ⭐ 故意 < 总请求 10000 */

static product_t        g_product;
static order_manager_t  g_mgr;
static order_strategy_t g_flash;
static atomic_int g_total_success = 0;

static double now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);        /* 单调时钟测耗时 */
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}

/* 每个用户一个线程：调用 flash 策略（内部 submit 到线程池）*/
static void *user_thread(void *arg) {
    (void)arg;
    int qtys[ORDERS_PER_USER];
    for (int i = 0; i < ORDERS_PER_USER; i++) qtys[i] = 1;

    size_t n = 0;
    int *ids = g_flash.place_orders(&g_flash, &g_product, &g_mgr,
                                    0, qtys, ORDERS_PER_USER, &n);
    int ok = 0;
    for (size_t i = 0; i < n; i++) if (ids[i] > 0) ok++;
    atomic_fetch_add(&g_total_success, ok);
    free(ids);
    return NULL;
}

int main(void) {
    long nt = sysconf(_SC_NPROCESSORS_ONLN);    /* 对应 C++ hardware_concurrency */

    thread_pool_t pool;
    pool_init(&pool, (size_t)(nt > 0 ? nt : 4));

    om_init(&g_mgr);
    product_init(&g_product, 1, "iPhone 15", 7999.0, INITIAL_STOCK);
    strategy_flash_init(&g_flash, &pool);

    double t0 = now_ms();

    /* 启动 1000 个用户线程，每人下 10 单
     * ⭐ 必须检查 create 返回值：失败时 users[u] 未被写入，
     *    join 垃圾 tid 是未定义行为（可能永久挂起）*/
    pthread_t users[TOTAL_USERS];
    int created = 0;
    for (int u = 0; u < TOTAL_USERS; u++) {
        if (pthread_create(&users[u], NULL, user_thread, NULL) != 0) {
            fprintf(stderr, "[bench] 线程 %d 创建失败（已创建 %d）\n",
                    u, created);
            break;
        }
        created++;
    }
    for (int u = 0; u < created; u++)
        pthread_join(users[u], NULL);

    double ms = now_ms() - t0;

    int total    = created * ORDERS_PER_USER;
    int success  = atomic_load(&g_total_success);
    int leftover = product_stock(&g_product);

    printf("========================================\n");
    printf("总请求数:    %d\n", total);
    printf("成功订单:    %d\n", success);
    printf("剩余库存:    %d\n", leftover);
    printf("总耗时:      %.0f ms\n", ms);
    printf("QPS:         %.0f\n", total * 1000.0 / ms);

    /* ⭐⭐⭐ 关键一致性验证：成功 + 剩余 == 初始，否则超卖/漏单 */
    bool consistent = (success + leftover == INITIAL_STOCK);
    printf("一致性校验: 成功(%d) + 剩余(%d) == 初始(%d) ? %s\n",
           success, leftover, INITIAL_STOCK,
           consistent ? "PASS" : "FAIL：超卖或漏单！");
    printf("========================================\n");

    pool_destroy(&pool);
    om_destroy(&g_mgr);
    return consistent ? 0 : 1;      /* 一致则退出码 0 */
}
