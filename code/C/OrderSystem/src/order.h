/* OrderManager：订单存储 + 多线程读写（教程阶段③，rwlock 版）
 *
 * 两个细节：
 *   ① atomic_fetch_add 在锁外执行——ID 生成不依赖数组，不占锁自增
 *   ② 查询把订单拷贝到出参再返回——绝不能返回内部指针
 *      （unlock 后另一线程可能 realloc 把它变成悬垂指针）*/
#ifndef ORDER_H
#define ORDER_H

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    int id;
    int user_id;
    int product_id;
    int quantity;
} order_t;

typedef struct {
    order_t        *orders;
    size_t          len, cap;
    pthread_rwlock_t rwlock;     /* 读写锁：80% 查询场景读吞吐翻倍 */
    atomic_int      next_id;     /* ⭐ ID 自增用原子，永不重复 */
} order_manager_t;

void  om_init(order_manager_t *m);
void  om_destroy(order_manager_t *m);
int   om_add_order(order_manager_t *m, int user_id, int product_id, int qty);
bool  om_find_order(order_manager_t *m, int id, order_t *out);
size_t om_orders_by_user(order_manager_t *m, int user_id,
                         order_t *out, size_t cap);
size_t om_size(order_manager_t *m);

#endif
