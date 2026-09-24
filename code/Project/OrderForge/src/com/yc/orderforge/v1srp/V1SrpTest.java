package com.yc.orderforge.v1srp;

import com.yc.orderforge.domain.Address;
import com.yc.orderforge.domain.City;
import com.yc.orderforge.domain.Customer;
import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;
import com.yc.orderforge.shared.testkit.Assert;
import com.yc.orderforge.shared.testkit.TestRunner;

/** V1 单元测试：SRP 拆分后的可测性证明。 */
public class V1SrpTest {

    private static Order order(double price, int qty, boolean vip) {
        return new Order("T", new Customer("测", "138", new Address(new City("北京"))),
                vip, false, null, 0)
            .addLine("SKU-A", Money.ofYuan(price), qty);
    }

    public static void main(String[] args) {
        new TestRunner("v1.SRP")
            .test("库存扣减与回滚守恒", () -> {
                InventoryService inv = new InventoryService();
                Order o = order(10, 3, false);
                Assert.trueThat(inv.tryDeduct(o), "扣减应成功");
                Assert.equals(97, inv.stock(), "应扣 3 件");
                inv.restock(o);
                Assert.equals(100, inv.stock(), "回滚后库存恢复");
            })
            .test("库存不足拒绝扣减（不变量守卫）", () -> {
                InventoryService inv = new InventoryService();
                Assert.falseThat(inv.tryDeduct(order(10, 999, false)), "999 件应失败");
                Assert.equals(100, inv.stock(), "失败时库存不动");
            })            .test("支付失败库存自动补偿（BUG 2 后遗症止血）", () -> {
                OrderService svc = new OrderService();
                Order credit = new Order("T2",
                    new Customer("测", "138", new Address(new City("北京"))),
                    false, false, null, 1)                       // 信用卡
                    .addLine("SKU-A", Money.ofYuan(3000), 2);    // 6000 元 > 限额
                Assert.throwsException(IllegalStateException.class, () -> svc.placeOrder(credit));
                Assert.equals(100, svc.inventory().stock(), "支付失败后库存应回滚到 100");
            })
            .test("计价服务可独立单测（不再牵连短信/库存）", () -> {
                PricingService pricing = new PricingService();
                // 399 元非会员：满 300 减 50 → 349
                Assert.equals(Money.ofYuan(349), pricing.price(order(399, 1, false)),
                    "满 300 应减 50");
            })
            .test("运费规则独立可测：满 99 包邮", () -> {
                ShippingService shipping = new ShippingService();
                Assert.equals(Money.ZERO, shipping.fee(order(99, 1, false)), "99 元应包邮");
                Assert.equals(Money.ofYuan(12), shipping.fee(order(88, 1, false)), "88 元应收 12 运费");
            })
            .test("对账台账独立可查", () -> {
                ReconciliationService recon = new ReconciliationService();
                recon.record("O-1", Money.ofYuan(100));
                Assert.equals(1, recon.ledger().size(), "应记录 1 条");
            })
            .run();
    }
}
