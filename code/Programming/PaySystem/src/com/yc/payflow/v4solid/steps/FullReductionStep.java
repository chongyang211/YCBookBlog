package com.yc.payflow.v4solid.steps;

import com.yc.payflow.shared.Money;
import com.yc.payflow.v4solid.PromotionStep;
import com.yc.payflow.v4solid.SettlementContext;

/**
 * Step 2：阶梯满减（V4 修复了 V0 的顺序错 bug——用高到低遍历）。
 *
 * V0 bug 复盘：if(total >= 300) 拦截了 >= 500 的订单，导致少减 50 元。
 * V4 修复：从高阈值到低阈值遍历，命中即停止。
 */
public class FullReductionStep implements PromotionStep {

    /** 阶梯定义：满 X 减 Y（按阈值降序） */
    private static final long[][] LADDERS = {
        {100000, 25000},  // 满 1000 减 250
        {50000,  10000},  // 满  500 减 100
        {30000,  5000}    // 满  300 减  50
    };

    @Override public int order() { return 20; }
    @Override public String name() { return "FullReduction"; }

    @Override
    public void apply(SettlementContext ctx) {
        Money total = ctx.currentTotal();
        for (long[] rung : LADDERS) {
            Money threshold = Money.ofCents(rung[0]);
            Money discount = Money.ofCents(rung[1]);
            if (total.greaterOrEqual(threshold)) {
                Money after = total.minus(discount);
                ctx.setCurrentTotal(after);
                ctx.audit(name(), discount, after);
                return;
            }
        }
    }
}
