package com.yc.payflow.v4solid;

/**
 * ============================================================
 *  PromotionStep 营销规则 SPI（对应博客 06 篇 OCP + 04 篇面向接口）
 *  ------------------------------------------------------------
 *  【核心承诺】：加"新营销规则"只需要写一个 Step 实现类，
 *              然后把它加进 SettlementEngine 的 List——
 *              不改 Engine 的任何一行代码。
 *
 *  【执行顺序】由 order() 决定；小的先执行。
 *  【是否适用】applies() 允许 step 自己决定"这个订单要不要跑我"，
 *              比 Engine 里判 if 强得多（散弹式修改的克星）。
 * ============================================================
 */
public interface PromotionStep {
    /** 执行顺序权重，小的先跑 */
    int order();

    /** 步骤名，用于审计 */
    String name();

    /** 是否适用于此上下文；不适用则跳过 */
    default boolean applies(SettlementContext ctx) { return true; }

    /** 执行步骤，可修改 ctx.currentTotal */
    void apply(SettlementContext ctx);
}
