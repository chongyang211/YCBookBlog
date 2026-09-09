/* 三种下单策略（教程阶段⑤）：函数指针表 = C 的策略模式
 * 03 案例手动 vtable 的直接复用——调用代码不变，只换结构体里的函数指针
 *
 * 线程池由调用方创建并注入（依赖注入），策略对象只是"用户"：
 * 避免 3 个策略 3 个池的资源浪费与生命周期失控 */
#ifndef STRATEGY_H
#define STRATEGY_H

#include <stddef.h>

#include "order.h"
#include "product.h"
#include "thread_pool.h"

/* 策略"接口"：哪个函数指针指哪，就是哪个策略 */
typedef struct order_strategy {
    /* 下 n 单；返回 malloc 的 int 数组（每单订单号，-1 = 失败），
     * *out_n 为长度，调用方负责 free 返回值 */
    int *(*place_orders)(struct order_strategy *self,
                         product_t *p, order_manager_t *m,
                         int user_id, const int *qtys, size_t n,
                         size_t *out_n);
    thread_pool_t *pool;      /* flash / batch 用；normal 不用（依赖注入）*/
    size_t batch_size;        /* batch 用 */
} order_strategy_t;

void strategy_normal_init(order_strategy_t *s);
void strategy_flash_init(order_strategy_t *s, thread_pool_t *pool);
void strategy_batch_init(order_strategy_t *s, thread_pool_t *pool,
                         size_t batch_size);

#endif
