#include "include/ThreadPool.h"
#include "include/OrderManager.h"
#include "include/Product.h"
#include "include/FlashStrategy.h"
#include <chrono>
#include <iostream>
#include <atomic>
#include <thread>

// 最终验收：万级并发压测 + 一致性校验
int main() {
    constexpr int TOTAL_USERS     = 1000;
    constexpr int ORDERS_PER_USER = 10;
    constexpr int INITIAL_STOCK   = 5000;        // 故意 < 总请求 10000

    ThreadPool   pool(std::thread::hardware_concurrency());
    OrderManager mgr;
    Product      iphone(1, "iPhone 15", 7999.0, INITIAL_STOCK);

    FlashStrategy flash(pool);
    std::vector<int> qtys(ORDERS_PER_USER, 1);
    std::atomic<int> totalSuccess{0};

    auto t0 = std::chrono::steady_clock::now();

    // 启动 1000 个用户线程，每人下 10 单
    std::vector<std::thread> users;
    users.reserve(TOTAL_USERS);
    for (int u = 0; u < TOTAL_USERS; ++u) {
        users.emplace_back([&, u]{
            auto ids = flash.placeOrders(iphone, mgr, u, qtys);
            for (int id : ids) if (id > 0) totalSuccess++;
        });
    }
    for (auto& t : users) t.join();

    auto t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    int total    = TOTAL_USERS * ORDERS_PER_USER;
    int success  = totalSuccess.load();
    int leftover = iphone.getStock();

    std::cout << "========================================\n";
    std::cout << "总请求数:    " << total << "\n";
    std::cout << "成功订单:    " << success << "\n";
    std::cout << "剩余库存:    " << leftover << "\n";
    std::cout << "总耗时:     " << ms << " ms\n";
    std::cout << "QPS:        " << (total * 1000.0 / ms) << "\n";

    // 关键一致性验证：成功 + 剩余 == 初始（零超卖、零漏单）
    bool consistent = (success + leftover == INITIAL_STOCK);
    std::cout << "一致性校验: 成功(" << success << ") + 剩余(" << leftover
              << ") == 初始(" << INITIAL_STOCK << ") ? "
              << (consistent ? "PASS" : "FAIL：超卖或漏单！") << "\n";
    std::cout << "========================================\n";
    return consistent ? 0 : 1;          // 一致则退出码 0
}
