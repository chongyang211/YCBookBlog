package com.kv.test;

import com.kv.aof.AofWriter;
import com.kv.aof.FlushPolicy;
import com.kv.store.IntValue;
import com.kv.store.Store;
import com.kv.store.StringValue;

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.concurrent.CountDownLatch;

/**
 * 性能压测（阶段⑪ Step 11.6）—— 复现教学文档的三组数字：
 *
 *   1. 单线程 10 万次 SET（纯内存）
 *   2. 100 线程 × 1 万次 = 100 万次 SET（CHM 并发）
 *   3. AOF 三档刷盘策略对比（EVERY_WRITE / EVERY_SECOND / NEVER）
 *
 * 结论：内存吞吐数十万 QPS；AOF 是瓶颈——fsync 才是持久化的真正代价；
 *      业务建议 EVERY_SECOND（吞吐 + 安全的最佳平衡，与 Redis 默认一致）。
 *
 * 运行：java -cp out com.kv.test.Benchmark
 */
public class Benchmark {

    public static void main(String[] args) throws Exception {
        System.out.println("====== Mini-KV 性能压测 ======\n");

        // ---- 1. 单线程 10 万次 SET（内存） ----
        {
            Store store = new Store();
            long t0 = System.nanoTime();
            for (int i = 0; i < 100_000; i++) {
                store.set("k" + i, new StringValue("v" + i));
            }
            long ms = elapsedMs(t0);
            System.out.printf("单线程 10 万次 SET:        %5d ms  ≈ %,d QPS%n", ms, qps(100_000, ms));
        }

        // ---- 2. 100 线程 × 1 万次 SET（CHM 并发） ----
        {
            Store store = new Store();
            int threads = 100, ops = 10_000;
            CountDownLatch done = new CountDownLatch(threads);
            long t0 = System.nanoTime();
            for (int t = 0; t < threads; t++) {
                int id = t;
                new Thread(() -> {
                    try {
                        for (int i = 0; i < ops; i++) {
                            store.set("k" + (id * ops + i), new IntValue(id));
                        }
                    } finally { done.countDown(); }
                }).start();
            }
            done.await();
            long ms = elapsedMs(t0);
            System.out.printf("100 线程 100 万次 SET(CHM): %5d ms  ≈ %,d QPS（size=%d）%n",
                    ms, qps(threads * ops, ms), store.size());
        }

        // ---- 3. AOF 三档刷盘（各 1 万条 append） ----
        System.out.println();
        for (FlushPolicy policy : FlushPolicy.values()) {
            Path aof = Files.createTempFile("bench-aof-", ".aof");
            try {
                long t0 = System.nanoTime();
                try (AofWriter w = new AofWriter(aof, policy)) {
                    for (int i = 0; i < 10_000; i++) {
                        w.append("SET k" + i + " v" + i);
                        if (policy == FlushPolicy.EVERY_SECOND && i % 1000 == 0) {
                            w.flushAndSync();       // 模拟每秒批量的周期性刷盘
                        }
                    }
                }
                long ms = elapsedMs(t0);
                System.out.printf("AOF %-12s 1 万条:   %5d ms  ≈ %,d QPS%n",
                        policy, ms, qps(10_000, ms));
            } finally {
                Files.deleteIfExists(aof);
            }
        }

        System.out.println("\n结论：fsync 是持久化的真正代价；生产推荐 EVERY_SECOND（与 Redis 默认一致）。");
    }

    private static long elapsedMs(long t0) {
        return Math.max((System.nanoTime() - t0) / 1_000_000, 1);
    }

    private static long qps(long ops, long ms) {
        return ops * 1000 / ms;
    }
}
