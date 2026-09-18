package com.yc.payflow.v4solid.steps;

import com.yc.payflow.shared.Money;
import com.yc.payflow.v4solid.PromotionStep;
import com.yc.payflow.v4solid.SettlementContext;

/**
 * Step 5：会员等级折扣（复用 V1 的多态思路，此处内联百分比表）。
 *
 * V4 修复了 V0 的"GOLD 分支重复减满减金额"bug——本 step 只做一次乘法折扣。
 */
public class VipDiscountStep implements PromotionStep {

    private static final double[] OFF_PERCENT = {0, 5, 10, 15}; // 依 vipLevel 索引

    @Override public int order() { return 50; }
    @Override public String name() { return "VipDiscount"; }

    @Override
    public boolean applies(SettlementContext ctx) {
        return ctx.vipLevel() > 0 && ctx.vipLevel() < OFF_PERCENT.length;
    }

    @Override
    public void apply(SettlementContext ctx) {
        Money before = ctx.currentTotal();
        double percent = OFF_PERCENT[ctx.vipLevel()];
        Money after = before.percentOff(percent);
        ctx.setCurrentTotal(after);
        ctx.audit(name() + "(vip=" + ctx.vipLevel() + ",-" + percent + "%)",
                  before.minus(after), after);
    }
}
