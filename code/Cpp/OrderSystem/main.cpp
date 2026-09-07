
#include "include/ThreadPool.h"
#include "include/OrderManager.h"
#include "include/Product.h"
#include "include/NormalStrategy.h"
#include "include/FlashStrategy.h"
#include "include/BatchStrategy.h"
#include "include/Logger.h"
#include <iostream>

int main() {
    ThreadPool   pool(4);
    OrderManager mgr;
    Product      iphone(1, "iPhone 15", 7999.0, 100);

    NormalStrategy normal;
    FlashStrategy  flash(pool);
    BatchStrategy  batch(pool, 10);

    while (true) {
        std::cout << "\n========== 订单系统 ==========\n"
                     "1=普通下单  2=秒杀下单  3=批量下单\n"
                     "4=查询订单  5=查看库存  0=退出\n> ";
        int op;
        if (!(std::cin >> op)) break;
        if (op == 0) break;

        try {
            if (op == 1 || op == 2 || op == 3) {
                int userId, count;
                std::cout << "用户 ID: ";    std::cin >> userId;
                std::cout << "下单次数: ";    std::cin >> count;

                std::vector<int> qtys(count, 1);
                OrderStrategy& s = (op == 1)
                    ? static_cast<OrderStrategy&>(normal)
                    : (op == 2)
                        ? static_cast<OrderStrategy&>(flash)
                        : static_cast<OrderStrategy&>(batch);

                auto ids = s.placeOrders(iphone, mgr, userId, qtys);
                int success = 0;
                for (int id : ids) if (id > 0) success++;
                Logger::info("用户 ", userId, " 提交 ", count, " 单，成功 ", success);
            }
            else if (op == 4) {
                int userId;
                std::cout << "用户 ID: "; std::cin >> userId;
                auto v = mgr.ordersByUser(userId);
                std::cout << "用户 " << userId << " 共 " << v.size() << " 单：\n";
                for (const auto& o : v) {
                    std::cout << "  订单 #" << o.id
                              << " - 商品 " << o.productId
                              << " - 数量 " << o.quantity << "\n";
                }
            }
            else if (op == 5) {
                std::cout << "iPhone 15 当前库存: " << iphone.getStock() << "\n";
            }
        } catch (const std::exception& e) {
            std::cout << "ERR " << e.what() << "\n";
        }
    }
    return 0;
}
