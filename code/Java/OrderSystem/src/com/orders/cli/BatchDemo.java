package com.orders.cli;

import com.orders.async.OrderPipeline;
import com.orders.service.InventoryConcurrent;
import com.orders.service.OrderManager;

import java.util.List;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

import com.orders.entity.Order;

/**
 * 阶段⑧ CompletableFuture 批量演示（教程 §9.2/§9.3）：
 * ① allOf 并发下 100 单
 * ② exceptionally 异常传播（qty=-1 与库存不足）
 */
public class BatchDemo {

    public static void main(String[] args) throws Exception {
        ExecutorService pool = Executors.newFixedThreadPool(8);
        InventoryConcurrent inv = new InventoryConcurrent();
        OrderManager mgr = new OrderManager();
        inv.set("P1", 1000);
        inv.set("P2", 1000);

        OrderPipeline pipeline = new OrderPipeline(inv, mgr, pool);

        System.out.println("========== ① allOf 批量：并发下 100 单 ==========");
        List<CompletableFuture<Order>> futures = IntStream.range(0, 100)
                .mapToObj(i -> pipeline.placeOrderAsync("U" + i, "P" + (i % 2 + 1), 1))
                .collect(Collectors.toList());

        CompletableFuture.allOf(futures.toArray(new CompletableFuture[0]))
                .thenRun(() -> {
                    long ok = futures.stream().filter(f -> {
                        try { return f.get() != null; } catch (Exception e) { return false; }
                    }).count();
                    System.out.println("成功 " + ok + " / 100，订单表 size=" + mgr.size());
                }).join();

        System.out.println("\n========== ② exceptionally 异常传播 ==========");
        Order bad = pipeline.placeOrderAsync("U999", "P1", -1).get();   // qty=-1 → validate 抛
        System.out.println("非法数量结果: " + bad);

        inv.set("P3", 0);
        Order oos = pipeline.placeOrderAsync("U998", "P3", 1).get();    // 库存 0 → deduct 抛
        System.out.println("库存不足结果: " + oos);

        pool.shutdown();
    }
}
