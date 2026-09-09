#include "blocking_queue.h"

#include <stdlib.h>

bool bq_init(blocking_queue_t *q) {
    q->head = q->tail = NULL;
    q->len = 0;
    q->stopped = false;
    if (pthread_mutex_init(&q->mtx, NULL) != 0) return false;
    if (pthread_cond_init(&q->not_empty, NULL) != 0) {
        pthread_mutex_destroy(&q->mtx);
        return false;
    }
    return true;
}

void bq_destroy(blocking_queue_t *q) {
    bq_node_t *n = q->head;
    while (n) {
        bq_node_t *next = n->next;
        free(n);
        n = next;
    }
    q->head = q->tail = NULL;
    q->len = 0;
    pthread_mutex_destroy(&q->mtx);
    pthread_cond_destroy(&q->not_empty);
}

bool bq_push(blocking_queue_t *q, void *item) {
    bq_node_t *n = malloc(sizeof *n);
    if (!n) return false;
    n->item = item;
    n->next = NULL;

    pthread_mutex_lock(&q->mtx);
    if (q->stopped) {
        pthread_mutex_unlock(&q->mtx);
        free(n);
        return false;                       /* 停机后拒收 */
    }
    if (q->tail) q->tail->next = n;
    else         q->head = n;
    q->tail = n;
    q->len++;
    pthread_mutex_unlock(&q->mtx);
    pthread_cond_signal(&q->not_empty);     /* ⭐ 锁外通知（性能更好）*/
    return true;
}

bool bq_pop(blocking_queue_t *q, void **out) {
    pthread_mutex_lock(&q->mtx);
    /* ⭐ while 而不是 if：防虚假唤醒 + 防多个消费者竞争 */
    while (q->len == 0 && !q->stopped)
        pthread_cond_wait(&q->not_empty, &q->mtx);
    if (q->len == 0) {                      /* stopped 且空：下班信号 */
        pthread_mutex_unlock(&q->mtx);
        return false;
    }
    bq_node_t *n = q->head;
    *out = n->item;
    q->head = n->next;
    if (!q->head) q->tail = NULL;
    q->len--;
    free(n);
    pthread_mutex_unlock(&q->mtx);
    return true;
}

void bq_shutdown(blocking_queue_t *q) {
    pthread_mutex_lock(&q->mtx);
    q->stopped = true;                      /* 存量任务仍会被消费完 */
    pthread_mutex_unlock(&q->mtx);
    pthread_cond_broadcast(&q->not_empty);  /* ⭐ 唤醒所有等待者 */
}

size_t bq_size(blocking_queue_t *q) {
    pthread_mutex_lock(&q->mtx);
    size_t n = q->len;                      /* ⭐ 连"读 len"也要锁 */
    pthread_mutex_unlock(&q->mtx);
    return n;
}
