package com.yc.payflow.v3composition.ability;

/**
 * ============================================================
 *  能力接口：可配送（Shippable）
 *  ------------------------------------------------------------
 *  对应博客第 05 篇「多用组合少用继承」。
 *
 *  【为什么用"能力接口"而不是"父类"】
 *    继承的死结（企鹅会飞现场）：
 *      class VirtualProduct extends PhysicalProduct { ship() }
 *      —— 虚拟课程被迫拥有 ship() 方法，运行时抛 UOE
 *
 *    组合的解法：
 *      "是否可配送"是一种能力，不是一种"是"
 *      → 抽成能力接口，谁有就装配谁
 * ============================================================
 */
public interface Shippable {
    /** 计算配送费。 */
    long shippingFeeCents(String city, long subtotalCents);
}
