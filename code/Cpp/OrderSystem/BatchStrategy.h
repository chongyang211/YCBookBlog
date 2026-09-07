#pragma once

#include "include/OrderStrategy.h"
#include "include/ThreadPool.h"
#include <future>
#include <algorithm>

// 批量下单策略：分批提交，组内并发、组间串行，控制内存压力
class BatchStrategy : public OrderStrategy {
private:
    ThreadPool& pool;
    size_t      batchSize;

public:
    BatchStrategy(ThreadPool& p, size_t bs = 10) : pool(p), batchSize(bs) {}

    std::vector<int> placeOrders(Product& product, OrderManager& mgr,
                                  int userId,
                                  const std::vector<int>& quantities) override {
        std::vector<int> result(quantities.size(), -1);

        // 每 batchSize 个为一组，组内并发，组间串行
        for (size_t i = 0; i < quantities.size(); i += batchSize) {
            size_t end = std::min(i + batchSize, quantities.size());

            std::vector<std::future<int>> futs;
            futs.reserve(end - i);
            for (size_t j = i; j < end; ++j) {
                int qty = quantities[j];
                futs.push_back(pool.submit([&product, &mgr, userId, qty]{
                    if (product.tryDeduct(qty)) {
                        return mgr.addOrder(userId, product.getId(), qty);
                    }
                    return -1;
                }));
            }
            // 等本批完成后再开下一批
            for (size_t j = i; j < end; ++j) {
                result[j] = futs[j - i].get();
            }
        }
        return result;
    }
};
