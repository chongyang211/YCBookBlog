#include "future.h"

bool future_init(future_t *f) {
    f->result = NULL;
    f->done = false;
    if (pthread_mutex_init(&f->mtx, NULL) != 0) return false;
    if (pthread_cond_init(&f->cv, NULL) != 0) {
        pthread_mutex_destroy(&f->mtx);
        return false;
    }
    return true;
}

void future_set(future_t *f, void *result) {
    pthread_mutex_lock(&f->mtx);
    f->result = result;
    f->done = true;
    pthread_mutex_unlock(&f->mtx);
    pthread_cond_broadcast(&f->cv);     /* 叫醒所有等待者 */
}

void *future_get(future_t *f) {
    pthread_mutex_lock(&f->mtx);
    while (!f->done)                    /* ⭐ while 防虚假唤醒 */
        pthread_cond_wait(&f->cv, &f->mtx);
    void *r = f->result;
    pthread_mutex_unlock(&f->mtx);
    return r;
}

void future_destroy(future_t *f) {
    pthread_mutex_destroy(&f->mtx);
    pthread_cond_destroy(&f->cv);
}
