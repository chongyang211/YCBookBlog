package com.yc.payflow.v2structure.payment;

import com.yc.payflow.shared.Money;

/** 微信支付适配器——如果换云闪付，只需要再写一个类，主流程零改动。 */
public class WechatChannel implements PaymentChannel {
    @Override
    public String pay(String orderNo, Money amount) {
        // 真实场景：调 SDK.transfer(...)
        System.out.println("  [微信] 支付 " + amount + " 订单=" + orderNo);
        return "WX-" + System.nanoTime();
    }
    @Override public String channelName() { return "WECHAT"; }
}
