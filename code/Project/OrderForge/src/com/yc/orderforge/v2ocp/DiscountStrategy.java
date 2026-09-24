package com.yc.orderforge.v2ocp;

import com.yc.orderforge.shared.Money;

/**
 * 折扣策略 SPI（03 篇 DiscountStrategy 的落地）。
 *
 * OCP 三要素（03 篇 §6.2）：
 *   稳定点：本接口（4 个方法，再也不改）
 *   接入点：DiscountChain.register()（注册即生效）
 *   变化点：每种折扣一个实现类（加文件，不改文件）
 *
 * Rule of Three：V0 时只有 1 种规则不抽象；满减/会员/券出现第 3 种时才抽——
 * 现在已有 4 种，抽象的时机到了。
 */
public interface DiscountStrategy {

    /** 策略名（审计用） */
    String name();

    /** 是否适用于本单 */
    default boolean applies(DiscountContext ctx) { return true; }

    /** 在当前金额上应用本策略，返回新金额 */
    Money apply(Money current, DiscountContext ctx);
}
