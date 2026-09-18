package com.yc.payflow.v4solid;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;

/**
 * ============================================================
 *  SettlementEngine 结算引擎（对应博客 06/07 篇 SOLID）
 *  ------------------------------------------------------------
 *  【核心承诺】：
 *    这个类以及它的方法，在 V4 完成之后就再也不需要改动了。
 *    加"新营销规则"：只需实现 PromotionStep + 加入 List。
 *    去"某营销规则"：只需从 List 移除。
 *
 *  【SRP 检查】：本类只有 1 个变化理由——"步骤编排顺序变了"。
 *  【OCP 检查】：对扩展开放（加 Step），对修改关闭（不改本类）。
 *  【LSP 检查】：任何 PromotionStep 实现都可以替换现有 Step 的位置。
 *  【ISP 检查】：PromotionStep 接口只有 4 个方法，全部有意义。
 *  【DIP 检查】：Engine 依赖抽象 PromotionStep，不依赖具体 Step。
 * ============================================================
 */
public class SettlementEngine {

    private final List<PromotionStep> steps;

    public SettlementEngine(List<PromotionStep> steps) {
        // 副本 + 按 order 排序（可变列表用来添加 step 后重新排序）
        List<PromotionStep> sorted = new ArrayList<>(steps);
        sorted.sort(Comparator.comparingInt(PromotionStep::order));
        this.steps = List.copyOf(sorted);
    }

    public SettlementResult settle(SettlementContext ctx) {
        for (PromotionStep step : steps) {
            if (step.applies(ctx)) {
                step.apply(ctx);
            } else {
                ctx.audit(step.name() + "(不适用)", com.yc.payflow.shared.Money.ZERO,
                          ctx.currentTotal());
            }
        }
        return new SettlementResult(ctx.orderNo(), ctx.currentTotal(), ctx.auditTrail());
    }

    /** 便捷工厂：返回默认全套 9 个 step 的引擎。 */
    public static SettlementEngine defaults() {
        return new SettlementEngine(List.of(
            new com.yc.payflow.v4solid.steps.SubtotalStep(),
            new com.yc.payflow.v4solid.steps.FullReductionStep(),
            new com.yc.payflow.v4solid.steps.FlashSaleStep(),
            new com.yc.payflow.v4solid.steps.CouponStep(),
            new com.yc.payflow.v4solid.steps.VipDiscountStep(),
            new com.yc.payflow.v4solid.steps.ShippingStep(),
            new com.yc.payflow.v4solid.steps.CrossBorderTaxStep(),
            new com.yc.payflow.v4solid.steps.PointsStep(),
            new com.yc.payflow.v4solid.steps.MinimumChargeStep()
        ));
    }
}
