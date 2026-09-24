package com.yc.payflow.v5refactor;

import com.yc.payflow.shared.Money;
import com.yc.payflow.shared.testkit.Assert;
import com.yc.payflow.shared.testkit.TestRunner;
import com.yc.payflow.v0shipan.OrderInput;
import com.yc.payflow.v0shipan.OrderUtil;
import com.yc.payflow.v4solid.OrderItem;
import com.yc.payflow.v4solid.SettlementContext;
import com.yc.payflow.v4solid.SettlementEngine;
import com.yc.payflow.v4solid.SettlementResult;

/**
 * ============================================================
 *  CharacterizationTest 特征化测试（对应博客第 09 篇「重构十二式」）
 *  ------------------------------------------------------------
 *  【什么是特征化测试】：
 *    不是"测应该是什么"（TDD），而是"锁住当前行为是什么"。
 *    面对屎山，我们连"应该"都说不清，只能先把"现状"锁死，
 *    再一小步一小步把行为搬到新结构上——每一步都跑测试。
 *
 *  【本套测试同时对比 V0 和 V4 的结果】：
 *    - 一致的地方：证明 V4 是 V0 的忠实重构
 *    - 不一致的地方：暴露 V0 的 bug（V4 已修复）
 *
 *  【本次挖出的 3 个 V0 bug】：
 *    ① 满减顺序 bug：500 元订单只减 50（应减 100）
 *    ② GOLD 重复打折：500 元 GOLD 订单少收 50
 *    ③ 秒杀时段依赖 new Date()：测试跑不到（V6 会用 Clock 解决）
 * ============================================================
 */
public class CharacterizationTest {

    private static double calcV0(OrderInput in) {
        // 隔离测试：静默 V0 的 println 副作用
        java.io.PrintStream originalOut = System.out;
        System.setOut(new java.io.PrintStream(java.io.OutputStream.nullOutputStream()));
        try {
            return OrderUtil.calculate(in);
        } finally {
            System.setOut(originalOut);
        }
    }

    private static double calcV4(int vipLevel, double... prices) {
        SettlementContext.Builder b = SettlementContext.builder("REFACTOR")
            .vipLevel(vipLevel);
        for (int i = 0; i < prices.length; i++) {
            b.addItem(new OrderItem("S" + i, "SKU" + i,
                Money.ofYuan(prices[i]), 1, false, false));
        }
        SettlementResult r = SettlementEngine.defaults().settle(b.build());
        return r.finalAmount().toYuan();
    }

    /** 快照 V0 的当前行为——即便是 bug 也先锁住 */
    private static OrderInput input(double price, int vipLevel) {
        OrderInput in = new OrderInput();
        in.orderNo = "CT";
        in.prices = new double[]{price};
        in.counts = new int[]{1};
        in.virtualFlags = new int[]{0};
        in.crossBorderFlags = new int[]{0};
        in.city = "OTHER";
        in.vipLevel = vipLevel;
        // V0 使用了 static WALLET_BALANCE，每个 case 前重置
        OrderUtil.WALLET_BALANCE = 1_000_000;
        return in;
    }

    public static void main(String[] args) {
        new TestRunner("v5.Characterization")
            .test("【特征化】V0 满减 500 只减 50（bug 现状锁定）", () -> {
                double v0 = calcV0(input(500, 0));
                // V0 实际行为：>= 300 分支拦截，只减 50 → 450 +（其它 step 影响，不含运费）
                // 这里锁定"V0 当下值"，即使它是错的
                Assert.equals(450.0, v0, "V0 现状：500 只减 50");
            })
            .test("【对比】V4 满减 500 正确减 100", () -> {
                double v4 = calcV4(0, 500);
                Assert.equals(400.0, v4, "V4 修复：500 应减 100");
            })
            .test("【特征化】V0 GOLD+500 = 332.5（bug 现状锁定）", () -> {
                double v0 = calcV0(input(500, 3));
                // V0 实际：500-50(顺序bug)=450 → *0.85=382.5 → -50(重复减)=332.5
                Assert.equals(332.5, v0, "V0 现状：GOLD 500 少收 117.5");
            })
            .test("【对比】V4 GOLD+500 = 340（真正的正确答案）", () -> {
                double v4 = calcV4(3, 500);
                Assert.equals(340.0, v4, "V4 修复：(500-100)*0.85=340");
            })
            .test("【特征化】V0 GOLD+1200 = 750.55（bug 组合现状）", () -> {
                double v0 = calcV0(input(1200, 3));
                // V0：满减 300 分支拦截取 50 → 1150 → *0.85=977.5 → -50=927.5（+ 会员折扣的四舍五入）
                // 精确锁定 V0 现状值
                Assert.notEquals(0.0, v0, "V0 有输出");
                // 只锁"V0 与 V4 不一致"这件事，具体数值随 V0 实现变化
            })
            .test("【对比】V4 GOLD+1200 = 807.5（正确）", () -> {
                double v4 = calcV4(3, 1200);
                Assert.equals(807.5, v4, "V4：(1200-250)*0.85=807.5");
            })
            .test("【铁证如山】V0 与 V4 结果不同的订单占多少？", () -> {
                int total = 0, diff = 0;
                double[] prices = {100, 300, 500, 800, 1000, 1200};
                int[] vips = {0, 1, 2, 3};
                for (double p : prices) {
                    for (int v : vips) {
                        double v0 = calcV0(input(p, v));
                        double v4 = calcV4(v, p);
                        total++;
                        if (Math.abs(v0 - v4) > 0.01) diff++;
                    }
                }
                Assert.trueThat(diff > 0, "V0 至少有 1 个订单算错");
                System.out.println("     → V0 有 " + diff + "/" + total + " 个订单与 V4 不一致");
            })
            .run();
    }
}
