/* 阻塞队列（最终版）：链表 FIFO + mutex + cond + shutdown
 * 生产者-消费者模式的核心组件，线程池的任务队列
 *
 * 三条铁律（教程 §02）：
 *   1. 每条失败路径都要 unlock（C 没有 RAII，lock/unlock 配对自己数）
 *   2. signal/broadcast 放锁外（被唤醒线程醒来不撞未放的锁）
 *   3. cond_wait 必须套 while（防虚假唤醒 + 多消费者竞争）*/
#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct bq_node {
    void *item;
    struct bq_node *next;
} bq_node_t;

typedef struct {
    bq_node_t *head;           /* 出队端 */
    bq_node_t *tail;           /* 入队端 */
    size_t      len;
    pthread_mutex_t mtx;       /* 全部操作都由它保护 */
    pthread_cond_t  not_empty; /* 队列非空时通知 */
    bool        stopped;
} blocking_queue_t;

bool  bq_init(blocking_queue_t *q);
void  bq_destroy(blocking_queue_t *q);
bool  bq_push(blocking_queue_t *q, void *item);   /* 停机后拒收 */
/* 阻塞版：队列空则挂起等待（零 CPU），shutdown 且空时返回 false */
bool  bq_pop(blocking_queue_t *q, void **out);
void  bq_shutdown(blocking_queue_t *q);           /* 存量任务仍会被消费完 */
size_t bq_size(blocking_queue_t *q);

#endif
