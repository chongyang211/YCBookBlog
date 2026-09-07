#pragma once

#include "OrderStrategy.h"
#include "ThreadPool.h"
#include <future>

// 秒杀下单策略：每个下单作为一个 task 提交线程池，工人并发抢库存
class FlashStrategy : public OrderStrategy {
private:
    ThreadPool& pool;

public:
    explicit FlashStrategy(ThreadPool& p) : pool(p) {}

    std::vector<int> placeOrders(Product& product, OrderManager& mgr,
                                  int userId,
                                  const std::vector<int>& quantities) override {
        std::vector<std::future<int>> futs;
        futs.reserve(quantities.size());
        for (int qty : quantities) {
            futs.push_back(pool.submit([&product, &mgr, userId, qty]{
                if (product.tryDeduct(qty)) {
                    return mgr.addOrder(userId, product.getId(), qty);
                }
                return -1;
            }));
        }

        // 收集所有结果（按提交顺序）
        std::vector<int> result;
        result.reserve(futs.size());
        for (auto& f : futs) result.push_back(f.get());
        return result;
    }
};
