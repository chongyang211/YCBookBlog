package com.yc.payflow.v6testable;

import com.yc.payflow.shared.Money;
import com.yc.payflow.shared.testkit.Assert;
import com.yc.payflow.shared.testkit.TestRunner;
import com.yc.payflow.v4solid.*;
import com.yc.payflow.v4solid.steps.SubtotalStep;

import java.time.LocalDateTime;
import java.util.List;

/**
 * FlashSaleClockTest：验证 Clock 注入让秒杀测试瞬时可跑（<10ms）。
 *
 * 【对比 V4 时代】：这些用例根本没法写——除非你允许测试在 20:00 才通过。
 */
public class FlashSaleClockTest {

    private static SettlementContext ctxFlashSale() {
        return SettlementContext.builder("T")
            .addItem(new OrderItem("SKU-F", "iPhone", Money.ofYuan(1000), 1, false, true))
            .build();
    }

    private static Money runAt(int hour) {
        Clock c = Clock.fixed(LocalDateTime.of(2024, 11, 11, hour, 0));
        SettlementEngine eng = new SettlementEngine(List.of(
            new SubtotalStep(), new FlashSaleStepV6(c)));
        return eng.settle(ctxFlashSale()).finalAmount();
    }

    public static void main(String[] args) {
        long start = System.currentTimeMillis();
        new TestRunner("v6.FlashSaleClock")
            .test("秒杀时段 20:00 打 8 折", () -> {
                Assert.equals(Money.ofYuan(800), runAt(20), "20点应打8折 → 800");
            })
            .test("秒杀时段 21:30 打 8 折", () -> {
                Clock c = Clock.fixed(LocalDateTime.of(2024, 11, 11, 21, 30));
                SettlementEngine eng = new SettlementEngine(List.of(
                    new SubtotalStep(), new FlashSaleStepV6(c)));
                Assert.equals(Money.ofYuan(800), eng.settle(ctxFlashSale()).finalAmount(),
                    "21:30 应打8折");
            })
            .test("非秒杀时段 15:00 不打折", () -> {
                Assert.equals(Money.ofYuan(1000), runAt(15), "15点应不打折");
            })
            .test("边界值：22:00 恰好不再秒杀（半开区间 [20,22)）", () -> {
                Assert.equals(Money.ofYuan(1000), runAt(22), "22点应不打折");
            })
            .test("边界值：19:59 尚未开始秒杀", () -> {
                Clock c = Clock.fixed(LocalDateTime.of(2024, 11, 11, 19, 59));
                SettlementEngine eng = new SettlementEngine(List.of(
                    new SubtotalStep(), new FlashSaleStepV6(c)));
                Assert.equals(Money.ofYuan(1000), eng.settle(ctxFlashSale()).finalAmount(),
                    "19:59 应不打折");
            })
            .test("非秒杀商品无论几点都不打折", () -> {
                Clock c = Clock.fixed(LocalDateTime.of(2024, 11, 11, 21, 0));
                SettlementEngine eng = new SettlementEngine(List.of(
                    new SubtotalStep(), new FlashSaleStepV6(c)));
                SettlementContext ctx = SettlementContext.builder("T")
                    .addItem(new OrderItem("SKU-N", "非秒杀", Money.ofYuan(1000), 1, false, false))
                    .build();
                Assert.equals(Money.ofYuan(1000), eng.settle(ctx).finalAmount(),
                    "非秒杀商品应保持原价");
            })
            .run();
        long cost = System.currentTimeMillis() - start;
        System.out.println("[总耗时] " + cost + "ms —— 6 次时段判定，无需等到晚 8 点");
        Assert.trueThat(cost < 1000, "全部秒杀测试应在 1 秒内完成（实际远小于 1s）");
    }
}
