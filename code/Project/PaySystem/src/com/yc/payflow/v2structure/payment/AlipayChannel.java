package com.yc.payflow.v2structure.payment;

import com.yc.payflow.shared.Money;

/** 支付宝渠道。 */
public class AlipayChannel implements PaymentChannel {
    @Override
    public String pay(String orderNo, Money amount) {
        System.out.println("  [支付宝] 支付 " + amount + " 订单=" + orderNo);
        return "ALI-" + System.nanoTime();
    }
    @Override public String channelName() { return "ALIPAY"; }
}
