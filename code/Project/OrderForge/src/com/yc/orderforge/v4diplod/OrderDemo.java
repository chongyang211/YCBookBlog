package com.yc.orderforge.v4diplod;

import com.yc.orderforge.domain.Address;
import com.yc.orderforge.domain.City;
import com.yc.orderforge.domain.Customer;
import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;
import com.yc.orderforge.v1srp.InventoryService;
import com.yc.orderforge.v1srp.ShippingService;
import com.yc.orderforge.v2ocp.StrategyPricingService;
import com.yc.orderforge.v3lspisp.BalancePayment;
import com.yc.orderforge.v3lspisp.CreditCardPayment;
import com.yc.orderforge.v3lspisp.PaymentRouter;

/**
 * V4 DIP+LOD 手术 Demo：三个 bug 全部修复的终局链路 + 换件演示。
 *
 * 运行：./run.sh v4
 */
public class OrderDemo {
    public static void main(String[] args) {
        System.out.println("========== V4 DIP + LOD 手术演示 ==========");

        // ---- 组装根：所有具体实现在此决定（业务代码只认抽象） ----
        OrderFacade facade = new OrderFacade(
            new InventoryService(),
            new StrategyPricingService(),
            new ShippingService(),
            new PaymentRouter(),
            new BalancePayment(Money.ofYuan(10000)),
            new NotificationService(new SmsSender()),        // ← 通知渠道在这里换
            new InMemoryOrderRepository());                   // ← 仓储在这里换

        // ① 正常单
        Order o1 = new Order("O-401",
            new Customer("张三", "138****0001", new Address(new City("北京"))),
            false, false, null, 0)
            .addLine("SKU-A", Money.ofYuan(299), 1)
            .addLine("SKU-B", Money.ofYuan(100), 1);
        System.out.println("[正常单] " + facade.placeOrder(o1));
        System.out.println();

        // ② BUG 1/2/3 终局验证
        Order gold1000 = new Order("O-402",
            new Customer("李四", "138****0002", new Address(new City("上海"))),
            true, false, null, 0)
            .addLine("SKU-A", Money.ofYuan(600), 1)
            .addLine("SKU-B", Money.ofYuan(400), 1);
        System.out.println("[BUG 1 已修] GOLD 1000 元 → 策略链计价 ¥675");
        facade.placeOrder(gold1000);
        System.out.println();

        OrderFacade creditFacade = new OrderFacade(
            new InventoryService(), new StrategyPricingService(),
            new ShippingService(), new PaymentRouter(),
            new CreditCardPayment(Money.ofYuan(5000)),
            new NotificationService(new SmsSender()),
            new InMemoryOrderRepository());
        Order big = new Order("O-403",
            new Customer("王五", "138****0003", new Address(new City("深圳"))),
            false, false, null, 0)
            .addLine("SKU-A", Money.ofYuan(3000), 2);
        System.out.println("[BUG 2 已修] 信用卡 6000 元超限：");
        try {
            creditFacade.placeOrder(big);
        } catch (Exception e) {
            System.out.println("  " + e.getMessage());
        }
        System.out.println("  库存 = " + creditFacade.inventory().stock() + "（未动）\n");

        Order tourist = new Order("O-404",
            new Customer("游客赵六", "138****0004", null),   // 空地址
            false, true, null, 0)
            .addLine("SKU-B", Money.ofYuan(88), 1);
        System.out.println("[BUG 3 已修] 游客无地址订单：");
        facade.placeOrder(tourist);
        System.out.println("  ↑ 城市兜底为'未知城市'，通知不再 NPE\n");

        // ③ 换件演示（DIP 收益）
        System.out.println("[换件演示] 运营：通知改走邮件 + 仓储落盘（门面/业务 0 行修改）");
        OrderFacade swapped = new OrderFacade(
            new InventoryService(), new StrategyPricingService(),
            new ShippingService(), new PaymentRouter(),
            new BalancePayment(Money.ofYuan(10000)),
            new NotificationService(new EmailSender()),       // ← 只改这一行
            new FileOrderRepository());                        // ← 只改这一行
        swapped.placeOrder(new Order("O-405",
            new Customer("孙七", "139****0005", new Address(new City("杭州"))),
            false, false, null, 0)
            .addLine("SKU-B", Money.ofYuan(59), 2));

        System.out.println("\n========== V4 结束：DIP 换件不改业务，LOD 只认直接朋友 ==========");
        System.out.println("终局验收：./run.sh report —— 度量前后对比 + 三改实验");
    }
}
