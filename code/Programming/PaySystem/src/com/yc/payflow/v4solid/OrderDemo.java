package com.yc.payflow.v4solid;

import com.yc.payflow.shared.Money;

public class OrderDemo {
    public static void main(String[] args) {
        System.out.println("================ V4 SOLID Demo ================");
        SettlementEngine engine = SettlementEngine.defaults();

        // 订单 1：普通用户，2 件商品
        SettlementContext ctx1 = SettlementContext.builder("V4-001")
            .addItem(new OrderItem("SKU-A", "笔记本", Money.ofYuan(29.9), 2, false, false))
            .addItem(new OrderItem("SKU-B", "钢笔",   Money.ofYuan(19.9), 1, false, false))
            .city("BEIJING")
            .build();
        print(engine.settle(ctx1));

        // 订单 2：GOLD 会员，购物 500 元 —— V0 的 bug 现场
        //   V0：500-50(满减)-15%(GOLD)-50(重复减) = 332.5，少收 50
        //   V4：500-100(满 500 减 100)*.85 = 340
        SettlementContext ctx2 = SettlementContext.builder("V4-002-GOLD")
            .addItem(new OrderItem("SKU-C", "礼盒", Money.ofYuan(250), 2, false, false))
            .vipLevel(3) // GOLD
            .city("SHANGHAI")
            .build();
        print(engine.settle(ctx2));

        // 订单 3：使用 C30 优惠券 + 银牌会员 + 使用 500 积分
        SettlementContext ctx3 = SettlementContext.builder("V4-003-COMBO")
            .addItem(new OrderItem("SKU-D", "T恤", Money.ofYuan(199), 2, false, false))
            .vipLevel(1) // SILVER
            .couponId("C30-2024")
            .usePoints(500)
            .city("HANGZHOU")
            .build();
        print(engine.settle(ctx3));

        System.out.println();
        System.out.println(">>> 关键观察：");
        System.out.println("    - 新增\"生日礼金\"、\"直播间折扣\" 只需再写一个 Step 类");
        System.out.println("    - SettlementEngine 本类和 9 个已有 Step 一行都不用改");
    }

    private static void print(SettlementResult r) {
        System.out.println();
        System.out.println("[" + r.orderNo() + "] 最终应付: " + r.finalAmount());
        r.auditTrail().forEach(System.out::println);
    }
}
