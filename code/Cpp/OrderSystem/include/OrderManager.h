#pragma once

#include "Order.h"
#include <vector>
#include <shared_mutex>
#include <atomic>
#include <optional>

// 订单管理器：订单存储 + 多线程读写
// - 写（addOrder）用独占锁，读（findOrder/ordersByUser/size）用共享锁
// - 订单 ID 用 atomic 自增，避免两个线程拿到同一 ID
class OrderManager {
private:
    std::vector<Order>           orders;
    mutable std::shared_mutex    mtx;            // 读写锁
    std::atomic<int>             nextId{1};      // ID 自增用 atomic

public:
    int addOrder(int userId, int productId, int qty) {
        Order o(nextId.fetch_add(1), userId, productId, qty);
        std::unique_lock lock(mtx);              // 写：独占锁
        orders.push_back(std::move(o));
        return o.id;
    }

    std::optional<Order> findOrder(int id) const {
        std::shared_lock lock(mtx);              // 读：共享锁
        for (const auto& o : orders) {
            if (o.id == id) return o;
        }
        return std::nullopt;
    }

    std::vector<Order> ordersByUser(int userId) const {
        std::shared_lock lock(mtx);              // 读锁
        std::vector<Order> result;
        for (const auto& o : orders) {
            if (o.userId == userId) result.push_back(o);
        }
        return result;
    }

    size_t size() const {
        std::shared_lock lock(mtx);
        return orders.size();
    }
};
