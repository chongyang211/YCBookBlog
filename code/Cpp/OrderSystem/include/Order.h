#pragma once

// 订单实体：纯数据载体
struct Order {
    int id;
    int userId;
    int productId;
    int quantity;

    Order() = default;
    Order(int i, int u, int p, int q)
        : id(i), userId(u), productId(p), quantity(q) {}
};
