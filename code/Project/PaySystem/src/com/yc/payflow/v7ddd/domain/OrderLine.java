package com.yc.payflow.v7ddd.domain;

import com.yc.payflow.shared.Money;

/**
 * OrderLine 订单行——实体（有身份但生命周期在聚合内）。
 */
public class OrderLine {
    private final String sku;
    private final String name;
    private final Money unitPrice;
    private final int quantity;

    public OrderLine(String sku, String name, Money unitPrice, int quantity) {
        if (quantity <= 0) throw new IllegalArgumentException("数量必须为正");
        this.sku = sku;
        this.name = name;
        this.unitPrice = unitPrice;
        this.quantity = quantity;
    }

    public Money subtotal() { return unitPrice.multiply(quantity); }
    public String sku() { return sku; }
    public String name() { return name; }
    public int quantity() { return quantity; }
    public Money unitPrice() { return unitPrice; }
}
