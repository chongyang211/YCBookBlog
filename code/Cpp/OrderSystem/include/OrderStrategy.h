#pragma once

#include "Product.h"
#include "OrderManager.h"
#include <vector>

// 下单策略接口（策略模式）：不同订单类型使用不同的下单策略
class OrderStrategy {
public:
    virtual ~OrderStrategy() = default;

    // 对给定商品下若干单，返回每个订单的 id（失败为 -1）
    virtual std::vector<int> placeOrders(
        Product& product,
        OrderManager& mgr,
        int userId,
        const std::vector<int>& quantities) = 0;
};
