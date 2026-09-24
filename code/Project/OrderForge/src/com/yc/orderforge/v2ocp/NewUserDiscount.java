package com.yc.orderforge.v2ocp;

import com.yc.orderforge.shared.Money;

/** 新人立减 30 元。 */
public class NewUserDiscount implements DiscountStrategy {

    @Override public String name() { return "新人立减"; }

    @Override
    public boolean applies(DiscountContext ctx) {
        return ctx.getOrder().isNewUser();
    }

    @Override
    public Money apply(Money current, DiscountContext ctx) {
        Money after = current.minus(Money.ofYuan(30));
        return after.isNegative() ? Money.ZERO : after;   // 立减不能把金额减成负数
    }
}
