package com.yc.payflow.v7ddd.subscriber;

import com.yc.payflow.v7ddd.event.EventSubscriber;
import com.yc.payflow.v7ddd.event.OrderSettled;

/**
 * 发票订阅方（发票上下文的入口）。
 *
 * ⚠️ 关键教学点：新增这个订阅方，SettlementEngine 或 Order 聚合的代码一行都不用改。
 * 只需在启动时 eventBus.subscribe(new InvoiceIssuedHandler()) 即可。
 */
public class InvoiceIssuedHandler implements EventSubscriber<OrderSettled> {

    @Override
    public Class<OrderSettled> subscribedTo() { return OrderSettled.class; }

    @Override
    public void handle(OrderSettled event) {
        System.out.println("  [发票] 为订单 " + event.orderNo() + " 开具电子发票 "
            + event.finalAmount() + " → " + event.contactEmail());
    }
}
