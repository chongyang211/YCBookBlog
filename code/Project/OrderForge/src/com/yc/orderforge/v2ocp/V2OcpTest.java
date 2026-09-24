package com.yc.orderforge.v2ocp;

import com.yc.orderforge.domain.Address;
import com.yc.orderforge.domain.City;
import com.yc.orderforge.domain.Customer;
import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;
import com.yc.orderforge.shared.testkit.Assert;
import com.yc.orderforge.shared.testkit.TestRunner;

/** V2 单元测试：策略链正确性 + OCP 扩展点验收。 */
public class V2OcpTest {

    private static Order order(double price, boolean vip, Money coupon, boolean newUser) {
        Order o = new Order("T",
            new Customer("测", "138", new Address(new City("北京"))), vip, newUser, coupon, 0);
        return o.addLine("SKU-A", Money.ofYuan(price), 1);
    }

    public static void main(String[] args) {
        new TestRunner("v2.OCP")
            .test("修复 BUG 1：GOLD 1000 元 = (1000-250)*0.9 = 675", () -> {
                Assert.equals(Money.ofYuan(675),
                    new StrategyPricingService().price(order(1000, true, null, false)),
                    "满减优先，会员折扣作用于满减后金额");
            })
            .test("满减阶梯：满 500 减 100", () -> {
                Assert.equals(Money.ofYuan(400),
                    new StrategyPricingService().price(order(500, false, null, false)),
                    "500 元应减 100");
            })
            .test("满减阶梯：满 300 减 50", () -> {
                Assert.equals(Money.ofYuan(250),
                    new StrategyPricingService().price(order(300, false, null, false)),
                    "300 元应减 50");
            })
            .test("优惠券抵扣不能超过金额（min 兜底）", () -> {
                // 30 元单用 100 元券：最多抵到 0，不能收到负数
                Assert.equals(Money.ofYuan(30),
                    new StrategyPricingService().price(order(30, false, Money.ofYuan(100), false)),
                    "100 元券只能把 30 元单抵到 30（券面值即上限）");
            })
            .test("新人立减与券可叠加", () -> {
                // 100 - 50(券) - 30(新人) = 20（未满 300 无满减）
                Assert.equals(Money.ofYuan(20),
                    new StrategyPricingService().price(order(100, false, Money.ofYuan(50), true)),
                    "券与新人立减按链条顺序叠加");
            })
            .test("OCP 验收：新增策略零改动核心类", () -> {
                DiscountChain chain = DiscountChain.defaults()
                    .register(new DiscountStrategy() {
                        @Override public String name() { return "店庆再减10"; }
                        @Override public Money apply(Money cur, DiscountContext ctx) {
                            Money after = cur.minus(Money.ofYuan(10));
                            return after.isNegative() ? Money.ZERO : after;
                        }
                    });
                // 675 - 10 = 665：新策略通过 register 接入，未改任何已有类
                Assert.equals(Money.ofYuan(665),
                    new StrategyPricingService(chain).price(order(1000, true, null, false)),
                    "新增策略只加类 + register，核心类零修改");
            })
            .run();
    }
}
