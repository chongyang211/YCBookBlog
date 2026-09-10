package com.orders.service;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.locks.ReentrantReadWriteLock;

/** v2：读写锁版本——读多写少场景读操作并发（教程 §8.1）*/
public class InventoryRWLock implements Inventory {
    private final Map<String, Integer> stock = new HashMap<>();
    private final ReentrantReadWriteLock rw = new ReentrantReadWriteLock();
    private final ReentrantReadWriteLock.ReadLock  rl = rw.readLock();
    private final ReentrantReadWriteLock.WriteLock wl = rw.writeLock();

    @Override
    public int get(String pid) {
        rl.lock();
        try { return stock.getOrDefault(pid, 0); }
        finally { rl.unlock(); }
    }

    @Override
    public boolean deduct(String pid, int qty) {
        wl.lock();
        try {
            int cur = stock.getOrDefault(pid, 0);
            if (cur < qty) return false;
            stock.put(pid, cur - qty);
            return true;
        } finally { wl.unlock(); }
    }

    @Override
    public void set(String pid, int v) {
        wl.lock();
        try { stock.put(pid, v); }
        finally { wl.unlock(); }
    }
}
