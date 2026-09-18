package com.yc.orderforge.v2ocp;

import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;

/** 折扣上下文：策略的输入（订单 + 小计快照）。 */
public class DiscountContext {
    private final Order order;
    private final Money subtotal;

    public DiscountContext(Order order, Money subtotal) {
        this.order = order;
        this.subtotal = subtotal;
    }

    public Order getOrder()   { return order; }
    public Money getSubtotal(){ return subtotal; }
}
