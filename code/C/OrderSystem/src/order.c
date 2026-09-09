#include "order.h"

#include <stdlib.h>

void om_init(order_manager_t *m) {
    m->orders = NULL;
    m->len = m->cap = 0;
    pthread_rwlock_init(&m->rwlock, NULL);
    atomic_init(&m->next_id, 1);
}

void om_destroy(order_manager_t *m) {
    free(m->orders);
    m->orders = NULL;
    pthread_rwlock_destroy(&m->rwlock);
}

int om_add_order(order_manager_t *m, int user_id, int product_id, int qty) {
    order_t o = {
        .id         = atomic_fetch_add(&m->next_id, 1),   /* ⭐ 原子自增（锁外）*/
        .user_id    = user_id,
        .product_id = product_id,
        .quantity   = qty,
    };

    pthread_rwlock_wrlock(&m->rwlock);           /* ⭐ 写：独占锁 */
    if (m->len == m->cap) {
        size_t nc = m->cap ? m->cap * 2 : 16;
        order_t *no = realloc(m->orders, nc * sizeof *no);
        if (!no) {
            pthread_rwlock_unlock(&m->rwlock);   /* ⭐ 失败路径也要解锁 */
            return -1;
        }
        m->orders = no;
        m->cap = nc;
    }
    m->orders[m->len++] = o;
    pthread_rwlock_unlock(&m->rwlock);
    return o.id;
}

bool om_find_order(order_manager_t *m, int id, order_t *out) {
    bool found = false;
    pthread_rwlock_rdlock(&m->rwlock);           /* ⭐ 读：共享锁！*/
    for (size_t i = 0; i < m->len; i++) {
        if (m->orders[i].id == id) {
            *out = m->orders[i];                 /* 拷贝出参，锁外使用 */
            found = true;
            break;
        }
    }
    pthread_rwlock_unlock(&m->rwlock);
    return found;
}

size_t om_orders_by_user(order_manager_t *m, int user_id,
                         order_t *out, size_t cap) {
    size_t n = 0;
    pthread_rwlock_rdlock(&m->rwlock);
    for (size_t i = 0; i < m->len && n < cap; i++)
        if (m->orders[i].user_id == user_id) out[n++] = m->orders[i];
    pthread_rwlock_unlock(&m->rwlock);
    return n;
}

size_t om_size(order_manager_t *m) {
    pthread_rwlock_rdlock(&m->rwlock);
    size_t n = m->len;
    pthread_rwlock_unlock(&m->rwlock);
    return n;
}
