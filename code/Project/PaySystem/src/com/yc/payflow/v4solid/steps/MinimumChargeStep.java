package com.yc.payflow.v4solid.steps;

import com.yc.payflow.shared.Money;
import com.yc.payflow.v4solid.PromotionStep;
import com.yc.payflow.v4solid.SettlementContext;

/** Step 9（收尾）：最低消费兜底。 */
public class MinimumChargeStep implements PromotionStep {

    private static final Money MIN_CHARGE = Money.ofCents(1); // 至少 1 分

    @Override public int order() { return 100; }
    @Override public String name() { return "MinCharge"; }

    @Override
    public void apply(SettlementContext ctx) {
        Money before = ctx.currentTotal();
        if (before.compareTo(MIN_CHARGE) < 0) {
            ctx.setCurrentTotal(MIN_CHARGE);
            ctx.audit(name() + "(兜底)", MIN_CHARGE.minus(before), MIN_CHARGE);
        } else {
            ctx.audit(name() + "(无需兜底)", Money.ZERO, before);
        }
    }
}
