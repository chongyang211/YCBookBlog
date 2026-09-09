#include "strategy.h"

#include <inttypes.h>
#include <stdlib.h>

/* ===== 共用的单笔下单逻辑 =====
 * 返回订单号或 -1，用 intptr_t 装箱进 void* 回传
 * （void* 是 8 字节，小整数直接塞进指针值本身；大对象传堆指针）*/
typedef struct {
    product_t       *p;
    order_manager_t *m;
    int              user_id;
    int              qty;
} order_arg_t;

static void *place_one(void *arg) {
    order_arg_t *a = arg;
    if (product_try_deduct(a->p, a->qty)) {
        int id = om_add_order(a->m, a->user_id, a->p->id, a->qty);
        if (id < 0)
            product_restock(a->p, a->qty);   /* ⭐ 记单失败：回滚库存 */
        return (void *)(intptr_t)id;
    }
    return (void *)(intptr_t)-1;
}

/* ---------- normal：单线程顺序 ---------- */
static int *normal_place(order_strategy_t *s, product_t *p,
                         order_manager_t *m, int user_id,
                         const int *qtys, size_t n, size_t *out_n) {
    (void)s;
    int *ids = malloc(n * sizeof *ids);
    if (!ids) { *out_n = 0; return NULL; }
    for (size_t i = 0; i < n; i++) {
        order_arg_t a = { p, m, user_id, qtys[i] };
        ids[i] = (int)(intptr_t)place_one(&a);      /* 直接调用，不开线程 */
    }
    *out_n = n;
    return ids;
}

void strategy_normal_init(order_strategy_t *s) {
    s->place_orders = normal_place;
    s->pool = NULL;
    s->batch_size = 0;
}

/* ---------- flash：线程池全量并发（秒杀）---------- */
static int *flash_place(order_strategy_t *s, product_t *p,
                        order_manager_t *m, int user_id,
                        const int *qtys, size_t n, size_t *out_n) {
    int *ids = malloc(n * sizeof *ids);
    order_arg_t *args = malloc(n * sizeof *args);
    future_t **futs = malloc(n * sizeof *futs);
    if (!ids || !args || !futs) {
        free(ids); free(args); free(futs);
        *out_n = 0;
        return NULL;
    }

    /* ⭐ 每笔下单作为一个 task 提交，工人并发抢库存 */
    for (size_t i = 0; i < n; i++) {
        args[i] = (order_arg_t){ p, m, user_id, qtys[i] };
        futs[i] = pool_submit(s->pool, place_one, &args[i], true);
    }
    /* 收集所有结果（按提交顺序）——args 在全部 get 前必须存活！*/
    for (size_t i = 0; i < n; i++)
        ids[i] = futs[i] ? (int)(intptr_t)future_get(futs[i]) : -1;
    for (size_t i = 0; i < n; i++) {
        if (futs[i]) { future_destroy(futs[i]); free(futs[i]); }
    }
    free(args);
    free(futs);
    *out_n = n;
    return ids;
}

void strategy_flash_init(order_strategy_t *s, thread_pool_t *pool) {
    s->place_orders = flash_place;
    s->pool = pool;
    s->batch_size = 0;
}

/* ---------- batch：分批并发（组内并发，组间串行）----------
 * 与 flash 的差别：同一时刻最多只有 batch_size 个 future 挂着——内存可控 */
static int *batch_place(order_strategy_t *s, product_t *p,
                        order_manager_t *m, int user_id,
                        const int *qtys, size_t n, size_t *out_n) {
    int *ids = malloc(n * sizeof *ids);
    if (!ids) { *out_n = 0; return NULL; }
    size_t bs = s->batch_size ? s->batch_size : 10;

    for (size_t i = 0; i < n; i += bs) {
        size_t end = (i + bs < n) ? i + bs : n;

        order_arg_t *args = malloc((end - i) * sizeof *args);
        future_t **futs = malloc((end - i) * sizeof *futs);
        if (!args || !futs) { free(args); free(futs); continue; }

        for (size_t j = i; j < end; j++) {
            args[j - i] = (order_arg_t){ p, m, user_id, qtys[j] };
            futs[j - i] = pool_submit(s->pool, place_one,
                                      &args[j - i], true);
        }
        /* 等本批全部完成，再开下一批 */
        for (size_t j = i; j < end; j++)
            ids[j] = futs[j - i] ? (int)(intptr_t)future_get(futs[j - i]) : -1;
        for (size_t j = i; j < end; j++) {
            if (futs[j - i]) { future_destroy(futs[j - i]); free(futs[j - i]); }
        }
        free(args);
        free(futs);
    }
    *out_n = n;
    return ids;
}

void strategy_batch_init(order_strategy_t *s, thread_pool_t *pool,
                         size_t batch_size) {
    s->place_orders = batch_place;
    s->pool = pool;
    s->batch_size = batch_size;
}
