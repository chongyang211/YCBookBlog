package com.yc.orderforge.v2ocp;

import com.yc.orderforge.shared.Money;

/** 会员 9 折（仅 GOLD；注册在满减之后 = 折扣作用于满减后的金额）。 */
public class VipDiscount implements DiscountStrategy {

    @Override public String name() { return "会员9折"; }

    @Override
    public boolean applies(DiscountContext ctx) {
        return ctx.getOrder().isVip();
    }

    @Override
    public Money apply(Money current, DiscountContext ctx) {
        return current.multiply(0.9);
    }
}
