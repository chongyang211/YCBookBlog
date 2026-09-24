package com.yc.payflow.v4solid.steps;

import com.yc.payflow.shared.Money;
import com.yc.payflow.v4solid.PromotionStep;
import com.yc.payflow.v4solid.SettlementContext;

/**
 * Step 4：优惠券抵扣。
 *
 * V4 修复了 V0 的重复减免 bug——优惠券只作用一次，不与满减重复叠加。
 * 优惠券 ID 规则：
 *   C10-xxx → 满 100 减 10
 *   C30-xxx → 满 300 减 30
 *   C50-xxx → 满 500 减 50
 */
public class CouponStep implements PromotionStep {

    @Override public int order() { return 40; }
    @Override public String name() { return "Coupon"; }

    @Override
    public boolean applies(SettlementContext ctx) {
        return ctx.couponId() != null && !ctx.couponId().isEmpty();
    }

    @Override
    public void apply(SettlementContext ctx) {
        String coupon = ctx.couponId();
        Money total = ctx.currentTotal();
        Money discount = Money.ZERO;
        Money threshold = Money.ZERO;

        if (coupon.startsWith("C10")) {
            threshold = Money.ofYuan(100); discount = Money.ofYuan(10);
        } else if (coupon.startsWith("C30")) {
            threshold = Money.ofYuan(300); discount = Money.ofYuan(30);
        } else if (coupon.startsWith("C50")) {
            threshold = Money.ofYuan(500); discount = Money.ofYuan(50);
        }

        if (!discount.isZero() && total.greaterOrEqual(threshold)) {
            Money after = total.minus(discount);
            ctx.setCurrentTotal(after);
            ctx.audit(name() + "(" + coupon + ")", discount, after);
        } else {
            ctx.audit(name() + "(未满足门槛)", Money.ZERO, total);
        }
    }
}
