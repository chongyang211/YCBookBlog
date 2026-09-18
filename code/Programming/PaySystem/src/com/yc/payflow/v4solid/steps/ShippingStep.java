package com.yc.payflow.v4solid.steps;

import com.yc.payflow.shared.Money;
import com.yc.payflow.v4solid.PromotionStep;
import com.yc.payflow.v4solid.SettlementContext;

/** Step 6：运费（满 99 包邮）。 */
public class ShippingStep implements PromotionStep {

    private static final Money FREE_THRESHOLD = Money.ofYuan(99);
    private static final Money FLAT_FEE = Money.ofYuan(12);

    @Override public int order() { return 60; }
    @Override public String name() { return "Shipping"; }

    @Override
    public void apply(SettlementContext ctx) {
        Money before = ctx.currentTotal();
        if (before.greaterOrEqual(FREE_THRESHOLD)) {
            ctx.audit(name() + "(包邮)", Money.ZERO, before);
            return;
        }
        Money after = before.plus(FLAT_FEE);
        ctx.setCurrentTotal(after);
        ctx.audit(name(), FLAT_FEE, after);
    }
}
