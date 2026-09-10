package com.orders.cli;

import com.orders.entity.Order;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.CountDownLatch;

/**
 * 阶段②造 BUG 演示（教程 §03）：竞态条件 + ArrayList 崩溃
 * 10 线程并发下单 → 亲眼看到"唯一 ID 数 < 订单总数"（++lastId 撞号 + add 丢数据）
 * 多跑几次每次结果都不一样——并发 BUG 不可重现是常态 */
public class RaceDemo {

    /** ⚠️ 故意无锁的 buggy 版（内联演示，工程里 OrderManager 已修复）*/
    static class BuggyOrderManager {
        long lastId = 0;
        final List<Order> orders = new ArrayList<>();

        Order placeOrder(String productId, int qty, double price) {
            long id = ++lastId;                       // ⚠️ 读-改-写不原子
            Order o = new Order(id, productId, qty, price * qty);
            orders.add(o);                            // ⚠️ ArrayList 并发 add
            return o;
        }
    }

    public static void main(String[] args) throws InterruptedException {
        BuggyOrderManager mgr = new BuggyOrderManager();
        int threads = 10;
        int perThread = 100;
        CountDownLatch done = new CountDownLatch(threads);

        long start = System.nanoTime();
        for (int t = 0; t < threads; t++) {
            new Thread(() -> {
                try {
                    for (int i = 0; i < perThread; i++) {
                        mgr.placeOrder("P" + (i % 10), 1, 9.9);
                    }
                } finally {
                    done.countDown();
                }
            }, "Worker-" + t).start();
        }
        done.await();
        long elapsed = System.nanoTime() - start;

        Set<Long> unique = new HashSet<>();
        for (Order o : mgr.orders) unique.add(o.getId());

        System.out.printf("[%d 线程并发-无锁] %d 单 耗时 %.2f ms%n",
                threads, threads * perThread, elapsed / 1_000_000.0);
        System.out.printf("订单总数=%d，唯一 ID 数=%d %s%n",
                mgr.orders.size(), unique.size(),
                mgr.orders.size() == unique.size() ? "无重复" : "有重复！（竞态条件）");
    }
}
