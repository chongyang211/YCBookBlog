/* 手写 mini future：C++ std::future 的裸机原理（教程阶段②核心⭐）
 *
 * future = 结果槽（void* result）
 *        + 就绪标志（bool done）
 *        + 一对 mutex/cond（"还没好就睡，好了叫你"）
 *
 * 对照 C++ 三剑客：
 *   std::promise<T>     = future_set 的写入端
 *   std::future<T>      = future_get 的读取端
 *   std::packaged_task  = "自动调 fn 并把返回值 future_set 进去"（thread_pool 实现）*/
#ifndef FUTURE_H
#define FUTURE_H

#include <pthread.h>
#include <stdbool.h>

typedef struct {
    void          *result;     /* 结果槽 */
    bool           done;       /* 就绪标志 */
    pthread_mutex_t mtx;
    pthread_cond_t  cv;
} future_t;

bool  future_init(future_t *f);
void  future_set(future_t *f, void *result);   /* worker 调用，恰好一次 */
void *future_get(future_t *f);                 /* 调用方阻塞直到 set */
void  future_destroy(future_t *f);

#endif
