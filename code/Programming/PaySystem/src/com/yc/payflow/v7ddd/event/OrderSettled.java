package com.yc.payflow.v7ddd.event;

import com.yc.payflow.shared.Money;

/**
 * OrderSettled 订单结算完成事件——是"账户扣款/发票/积分/通知"下游的共同触发源。
 *
 * 【设计要点】：
 *   - 携带订阅方需要的数据（订单号、金额、买家、通知渠道等）
 *   - 不携带订阅方不需要的（例如：不带原始 items）
 *   - 事件一旦发布不可变（final 字段）
 */
public class OrderSettled extends DomainEvent {

    private final String orderNo;
    private final String buyerId;
    private final Money finalAmount;
    private final String contactEmail;
    private final String contactPhone;

    public OrderSettled(String orderNo, String buyerId, Money finalAmount,
                        String contactEmail, String contactPhone) {
        this.orderNo = orderNo;
        this.buyerId = buyerId;
        this.finalAmount = finalAmount;
        this.contactEmail = contactEmail;
        this.contactPhone = contactPhone;
    }

    public String orderNo() { return orderNo; }
    public String buyerId() { return buyerId; }
    public Money finalAmount() { return finalAmount; }
    public String contactEmail() { return contactEmail; }
    public String contactPhone() { return contactPhone; }
}
