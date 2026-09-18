package com.yc.payflow.v4solid.steps;

import com.yc.payflow.shared.Money;
import com.yc.payflow.v4solid.OrderItem;
import com.yc.payflow.v4solid.PromotionStep;
import com.yc.payflow.v4solid.SettlementContext;

/** Step 7：跨境税（11.9%）。 */
public class CrossBorderTaxStep implements PromotionStep {

    private static final double TAX_RATE = 0.119;

    @Override public int order() { return 70; }
    @Override public String name() { return "CrossBorderTax"; }

    @Override
    public boolean applies(SettlementContext ctx) {
        for (OrderItem it : ctx.items()) {
            if (it.isCrossBorder()) return true;
        }
        return false;
    }

    @Override
    public void apply(SettlementContext ctx) {
        Money before = ctx.currentTotal();
        Money tax = before.multiply(TAX_RATE);
        Money after = before.plus(tax);
        ctx.setCurrentTotal(after);
        ctx.audit(name(), tax, after);
    }
}
