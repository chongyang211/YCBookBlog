package com.yc.payflow.v4solid;

import com.yc.payflow.shared.Money;

/**
 * 订单行——领域对象，取代 V0 的 double[] + int[] 数据泥团。
 */
public class OrderItem {
    private final String sku;
    private final String name;
    private final Money unitPrice;
    private final int quantity;
    private final boolean crossBorder;
    private final boolean flashSale;

    public OrderItem(String sku, String name, Money unitPrice, int quantity,
                     boolean crossBorder, boolean flashSale) {
        if (quantity <= 0) throw new IllegalArgumentException("数量必须为正: " + quantity);
        if (unitPrice == null || unitPrice.isNegative()) {
            throw new IllegalArgumentException("单价不能为负: " + unitPrice);
        }
        this.sku = sku;
        this.name = name;
        this.unitPrice = unitPrice;
        this.quantity = quantity;
        this.crossBorder = crossBorder;
        this.flashSale = flashSale;
    }

    public Money subtotal() {
        return unitPrice.multiply(quantity);
    }

    public String sku() { return sku; }
    public String name() { return name; }
    public Money unitPrice() { return unitPrice; }
    public int quantity() { return quantity; }
    public boolean isCrossBorder() { return crossBorder; }
    public boolean isFlashSale() { return flashSale; }
}
