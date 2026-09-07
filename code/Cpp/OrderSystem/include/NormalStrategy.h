#pragma once

#include "OrderStrategy.h"

// 普通下单策略：单线程顺序处理
class NormalStrategy : public OrderStrategy {
public:
    std::vector<int> placeOrders(Product& product, OrderManager& mgr,
                                  int userId,
                                  const std::vector<int>& quantities) override {
        std::vector<int> orderIds;
        orderIds.reserve(quantities.size());
        for (int qty : quantities) {
            if (product.tryDeduct(qty)) {
                orderIds.push_back(mgr.addOrder(userId, product.getId(), qty));
            } else {
                orderIds.push_back(-1);             // 失败标记
            }
        }
        return orderIds;
    }
};
