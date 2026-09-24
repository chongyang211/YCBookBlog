package com.yc.orderforge.v2ocp;

import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;

import java.util.ArrayList;
import java.util.List;

/**
 * 折扣链（注册表）：注册顺序 = 执行顺序，顺序从此显式可查。
 *
 * 【OCP 验收点】新增一种折扣 = 新写一个 Strategy 类 + 这里 register 一行。
 * 03 篇 §12.2 的辨析：register 一行属于"组装"，不属于"修改核心逻辑"——
 * 它在组合根（组装代码）里，不在任何被复用的业务类里。
 */
public class DiscountChain {

    private final List<DiscountStrategy> strategies = new ArrayList<>();

    public DiscountChain register(DiscountStrategy strategy) {
        strategies.add(strategy);
        return this;
    }

    public Money apply(Order order) {
        DiscountContext ctx = new DiscountContext(order, order.subtotal());
        Money current = order.subtotal();
        for (DiscountStrategy s : strategies) {
            if (s.applies(ctx)) {
                current = s.apply(current, ctx);
            }
        }
        return current;
    }

    /** 默认链条：满减 → 优惠券 → 会员 → 新人（顺序即业务规则，修复 BUG 1） */
    public static DiscountChain defaults() {
        return new DiscountChain()
            .register(new FullReductionDiscount())
            .register(new CouponDiscount())
            .register(new VipDiscount())
            .register(new NewUserDiscount());
    }
}
