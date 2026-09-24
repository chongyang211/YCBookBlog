package com.yc.orderforge.v2ocp;

import com.yc.orderforge.shared.Money;

/**
 * 阶梯满减（V2 修复 BUG 1 的第一刀：满减优先，从高到低命中即停）。
 *
 * 对照 V0/V1 的写法（会员折扣在前、阶梯从低到高），策略化的意义：
 *   - 顺序从"代码里碰巧的书写顺序"变成"注册顺序"，显式可查
 *   - 阶梯数据化（LADDERS），运营加档位改数据不改逻辑
 */
public class FullReductionDiscount implements DiscountStrategy {

    /** 阶梯：满 X 元减 Y 元（阈值降序，命中即停） */
    private static final double[][] LADDERS = {
        {1000, 250},   // 满 1000 减 250
        {500,  100},   // 满  500 减 100
        {300,  50}     // 满  300 减  50
    };

    @Override public String name() { return "阶梯满减"; }

    @Override
    public Money apply(Money current, DiscountContext ctx) {
        for (double[] rung : LADDERS) {
            Money threshold = Money.ofYuan(rung[0]);
            if (current.greaterOrEqual(threshold)) {
                return current.minus(Money.ofYuan(rung[1]));
            }
        }
        return current;
    }
}
