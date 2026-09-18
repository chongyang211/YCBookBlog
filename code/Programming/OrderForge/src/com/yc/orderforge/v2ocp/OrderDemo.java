package com.yc.orderforge.v2ocp;

import com.yc.orderforge.domain.Address;
import com.yc.orderforge.domain.City;
import com.yc.orderforge.domain.Customer;
import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;

/**
 * V2 OCP 手术 Demo：修复 BUG 1 + 现场验收"加折扣零改动"。
 *
 * 运行：./run.sh v2
 */
public class OrderDemo {
    public static void main(String[] args) {
        System.out.println("========== V2 OCP 手术演示 ==========");

        Order gold1000 = new Order("O-201",
            new Customer("李四", "138****0002", new Address(new City("上海"))),
            true, false, null, 0)
            .addLine("SKU-A", Money.ofYuan(600), 1)
            .addLine("SKU-B", Money.ofYuan(400), 1);

        // ① 修复 BUG 1：新旧同台对算
        com.yc.orderforge.v1srp.PricingService oldPricing =
            new com.yc.orderforge.v1srp.PricingService();
        StrategyPricingService newPricing = new StrategyPricingService();
        System.out.println("[BUG 1 修复验证] GOLD 会员 1000 元订单：");
        System.out.println("  V1（if-else 版）: " + oldPricing.price(gold1000) + "  ← 多收 125");
        System.out.println("  V2（策略链版）:   " + newPricing.price(gold1000)
            + "  ← (1000-250)*0.9，正确\n");

        // ② OCP 现场验收：临时来一个"节日直减 20"需求
        System.out.println("[OCP 验收] 产品经理：今晚上线『节日直减 20 元』！");
        DiscountChain chain = DiscountChain.defaults()
            .register(new DiscountStrategy() {            // ← 只写一个新类（此处用匿名类演示）
                @Override public String name() { return "节日直减"; }
                @Override public Money apply(Money cur, DiscountContext ctx) {
                    Money after = cur.minus(Money.ofYuan(20));
                    return after.isNegative() ? Money.ZERO : after;
                }
            });
        StrategyPricingService festival = new StrategyPricingService(chain);
        System.out.println("  叠加节日直减后: " + festival.price(gold1000));
        System.out.println("  改动统计: StrategyPricingService 0 行修改，"
            + "DiscountChain 0 行修改 —— 新增 1 个策略 + 组装根 1 行 register\n");

        // ③ 顺序即业务：链条注册顺序打印
        System.out.println("[顺序即业务] 默认链条 = 满减 → 优惠券 → 会员9折 → 新人立减");
        System.out.println("  V0 的教训：顺序藏在 if-else 的书写顺序里，出了 bug 查三天");
        System.out.println("  V2 的答案：顺序就是注册顺序，评审时一眼可见");
        System.out.println("========== V2 结束：加折扣 = 加文件，不是改文件 ==========");
    }
}
