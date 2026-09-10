package com.orders.cli;

import com.orders.async.OrderPipeline;
import com.orders.concurrent.MyThreadPool;
import com.orders.pool.RejectPolicy;
import com.orders.service.InventoryConcurrent;
import com.orders.service.OrderManager;

import java.util.Scanner;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicLong;

/**
 * 端到端订单系统（教程 §10）：
 * CLI 生产者 + 自实现池作前置缓冲 + CompletableFuture 流水线 + QPS 监控 + 优雅关停
 */
public class Main {
    public static void main(String[] args) throws Exception {
        // ===== 基础设施 =====
        MyThreadPool myPool = new MyThreadPool(
                4, 8, 30_000,
                new ArrayBlockingQueue<>(100),
                RejectPolicy.callerRuns());

        ExecutorService bizPool = Executors.newFixedThreadPool(8);
        ScheduledExecutorService monitor = Executors.newSingleThreadScheduledExecutor();

        InventoryConcurrent inv = new InventoryConcurrent();
        OrderManager mgr = new OrderManager();
        inv.set("P1", 1000); inv.set("P2", 1000); inv.set("P3", 1000);

        OrderPipeline pipeline = new OrderPipeline(inv, mgr, bizPool);
        AtomicLong qpsCounter = new AtomicLong();

        // ===== 监控线程：每秒打印一次 QPS =====
        monitor.scheduleAtFixedRate(() -> {
            long count = qpsCounter.getAndSet(0);
            System.out.printf("[监控] QPS=%d 已下单=%d 库存P1=%d P2=%d P3=%d%n",
                    count, mgr.size(), inv.get("P1"), inv.get("P2"), inv.get("P3"));
        }, 1, 1, TimeUnit.SECONDS);

        // ⭐ AtomicBoolean：volatile 语义（教程原版 boolean[] 数组元素无 volatile 语义）
        final AtomicBoolean running = new AtomicBoolean(true);

        // ===== 生产者：CLI 读输入 =====
        System.out.println("命令格式：<productId> <qty>，例如 'P1 3'。输入 'quit' 退出，'auto' 启动压测");
        Scanner sc = new Scanner(System.in);

        while (running.get() && sc.hasNextLine()) {
            String line = sc.nextLine().trim();
            if (line.isEmpty()) continue;
            if (line.equalsIgnoreCase("quit")) { running.set(false); break; }

            if (line.equalsIgnoreCase("auto")) {
                for (int i = 0; i < 1000; i++) {
                    String pid = "P" + (i % 3 + 1);
                    myPool.execute(() ->
                            pipeline.placeOrderAsync("Auto", pid, 1)
                                    .thenRun(qpsCounter::incrementAndGet));
                }
                continue;
            }

            String[] parts = line.split("\\s+");
            if (parts.length != 2) { System.out.println("用法: <pid> <qty>"); continue; }
            String pid = parts[0];
            int qty = Integer.parseInt(parts[1]);

            myPool.execute(() ->
                    pipeline.placeOrderAsync("CLI", pid, qty)
                            .thenRun(qpsCounter::incrementAndGet));
        }

        // ===== 优雅关闭（顺序：前置池 → 业务池 → 监控）=====
        System.out.println("\n开始优雅关闭...");
        myPool.shutdown();
        myPool.awaitTermination(5_000);
        bizPool.shutdown();
        bizPool.awaitTermination(5, TimeUnit.SECONDS);
        monitor.shutdown();
        System.out.println("已退出。共下单 " + mgr.size());
    }
}
