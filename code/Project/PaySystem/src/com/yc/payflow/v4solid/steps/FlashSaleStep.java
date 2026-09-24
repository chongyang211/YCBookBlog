package com.yc.payflow.v4solid.steps;

import com.yc.payflow.shared.Money;
import com.yc.payflow.v4solid.OrderItem;
import com.yc.payflow.v4solid.PromotionStep;
import com.yc.payflow.v4solid.SettlementContext;

import java.time.LocalTime;

/**
 * Step 3：秒杀限时折扣（20:00-22:00 打 8 折）。
 *
 * ⚠️ V4 仍用 LocalTime.now()——V6 会把这里改造成注入 Clock。
 * 保留"时间依赖"作为 V6 的重构切入点。
 */
public class FlashSaleStep implements PromotionStep {
    private static final int START_HOUR = 20;
    private static final int END_HOUR = 22;

    @Override public int order() { return 30; }
    @Override public String name() { return "FlashSale"; }

    @Override
    public boolean applies(SettlementContext ctx) {
        for (OrderItem it : ctx.items()) {
            if (it.isFlashSale()) return true;
        }
        return false;
    }

    @Override
    public void apply(SettlementContext ctx) {
        int hour = LocalTime.now().getHour();
        if (hour < START_HOUR || hour >= END_HOUR) {
            ctx.audit(name() + "(不在时段)", Money.ZERO, ctx.currentTotal());
            return;
        }
        Money before = ctx.currentTotal();
        Money after = before.percentOff(20);
        ctx.setCurrentTotal(after);
        ctx.audit(name(), before.minus(after), after);
    }
}
