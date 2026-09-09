/* Product：atomic CAS 原子扣库存，防超卖（教程阶段④核心⭐）
 *
 * ❌ 裸 int 版的 check-then-act race：
 *    if (p->stock >= n) p->stock -= n;   ← 两步之间被插队 → 超卖（stock 变负）
 * ✅ CAS 循环把"判断+扣减"变成一条不可分割的硬件指令，OS 也插不进缝 */
#ifndef PRODUCT_H
#define PRODUCT_H

#include <stdatomic.h>
#include <stdbool.h>

typedef struct {
    int         id;
    const char *name;
    double      price;
    atomic_int  stock;         /* ⭐ C11 原子变量 */
} product_t;

void product_init(product_t *p, int id, const char *name,
                  double price, int stock);
int  product_stock(product_t *p);

/* ⭐⭐⭐ CAS 循环扣库存（防超卖）：成功返回 true，库存不够返回 false */
bool product_try_deduct(product_t *p, int n);
void product_restock(product_t *p, int n);

#endif
