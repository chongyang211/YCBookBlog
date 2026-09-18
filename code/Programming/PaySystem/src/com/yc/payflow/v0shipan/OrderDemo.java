package com.yc.payflow.v0shipan;

/**
 * V0 Demo：跑 3 个订单，暴露 V0 的问题
 *   订单 1：正常小订单，感觉一切良好
 *   订单 2：GOLD 会员满减订单——就是这个订单少收 50 元的（BUG 2）
 *   订单 3：秒杀订单——测试时如果不在 20:00-22:00，折扣就"消失"
 *
 * 运行：./run.sh v0
 */
public class OrderDemo {
    public static void main(String[] args) {
        System.out.println("================ V0 屎山演示（双 11 事故现场）================");

        // 订单 1：普通用户，2 件商品，小额
        OrderInput o1 = new OrderInput();
        o1.orderNo = "V0-001";
        o1.prices = new double[]{29.9, 19.9};
        o1.counts = new int[]{2, 1};
        o1.virtualFlags = new int[]{0, 0};
        o1.crossBorderFlags = new int[]{0, 0};
        o1.city = "北京";
        o1.vipLevel = 0;
        OrderUtil.calculate(o1);
        System.out.println();

        // 订单 2：GOLD 会员，购物 500 元 —— 这里会触发 BUG 2（重复打折）
        //   期望：500 - 50(满减) - 15%(GOLD 折) = 382.5
        //   实际（V0）：500 - 50 - 15% - 50(bug 重复减) = 332.5，少收 50 元
        OrderInput o2 = new OrderInput();
        o2.orderNo = "V0-002-GOLD";
        o2.prices = new double[]{250, 250};
        o2.counts = new int[]{1, 1};
        o2.virtualFlags = new int[]{0, 0};
        o2.crossBorderFlags = new int[]{0, 0};
        o2.city = "上海";
        o2.vipLevel = 3; // GOLD
        double actual2 = OrderUtil.calculate(o2);
        double expected2 = (500 - 50) * 0.85; // 应该是 382.5
        System.out.println(">>> 订单 2 期望实付: ¥" + expected2 + "，实际: ¥" + actual2
                + "，差额: ¥" + (expected2 - actual2) + "（重复打折 bug）");
        System.out.println();

        // 订单 3：秒杀商品
        //   期望：秒杀商品打 8 折
        //   实际（V0）：判定用 new Date().getHours()，不在 20-22 点就不打折
        OrderInput o3 = new OrderInput();
        o3.orderNo = "V0-003-FLASH";
        o3.prices = new double[]{200};
        o3.counts = new int[]{1};
        o3.virtualFlags = new int[]{0};
        o3.crossBorderFlags = new int[]{0};
        o3.city = "深圳";
        o3.vipLevel = 0;
        o3.isFlashSale = true;
        OrderUtil.calculate(o3);
        int hour = new java.util.Date().getHours();
        if (hour < 20 || hour >= 22) {
            System.out.println(">>> 订单 3 说明：当前 " + hour + " 点，未进入秒杀时段，折扣未生效");
            System.out.println("    ↑ 这就是 V6 要用 Clock 注入解决的问题");
        }
        System.out.println();

        System.out.println("================ V0 结束，坏味道 17 处，真实 bug 3 个 ================");
        System.out.println("请翻到博客第 08 篇「坏味道大全」，做一次扫描赛，看你能找出几处");
    }
}
