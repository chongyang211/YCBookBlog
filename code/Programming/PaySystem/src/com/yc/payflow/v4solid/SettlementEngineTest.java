package com.yc.payflow.v4solid;

import com.yc.payflow.shared.Money;
import com.yc.payflow.shared.testkit.Assert;
import com.yc.payflow.shared.testkit.TestRunner;
import com.yc.payflow.v4solid.steps.*;

import java.util.List;

public class SettlementEngineTest {

    /** 只启用指定的 step 构造测试引擎，避免时间/其它 step 干扰 */
    private static SettlementEngine engineOf(PromotionStep... steps) {
        return new SettlementEngine(List.of(steps));
    }

    public static void main(String[] args) {
        new TestRunner("v4.SettlementEngine")
            .test("小计：3 件商品 = 单价×数量之和", () -> {
                SettlementContext ctx = SettlementContext.builder("T1")
                    .addItem(new OrderItem("A", "A", Money.ofYuan(10), 2, false, false))
                    .addItem(new OrderItem("B", "B", Money.ofYuan(20), 3, false, false))
                    .build();
                SettlementResult r = engineOf(new SubtotalStep()).settle(ctx);
                Assert.equals(Money.ofYuan(80), r.finalAmount(), "10*2+20*3=80");
            })
            .test("满减 500 减 100（修复了 V0 的顺序 bug）", () -> {
                SettlementContext ctx = SettlementContext.builder("T2")
                    .addItem(new OrderItem("A", "A", Money.ofYuan(500), 1, false, false))
                    .build();
                SettlementResult r = engineOf(new SubtotalStep(), new FullReductionStep()).settle(ctx);
                Assert.equals(Money.ofYuan(400), r.finalAmount(), "500-100=400（不是 V0 的 450）");
            })
            .test("满减 1000 减 250", () -> {
                SettlementContext ctx = SettlementContext.builder("T3")
                    .addItem(new OrderItem("A", "A", Money.ofYuan(1200), 1, false, false))
                    .build();
                SettlementResult r = engineOf(new SubtotalStep(), new FullReductionStep()).settle(ctx);
                Assert.equals(Money.ofYuan(950), r.finalAmount(), "1200-250=950");
            })
            .test("GOLD 会员满减：修复 V0 重复打折 bug", () -> {
                SettlementContext ctx = SettlementContext.builder("T4")
                    .addItem(new OrderItem("A", "A", Money.ofYuan(500), 1, false, false))
                    .vipLevel(3)
                    .build();
                SettlementResult r = engineOf(
                    new SubtotalStep(), new FullReductionStep(), new VipDiscountStep()).settle(ctx);
                // V0 会算成 332.5（少收 50）；V4 应算成 (500-100)*0.85=340
                Assert.equals(Money.ofYuan(340), r.finalAmount(), "V4 应为 340 而非 V0 的 332.5");
            })
            .test("优惠券 C30：满 300 减 30", () -> {
                SettlementContext ctx = SettlementContext.builder("T5")
                    .addItem(new OrderItem("A", "A", Money.ofYuan(400), 1, false, false))
                    .couponId("C30-XYZ")
                    .build();
                SettlementResult r = engineOf(new SubtotalStep(), new CouponStep()).settle(ctx);
                Assert.equals(Money.ofYuan(370), r.finalAmount(), "400-30=370");
            })
            .test("优惠券未满门槛不生效", () -> {
                SettlementContext ctx = SettlementContext.builder("T6")
                    .addItem(new OrderItem("A", "A", Money.ofYuan(50), 1, false, false))
                    .couponId("C30-XYZ")
                    .build();
                SettlementResult r = engineOf(new SubtotalStep(), new CouponStep()).settle(ctx);
                Assert.equals(Money.ofYuan(50), r.finalAmount(), "50 元不满 300，券不生效");
            })
            .test("跨境税 11.9%", () -> {
                SettlementContext ctx = SettlementContext.builder("T7")
                    .addItem(new OrderItem("A", "A", Money.ofYuan(100), 1, true, false))
                    .build();
                SettlementResult r = engineOf(new SubtotalStep(), new CrossBorderTaxStep()).settle(ctx);
                Assert.equals(Money.ofYuan(111.90), r.finalAmount(), "100+11.9=111.9");
            })
            .test("积分抵扣：500 积分 = 5 元", () -> {
                SettlementContext ctx = SettlementContext.builder("T8")
                    .addItem(new OrderItem("A", "A", Money.ofYuan(50), 1, false, false))
                    .usePoints(500)
                    .build();
                SettlementResult r = engineOf(new SubtotalStep(), new PointsStep()).settle(ctx);
                Assert.equals(Money.ofYuan(45), r.finalAmount(), "50-5=45");
            })
            .test("积分抵扣不能让金额为负", () -> {
                SettlementContext ctx = SettlementContext.builder("T9")
                    .addItem(new OrderItem("A", "A", Money.ofYuan(3), 1, false, false))
                    .usePoints(10000) // 抵扣 100 元
                    .build();
                SettlementResult r = engineOf(
                    new SubtotalStep(), new PointsStep(), new MinimumChargeStep()).settle(ctx);
                Assert.trueThat(r.finalAmount().isPositive(), "最终金额应为正（兜底至 0.01）");
            })
            .test("扩展点验收：新增 Step 零改动 Engine", () -> {
                // 现场自定义一个"节日满减"step，不改任何现有代码
                PromotionStep festival = new PromotionStep() {
                    @Override public int order() { return 25; }
                    @Override public String name() { return "Festival"; }
                    @Override public void apply(SettlementContext ctx) {
                        ctx.setCurrentTotal(ctx.currentTotal().minus(Money.ofYuan(5)));
                        ctx.audit(name(), Money.ofYuan(5), ctx.currentTotal());
                    }
                };
                SettlementContext ctx = SettlementContext.builder("T10")
                    .addItem(new OrderItem("A", "A", Money.ofYuan(100), 1, false, false))
                    .build();
                SettlementResult r = engineOf(new SubtotalStep(), festival).settle(ctx);
                Assert.equals(Money.ofYuan(95), r.finalAmount(), "100-5=95");
            })
            .run();
    }
}
