package com.orders.service;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

/** v3：ConcurrentHashMap + AtomicInteger（最优，教程 §8.1）
 * CAS 循环扣库存 = Redis 原子扣减的单机版 */
public class InventoryConcurrent implements Inventory {
    private final ConcurrentHashMap<String, AtomicInteger> stock = new ConcurrentHashMap<>();

    @Override
    public int get(String pid) {
        AtomicInteger v = stock.get(pid);
        return v == null ? 0 : v.get();
    }

    @Override
    public boolean deduct(String pid, int qty) {
        AtomicInteger v = stock.get(pid);
        if (v == null) return false;
        // ⭐ CAS 循环：核心原子扣减模式（与 C 版 atomic_compare_exchange 同源）
        while (true) {
            int cur = v.get();
            if (cur < qty) return false;
            if (v.compareAndSet(cur, cur - qty)) return true;
        }
    }

    @Override
    public void set(String pid, int v) {
        stock.computeIfAbsent(pid, k -> new AtomicInteger()).set(v);
    }
}
