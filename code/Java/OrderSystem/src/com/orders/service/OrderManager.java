package com.orders.service;

import com.orders.entity.Order;
import com.orders.entity.OrderStatus;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Optional;
import java.util.Set;

/**
 * 订单管理（教程阶段③最终版：双小锁同步块）
 * 演进史：阶段①单线程无锁（会竞态）→ 方法级 synchronized（38ms）→ 本版双小锁（22ms）
 *
 * synchronized 三铁律（§4.0）：
 *   1. 私有 final 锁对象（不用 this / Class，防外部干扰）
 *   2. 细粒度同步块（只锁真正共享的部分）
 *   3. IO / 计算必须挪出锁外
 */
public class OrderManager {

    private long lastId = 0;
    private final Object idLock = new Object();         // ⭐ 私有 final 锁
    private final List<Order> orders = new ArrayList<>();
    private final Object listLock = new Object();       // ⭐ 私有 final 锁

    public Order placeOrder(String productId, int qty, double price) {
        long id;
        synchronized (idLock) { id = ++lastId; }        // 第 1 把小锁

        // ⭐ 中间这部分不在任何锁里：构造对象 + 计算 totalPrice
        Order order = new Order(id, productId, qty, price * qty);

        synchronized (listLock) { orders.add(order); }  // 第 2 把小锁
        return order;
    }

    public int size() {
        synchronized (listLock) { return orders.size(); }
    }

    public Set<Long> uniqueIds() {
        synchronized (listLock) {                       // 遍历集合也要锁
            Set<Long> set = new HashSet<>();
            for (Order o : orders) set.add(o.getId());
            return set;
        }
    }

    public Optional<Order> findById(long id) {
        synchronized (listLock) {
            for (Order o : orders) {
                if (o.getId() == id) return Optional.of(o);
            }
            return Optional.empty();
        }
    }

    public boolean cancel(long id) {
        return findById(id)
                .map(o -> { o.setStatus(OrderStatus.CANCELLED); return true; })
                .orElse(false);
    }
}
