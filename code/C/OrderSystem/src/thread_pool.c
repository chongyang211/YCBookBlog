#include "thread_pool.h"

#include <stdlib.h>

/* 工人主循环：阻塞 pop 任务 → 执行 → 结果回填 future
 * 相当于自动执行 packaged_task 并 promise.set_value */
static void *worker_main(void *arg) {
    thread_pool_t *p = arg;
    for (;;) {
        task_t *t;
        if (!bq_pop(&p->q, (void **)&t)) break;   /* shutdown 且空：下班 */
        void *result = t->fn(t->arg);             /* ⭐ 执行任务 */
        if (t->fut) future_set(t->fut, result);   /* ⭐ 结果自动回填 future */
        free(t);
    }
    return NULL;
}

bool pool_init(thread_pool_t *p, size_t n) {
    p->workers = calloc(n, sizeof *p->workers);
    if (!p->workers) return false;
    if (!bq_init(&p->q)) {
        free(p->workers);
        return false;
    }
    p->n = 0;
    for (size_t i = 0; i < n; i++) {
        if (pthread_create(&p->workers[i], NULL, worker_main, p) != 0)
            return p->n > 0;                      /* 教学从简：部分启动也接受 */
        p->n++;
    }
    return true;
}

future_t *pool_submit(thread_pool_t *p, task_fn fn, void *arg,
                      bool want_result) {
    task_t *t = malloc(sizeof *t);
    if (!t) return NULL;
    t->fn = fn;
    t->arg = arg;
    t->fut = NULL;
    future_t *fut = NULL;

    if (want_result) {
        t->fut = malloc(sizeof *t->fut);
        if (!t->fut || !future_init(t->fut)) {
            free(t->fut);
            free(t);
            return NULL;
        }
        fut = t->fut;                   /* ⭐ push 前存局部变量 */
    }
    /* ⭐ 先把 future 准备好，再 push——顺序不能反！
     *    push 后任务可能立刻被工人执行并 free(t)，
     *    因此 push 之后绝不能再碰 t（教程原版 return t->fut 是 UAF）*/
    if (!bq_push(&p->q, t)) {
        if (t->fut) { future_destroy(t->fut); free(t->fut); }
        free(t);
        return NULL;
    }
    return fut;
}

void pool_destroy(thread_pool_t *p) {
    bq_shutdown(&p->q);                           /* ⭐ 先停机（drain 语义）*/
    for (size_t i = 0; i < p->n; i++)
        pthread_join(p->workers[i], NULL);        /* ⭐ 再 join */
    bq_destroy(&p->q);
    free(p->workers);
    p->workers = NULL;
    p->n = 0;
}
