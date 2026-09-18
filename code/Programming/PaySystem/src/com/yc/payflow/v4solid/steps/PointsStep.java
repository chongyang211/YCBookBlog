package com.yc.payflow.v4solid.steps;

import com.yc.payflow.shared.Money;
import com.yc.payflow.v4solid.PromotionStep;
import com.yc.payflow.v4solid.SettlementContext;

/** Step 8：积分抵扣（100 积分 = 1 元），抵扣不能让金额为负。 */
public class PointsStep implements PromotionStep {

    private static final int POINTS_PER_YUAN = 100;

    @Override public int order() { return 80; }
    @Override public String name() { return "Points"; }

    @Override
    public boolean applies(SettlementContext ctx) { return ctx.usePoints() > 0; }

    @Override
    public void apply(SettlementContext ctx) {
        Money before = ctx.currentTotal();
        Money deduction = Money.ofYuan(ctx.usePoints() / (double) POINTS_PER_YUAN);
        // 抵扣不能超过订单金额
        Money actualDeduction = deduction.min(before);
        Money after = before.minus(actualDeduction);
        ctx.setCurrentTotal(after);
        ctx.audit(name() + "(-" + ctx.usePoints() + "积分)", actualDeduction, after);
    }
}
