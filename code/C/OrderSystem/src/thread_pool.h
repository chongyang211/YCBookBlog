/* 通用线程池：工人线程 + 任务队列 + future 异步返回（教程阶段②）
 *
 * 为什么不用"每请求一线程"？——pthread_create 约 50µs、线程数无上限会 OOM、
 * detach 后生死不明、没有背压。固定工人 + 队列 = 线程受控、复用、天然背压。
 *
 * 任务 = 函数指针 + void* 参数（C 的"可调用对象"，04 案例描述表同款哲学）*/
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

#include "blocking_queue.h"
#include "future.h"

/* ⭐ 任务：接收 void*，返回 void*（ intptr_t 装箱小整数，堆指针传大对象）*/
typedef void *(*task_fn)(void *arg);

typedef struct {
    task_fn   fn;
    void     *arg;
    future_t *fut;      /* NULL = fire-and-forget（不关心结果）*/
} task_t;

typedef struct {
    pthread_t       *workers;
    size_t           n;
    blocking_queue_t q;
} thread_pool_t;

bool pool_init(thread_pool_t *p, size_t n);
/* shutdown 队列 → join 工人 → 释放。顺序绝不能反（先 join 会死锁）*/
void pool_destroy(thread_pool_t *p);

/* 提交任务：
 *   want_result = false → 返回 NULL，submit 即"发射后不管"
 *   want_result = true  → 返回 future*，调用方 future_get 后 destroy + free */
future_t *pool_submit(thread_pool_t *p, task_fn fn, void *arg,
                      bool want_result);

#endif
