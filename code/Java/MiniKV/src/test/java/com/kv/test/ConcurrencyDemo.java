package com.kv.test;

import com.kv.store.IntValue;
import com.kv.store.Store;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;
import java.util.concurrent.CountDownLatch;

/**
 * 造 BUG #4 现场（阶段⑨ Step 9.1）—— 100 线程并发操作 HashMap。
 *
 * 两个症状：
 *   1. CME：Reader 线程遍历 keys 时，其他线程并发 put/remove → ConcurrentModificationException
 *   2. 数据丢失：HashMap 并发 put 非原子，最终 size < 100（本机多次跑会看到不同数字）
 *
 * 对照：最终版 Store（ConcurrentHashMap + compute）在同样压力下不丢数据、不抛 CME
 *      —— 见 testConcurrent（AllTests）。
 *
 * 运行：java -cp out com.kv.test.ConcurrencyDemo
 */
public class ConcurrencyDemo {

    /** 复刻阶段③的 HashMap 版 Store（危险！仅供造 BUG 演示） */
    static class UnsafeStore {
        private final Map<String, com.kv.store.Entry> data = new HashMap<>();

        void set(String key, com.kv.store.Value v) {
            com.kv.store.Entry old = data.get(key);
            data.put(key, (old == null) ? com.kv.store.Entry.of(v) : old.withValue(v));
        }

        void del(String key) { data.remove(key); }

        Set<String> keys() { return new TreeSet<>(data.keySet()); }

        int size() { return data.size(); }
    }

    public static void main(String[] args) throws InterruptedException {
        UnsafeStore store = new UnsafeStore();       // ⚠️ 内部是 HashMap

        int threads = 100;
        int opsPerThread = 1000;
        CountDownLatch done = new CountDownLatch(threads);

        for (int t = 0; t < threads; t++) {
            int id = t;
            new Thread(() -> {
                try {
                    for (int i = 0; i < opsPerThread; i++) {
                        if (i % 2 == 0) {
                            store.set("k" + (i % 100), new IntValue(id));
                        } else {
                            store.del("k" + (i % 100));
                        }
                    }
                } finally {
                    done.countDown();
                }
            }, "T-" + id).start();
        }

        // ⚠️ 同时另起一个线程持续遍历 keys —— 触发 CME
        int[] cmeCount = {0};
        Thread reader = new Thread(() -> {
            while (done.getCount() > 0) {
                try {
                    for (String k : store.keys()) {
                        // 啥也不做，只为触发遍历
                    }
                } catch (Exception e) {
                    cmeCount[0]++;
                    System.err.println("⚠️ 遍历异常: " + e);
                }
            }
        }, "Reader");
        reader.start();

        done.await();
        reader.join(1000);
        System.out.println();
        System.out.println("CME 次数: " + cmeCount[0]);
        System.out.println("最终大小: " + store.size() + "   ← ⚠️ 期望 100，HashMap 并发丢数据");
        System.out.println();
        System.out.println("✅ 修复方案（三选一）：");
        System.out.println("   A. synchronized        —— 简单但读也串行（基线性能）");
        System.out.println("   B. ReentrantReadWriteLock —— 读多写少时 2.7× 加速");
        System.out.println("   C. ConcurrentHashMap   —— 本项目终态，约 9× 加速（Store.java 已用）");
    }
}
