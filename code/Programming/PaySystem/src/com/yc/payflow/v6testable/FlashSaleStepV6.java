package com.yc.payflow.v6testable;

import com.yc.payflow.shared.Money;
import com.yc.payflow.v4solid.OrderItem;
import com.yc.payflow.v4solid.PromotionStep;
import com.yc.payflow.v4solid.SettlementContext;

/**
 * 秒杀 Step V6 版：依赖注入 Clock，秒杀时段 20:00-22:00 打 8 折。
 *
 * 【对比 V4 版本】：
 *   V4：LocalTime.now().getHour()  ← 隐式依赖 JVM 时钟
 *   V6：clock.nowTime().getHour()  ← 显式依赖注入的 Clock
 *
 * 【测试收益】：
 *   同一段业务规则的验证从"等到 20:00"变成"注入 fixed(21:00) 直接跑"
 */
public class FlashSaleStepV6 implements PromotionStep {

    private static final int START_HOUR = 20;
    private static final int END_HOUR = 22;

    private final Clock clock;

    public FlashSaleStepV6(Clock clock) {
        this.clock = clock;
    }

    @Override public int order() { return 30; }
    @Override public String name() { return "FlashSaleV6"; }

    @Override
    public boolean applies(SettlementContext ctx) {
        for (OrderItem it : ctx.items()) {
            if (it.isFlashSale()) return true;
        }
        return false;
    }

    @Override
    public void apply(SettlementContext ctx) {
        int hour = clock.nowTime().getHour();
        if (hour < START_HOUR || hour >= END_HOUR) {
            ctx.audit(name() + "(不在秒杀时段," + hour + "时)", Money.ZERO, ctx.currentTotal());
            return;
        }
        Money before = ctx.currentTotal();
        Money after = before.percentOff(20);
        ctx.setCurrentTotal(after);
        ctx.audit(name() + "(-20%," + hour + "时)", before.minus(after), after);
    }
}
