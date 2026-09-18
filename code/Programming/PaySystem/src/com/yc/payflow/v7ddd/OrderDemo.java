package com.yc.payflow.v7ddd;

import com.yc.payflow.shared.Money;
import com.yc.payflow.v7ddd.domain.Order;
import com.yc.payflow.v7ddd.domain.OrderId;
import com.yc.payflow.v7ddd.domain.OrderLine;
import com.yc.payflow.v7ddd.event.EventBus;
import com.yc.payflow.v7ddd.subscriber.InvoiceIssuedHandler;
import com.yc.payflow.v7ddd.subscriber.NotificationHandler;
import com.yc.payflow.v7ddd.subscriber.PointsGrantedHandler;

/**
 * ============================================================
 *  V7 Demo：DDD 终局—— Order 聚合根 + 领域事件 + 多订阅方
 *  ------------------------------------------------------------
 *  【毕业验收点】：
 *    ① Order 状态机守住不变量：settled 之后不能 addLine
 *    ② 结算成功发布领域事件，3 个订阅方独立响应
 *    ③ "临时加一个财务对账订阅方"：只加一个类 + 一行 subscribe
 *       ↓ 本 demo 现场演示
 * ============================================================
 */
public class OrderDemo {

    public static void main(String[] args) {
        System.out.println("================ V7 DDD 终局 Demo ================");

        // 1. 装配事件总线 + 三个订阅方
        EventBus bus = new EventBus();
        bus.subscribe(new InvoiceIssuedHandler());
        bus.subscribe(new PointsGrantedHandler());
        bus.subscribe(new NotificationHandler());

        // 2. 构造订单（聚合根守住状态与不变量）
        Order order = new Order(
            new OrderId("O-20241111-0001"),
            "buyer-alice",
            "alice@example.com",
            "138****8888"
        );
        order.addLine(new OrderLine("SKU-B", "《面向对象设计》纸质版", Money.ofYuan(68), 1));
        order.addLine(new OrderLine("SKU-C", "视频课程",             Money.ofYuan(299), 1));

        System.out.println("[Order 小计] " + order.subtotal());

        // 3. 结算（假设最终金额已由 V4 SettlementEngine 计算完毕，这里传入结果）
        Money finalAmount = Money.ofYuan(340);
        System.out.println("[Order 结算] 最终金额 " + finalAmount);
        order.settle(finalAmount, bus);
        System.out.println("[Order 状态] " + order.status());

        // 4. 现场演示：临时加一个"财务对账"订阅方 —— 结算/聚合/其它订阅方零改动
        System.out.println();
        System.out.println(">>> 临时挂上'财务对账'订阅方（结算代码 0 改动）");
        bus.subscribe(new com.yc.payflow.v7ddd.event.EventSubscriber<com.yc.payflow.v7ddd.event.OrderSettled>() {
            @Override public Class<com.yc.payflow.v7ddd.event.OrderSettled> subscribedTo() {
                return com.yc.payflow.v7ddd.event.OrderSettled.class;
            }
            @Override public void handle(com.yc.payflow.v7ddd.event.OrderSettled e) {
                System.out.println("  [财务] 对账入表: " + e.orderNo() + " " + e.finalAmount());
            }
        });

        // 5. 再来一单，四个订阅方都会被触发
        Order order2 = new Order(new OrderId("O-20241111-0002"),
            "buyer-bob", "bob@example.com", "139****9999");
        order2.addLine(new OrderLine("SKU-A", "耳机", Money.ofYuan(199), 1));
        order2.settle(Money.ofYuan(199), bus);

        // 6. 状态机守卫展示
        System.out.println();
        System.out.println(">>> 状态机不变量演示");
        try {
            order.settle(Money.ofYuan(100), bus);
        } catch (IllegalStateException e) {
            System.out.println("  [守卫] 已结算订单再次结算被拒: " + e.getMessage());
        }
        try {
            order.addLine(new OrderLine("SKU-X", "偷加行", Money.ofYuan(1), 1));
        } catch (IllegalStateException e) {
            System.out.println("  [守卫] 已结算订单加行被拒: " + e.getMessage());
        }
    }
}
