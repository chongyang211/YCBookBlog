package com.orders.service;

import java.util.HashMap;
import java.util.Map;

/** v1：互斥锁版本（性能基线，教程 §8.1）*/
public class InventorySync implements Inventory {
    private final Map<String, Integer> stock = new HashMap<>();

    @Override
    public synchronized int get(String pid) {
        return stock.getOrDefault(pid, 0);
    }

    @Override
    public synchronized boolean deduct(String pid, int qty) {
        int cur = stock.getOrDefault(pid, 0);
        if (cur < qty) return false;
        stock.put(pid, cur - qty);
        return true;
    }

    @Override
    public synchronized void set(String pid, int v) { stock.put(pid, v); }
}
