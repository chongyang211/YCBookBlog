package com.orders.entity;

import java.time.Instant;

public class Order {
    private final long id;
    private final String productId;
    private final int quantity;
    private final double totalPrice;
    private volatile OrderStatus status;       // ⭐ 多线程会读，volatile 保证可见性
    private final Instant createdAt;

    public Order(long id, String productId, int quantity, double totalPrice) {
        this.id = id;
        this.productId = productId;
        this.quantity = quantity;
        this.totalPrice = totalPrice;
        this.status = OrderStatus.PENDING;
        this.createdAt = Instant.now();
    }

    public long getId()              { return id; }
    public String getProductId()     { return productId; }
    public int getQuantity()         { return quantity; }
    public double getTotalPrice()    { return totalPrice; }
    public OrderStatus getStatus()   { return status; }
    public Instant getCreatedAt()    { return createdAt; }
    public void setStatus(OrderStatus s) { this.status = s; }

    @Override
    public String toString() {
        return "Order{#" + id + ", " + productId + " x" + quantity
                + ", " + totalPrice + ", " + status + "}";
    }
}
