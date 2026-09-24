package com.yc.orderforge.v4diplod;

import com.yc.orderforge.domain.Address;
import com.yc.orderforge.domain.City;
import com.yc.orderforge.domain.Customer;
import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;
import com.yc.orderforge.shared.testkit.Assert;
import com.yc.orderforge.shared.testkit.TestRunner;
import com.yc.orderforge.v1srp.InventoryService;
import com.yc.orderforge.v1srp.ShippingService;
import com.yc.orderforge.v2ocp.StrategyPricingService;
import com.yc.orderforge.v3lspisp.BalancePayment;
import com.yc.orderforge.v3lspisp.CreditCardPayment;
import com.yc.orderforge.v3lspisp.PaymentRejectedException;
import com.yc.orderforge.v3lspisp.PaymentRouter;

/** V4 单元测试：DIP 换件 + LOD 兜底 + 门面唯一入口。 */
public class V4DipLodTest {

    private static OrderFacade facade(MessageSender sender, OrderRepository repo,
                                      com.yc.orderforge.v3lspisp.PaymentMethod pm) {
        return new OrderFacade(new InventoryService(), new StrategyPricingService(),
            new ShippingService(), new PaymentRouter(), pm,
            new NotificationService(sender), repo);
    }

    private static Order order(String no, String city, Money price, int qty) {
        return new Order(no,
            new Customer("测", "138", city == null ? null : new Address(new City(city))),
            false, false, null, 0)
            .addLine("SKU-A", price, qty);
    }

    public static void main(String[] args) {
        new TestRunner("v4.DIP+LOD")
            .test("DIP：换通知渠道（短信→邮件）门面与业务 0 行修改", () -> {
                OrderFacade emailFacade = facade(new EmailSender(),
                    new InMemoryOrderRepository(), new BalancePayment(Money.ofYuan(10000)));
                emailFacade.placeOrder(order("T1", "北京", Money.ofYuan(100), 1));
            })
            .test("DIP：换仓储实现（内存→文件）0 行修改", () -> {
                FileOrderRepository fileRepo = new FileOrderRepository();
                OrderFacade f = facade(new SmsSender(), fileRepo,
                    new BalancePayment(Money.ofYuan(10000)));
                f.placeOrder(order("T2", "上海", Money.ofYuan(100), 1));
                Assert.equals(1, fileRepo.records().size(), "文件仓储应记录 1 单");
            })
            .test("LOD：取城市只认直接朋友（一层调用替代四层残骸）", () -> {
                Order o = order("T3", "广州", Money.ofYuan(10), 1);
                Assert.equals("广州", o.getShippingCityName(), "一层拿到城市");
            })
            .test("LOD：空地址兜底'未知城市'，不再 NPE（BUG 3 修复）", () -> {
                Order o = order("T4", null, Money.ofYuan(10), 1);
                Assert.equals("未知城市", o.getShippingCityName(), "空地址安全兜底");
                // 走完整链路也不炸
                facade(new SmsSender(), new InMemoryOrderRepository(),
                    new BalancePayment(Money.ofYuan(10000))).placeOrder(o);
            })
            .test("契约+门面：信用卡超限提前拦截，库存未动（BUG 2 修复）", () -> {
                OrderFacade f = facade(new SmsSender(), new InMemoryOrderRepository(),
                    new CreditCardPayment(Money.ofYuan(5000)));
                Assert.throwsException(PaymentRejectedException.class,
                    () -> f.placeOrder(order("T5", "北京", Money.ofYuan(3000), 2)));
                Assert.equals(100, f.inventory().stock(), "失败时库存应保持 100");
            })
            .test("门面：客户端只见 placeOrder 一个入口（LOD 朋友数 = 1）", () -> {
                // 客户端代码只 import OrderFacade —— 编译依赖只有 1 个类
                OrderFacade f = facade(new SmsSender(), new InMemoryOrderRepository(),
                    new BalancePayment(Money.ofYuan(10000)));
                Assert.trueThat(f.placeOrder(order("T6", "北京", Money.ofYuan(50), 1))
                    .startsWith("T6"), "门面入口应返回订单号");
            })
            .run();
    }
}
