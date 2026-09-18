package com.yc.orderforge.v1srp;

import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;

/**
 * 计价服务（V1 拆出）。
 *
 * ⚠️ 注意：V1 只做结构拆分，不做业务修复——
 *    计价逻辑原封不动从 V0 搬过来，BUG 1（会员折扣先于满减）仍在！
 *    这是有意为之的教学设计：SRP 让"计价"可以被独立测试、独立修改，
 *    但"改对它"是 V2（OCP 策略链）的活。
 *    → 一次手术只治一种病（08 篇：小步重构，每步可验收）。
 */
public class PricingService {

    public Money price(Order order) {
        Money total = order.subtotal();
        if (order.isVip()) {
            total = total.multiply(0.9);
        }
        if (total.greaterOrEqual(Money.ofYuan(1000))) {
            total = total.minus(Money.ofYuan(250));
        } else if (total.greaterOrEqual(Money.ofYuan(500))) {
            total = total.minus(Money.ofYuan(100));
        } else if (total.greaterOrEqual(Money.ofYuan(300))) {
            total = total.minus(Money.ofYuan(50));
        }
        if (order.getCoupon() != null) {
            total = total.minus(order.getCoupon());
        }
        if (order.isNewUser()) {
            total = total.minus(Money.ofYuan(30));
        }
        return total;
    }
}
