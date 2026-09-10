package com.orders.entity;

public enum OrderStatus {
    PENDING,    // 已下单未支付
    PAID,       // 已支付
    SHIPPED,    // 已发货
    DELIVERED,  // 已送达
    CANCELLED   // 已取消
}
