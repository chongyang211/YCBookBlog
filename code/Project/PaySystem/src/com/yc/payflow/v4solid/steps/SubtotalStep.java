package com.yc.payflow.v4solid.steps;

import com.yc.payflow.shared.Money;
import com.yc.payflow.v4solid.OrderItem;
import com.yc.payflow.v4solid.PromotionStep;
import com.yc.payflow.v4solid.SettlementContext;

/** Step 1：商品小计。 */
public class SubtotalStep implements PromotionStep {
    @Override public int order() { return 10; }
    @Override public String name() { return "Subtotal"; }

    @Override
    public void apply(SettlementContext ctx) {
        Money sum = Money.ZERO;
        for (OrderItem it : ctx.items()) {
            sum = sum.plus(it.subtotal());
        }
        ctx.setCurrentTotal(sum);
        ctx.setSubtotalSnapshot(sum);
        ctx.audit(name(), sum, sum);
    }
}
