package com.yc.orderforge.v2ocp;

import com.yc.orderforge.shared.Money;

/** 优惠券立减（有券才适用）。 */
public class CouponDiscount implements DiscountStrategy {

    @Override public String name() { return "优惠券"; }

    @Override
    public boolean applies(DiscountContext ctx) {
        return ctx.getOrder().getCoupon() != null;
    }

    @Override
    public Money apply(Money current, DiscountContext ctx) {
        // 抵扣不能超过当前金额（min 兜底）
        return current.min(ctx.getOrder().getCoupon());
    }
}
