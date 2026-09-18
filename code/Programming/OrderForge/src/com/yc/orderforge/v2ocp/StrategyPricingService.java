package com.yc.orderforge.v2ocp;

import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;

/**
 * 策略版计价服务：从"if-else 森林"变成"一条链"。
 *
 * 【核心承诺】本类从 V2 起再也不会因为"新增营销规则"被修改——
 * 加规则 = 加 Strategy 类 + 组装根 register 一行。
 *
 * 对照 03 篇开篇促销演变史：V1 堆到 V5 的 if-else，在 V2 之后成为历史。
 */
public class StrategyPricingService {

    private final DiscountChain chain;

    public StrategyPricingService(DiscountChain chain) {
        this.chain = chain;
    }

    public StrategyPricingService() {
        this(DiscountChain.defaults());
    }

    public Money price(Order order) {
        return chain.apply(order);
    }
}
