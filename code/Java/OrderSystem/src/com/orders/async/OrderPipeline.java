package com.orders.async;

import com.orders.entity.Order;
import com.orders.service.InventoryConcurrent;
import com.orders.service.OrderManager;

import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;

/**
 * CompletableFuture 异步编排（教程 §09）：
 * 下单四阶段流水线——校验 → 扣库存 → 写订单 → 发通知
 * 异常沿链传播，一处 exceptionally 兜底所有阶段 */
public class OrderPipeline {

    private final InventoryConcurrent inventory;
    private final OrderManager orders;
    private final ExecutorService executor;

    public OrderPipeline(InventoryConcurrent inv, OrderManager orders, ExecutorService exec) {
        this.inventory = inv;
        this.orders    = orders;
        this.executor  = exec;
    }

    public CompletableFuture<Order> placeOrderAsync(String userId, String productId, int qty) {
        return CompletableFuture
                .supplyAsync(() -> validate(userId, productId, qty), executor)
                .thenApplyAsync(unused -> deduct(productId, qty), executor)
                .thenApplyAsync(stockOk -> writeOrder(productId, qty), executor)
                .thenApplyAsync(order -> { notify(order); return order; }, executor)
                .exceptionally(ex -> {
                    System.err.println("下单失败: " + ex.getMessage());
                    return null;
                });
    }

    private boolean validate(String userId, String productId, int qty) {
        if (qty <= 0) throw new IllegalArgumentException("数量必须 > 0");
        return true;
    }

    private boolean deduct(String productId, int qty) {
        if (!inventory.deduct(productId, qty)) {
            throw new IllegalStateException("库存不足: " + productId);
        }
        return true;
    }

    private Order writeOrder(String productId, int qty) {
        return orders.placeOrder(productId, qty, 9.9);
    }

    private void notify(Order o) {
        // 模拟发通知（IO 操作）
        try { Thread.sleep(10); } catch (InterruptedException e) { Thread.currentThread().interrupt(); }
        System.out.println("[通知] 用户订单已受理: " + o);
    }
}
