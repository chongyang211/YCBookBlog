package com.yc.payflow.v7ddd.subscriber;

import com.yc.payflow.v7ddd.event.EventSubscriber;
import com.yc.payflow.v7ddd.event.OrderSettled;

/** 积分订阅方（会员上下文的入口）：每消费 1 元发 1 积分。 */
public class PointsGrantedHandler implements EventSubscriber<OrderSettled> {

    @Override
    public Class<OrderSettled> subscribedTo() { return OrderSettled.class; }

    @Override
    public void handle(OrderSettled event) {
        long points = event.finalAmount().cents() / 100;
        System.out.println("  [积分] 为用户 " + event.buyerId() + " 发放 " + points + " 积分");
    }
}
