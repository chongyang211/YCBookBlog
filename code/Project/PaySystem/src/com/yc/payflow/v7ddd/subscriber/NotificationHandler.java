package com.yc.payflow.v7ddd.subscriber;

import com.yc.payflow.v7ddd.event.EventSubscriber;
import com.yc.payflow.v7ddd.event.OrderSettled;

/** 通知订阅方（通知上下文的入口）：给买家发短信账单。 */
public class NotificationHandler implements EventSubscriber<OrderSettled> {

    @Override
    public Class<OrderSettled> subscribedTo() { return OrderSettled.class; }

    @Override
    public void handle(OrderSettled event) {
        System.out.println("  [通知] 短信→" + event.contactPhone()
            + ": 订单 " + event.orderNo() + " 已支付 " + event.finalAmount());
    }
}
