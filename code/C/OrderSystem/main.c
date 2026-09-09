/* 交互式 CLI（教程阶段⑦）：串起线程池 / 策略 / 订单 / 库存 / 日志 */
#include <stdio.h>
#include <stdlib.h>

#include "src/logger.h"
#include "src/order.h"
#include "src/product.h"
#include "src/strategy.h"
#include "src/thread_pool.h"

int main(void) {
    thread_pool_t pool;
    pool_init(&pool, 4);

    order_manager_t mgr;
    om_init(&mgr);

    product_t iphone;
    product_init(&iphone, 1, "iPhone 15", 7999.0, 100);

    order_strategy_t normal, flash;
    strategy_normal_init(&normal);
    strategy_flash_init(&flash, &pool);      /* ⭐ 线程池注入策略 */

    while (true) {
        printf("\n========== 订单系统 ==========\n"
               "1=普通下单  2=秒杀下单  3=查询订单\n"
               "4=查看库存  0=退出\n> ");
        int op;
        if (scanf("%d", &op) != 1) break;
        if (op == 0) break;

        if (op == 1 || op == 2) {
            int user_id, count;
            printf("用户 ID: ");
            if (scanf("%d", &user_id) != 1) break;
            printf("下单次数: ");
            if (scanf("%d", &count) != 1) break;
            if (count <= 0 || count > 100000) continue;

            int *qtys = malloc((size_t)count * sizeof *qtys);
            if (!qtys) continue;
            for (int i = 0; i < count; i++) qtys[i] = 1;

            order_strategy_t *s = (op == 1) ? &normal : &flash;
            size_t n = 0;
            int *ids = s->place_orders(s, &iphone, &mgr, user_id,
                                       qtys, (size_t)count, &n);
            int success = 0;
            for (size_t i = 0; i < n; i++) if (ids[i] > 0) success++;
            logger_info("用户 %d 提交 %d 单，成功 %d",
                        user_id, count, success);
            free(ids);
            free(qtys);
        }
        else if (op == 3) {
            int user_id;
            printf("用户 ID: ");
            if (scanf("%d", &user_id) != 1) break;
            order_t out[256];
            size_t n = om_orders_by_user(&mgr, user_id, out, 256);
            printf("用户 %d 共 %zu 单：\n", user_id, n);
            for (size_t i = 0; i < n; i++)
                printf("  订单 #%d - 商品 %d - 数量 %d\n",
                       out[i].id, out[i].product_id, out[i].quantity);
        }
        else if (op == 4) {
            printf("iPhone 15 当前库存: %d\n", product_stock(&iphone));
        }
    }

    pool_destroy(&pool);
    om_destroy(&mgr);
    return 0;
}
