#include "product.h"

void product_init(product_t *p, int id, const char *name,
                  double price, int stock) {
    p->id = id;
    p->name = name;
    p->price = price;
    atomic_init(&p->stock, stock);
}

int product_stock(product_t *p) {
    return atomic_load(&p->stock);       /* 原子读 */
}

bool product_try_deduct(product_t *p, int n) {
    int cur = atomic_load(&p->stock);
    while (cur >= n) {
        /* CAS：如果 stock 仍然是 cur，则把它改成 cur - n
         * 失败时 cur 会被自动更新为最新值，进入下一轮
         * weak 会伪失败但循环里立即重试，性能优于 strong——循环首选 weak */
        if (atomic_compare_exchange_weak(&p->stock, &cur, cur - n))
            return true;                 /* 成功扣减 */
    }
    return false;                        /* 库存不够（cur < n）*/
}

void product_restock(product_t *p, int n) {
    atomic_fetch_add(&p->stock, n);
}
