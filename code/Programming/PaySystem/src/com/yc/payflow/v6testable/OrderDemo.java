package com.yc.payflow.v6testable;

import com.yc.payflow.shared.Money;
import com.yc.payflow.v4solid.*;
import com.yc.payflow.v4solid.steps.SubtotalStep;

import java.time.LocalDateTime;
import java.util.List;
import java.util.function.Supplier;

/**
 * V6 Demo：Clock 注入让秒杀折扣的行为可观察、可控制。
 */
public class OrderDemo {
    public static void main(String[] args) {
        System.out.println("================ V6 Clock 注入 Demo ================");

        // 每次都新建 context（一个 context 只结算一次）
        Supplier<SettlementContext> ctxFactory = () -> SettlementContext.builder("V6-FLASH")
            .addItem(new OrderItem("SKU-F", "iPhone 15", Money.ofYuan(5999), 1, false, true))
            .build();

        // 场景 1：注入 21:00 的 Clock —— 秒杀时段
        LocalDateTime peak = LocalDateTime.of(2024, 11, 11, 21, 30);
        SettlementResult r1 = engine(Clock.fixed(peak)).settle(ctxFactory.get());
        System.out.println("[21:30 秒杀时段] " + r1.finalAmount());
        r1.auditTrail().forEach(System.out::println);
        System.out.println();

        // 场景 2：注入 15:00 的 Clock —— 非秒杀时段
        LocalDateTime afternoon = LocalDateTime.of(2024, 11, 11, 15, 0);
        SettlementResult r2 = engine(Clock.fixed(afternoon)).settle(ctxFactory.get());
        System.out.println("[15:00 非秒杀时段] " + r2.finalAmount());
        r2.auditTrail().forEach(System.out::println);

        System.out.println();
        System.out.println(">>> 关键观察：");
        System.out.println("    - 两个场景只差一个 Clock 注入参数，业务规则一行代码没改");
        System.out.println("    - 测试可以随意拨表，不再受制于 JVM 系统时钟");
    }

    private static SettlementEngine engine(Clock clock) {
        return new SettlementEngine(List.of(
            new SubtotalStep(),
            new FlashSaleStepV6(clock)
        ));
    }
}
