package com.yc.payflow.v2structure.payment;

import com.yc.payflow.shared.Money;

/**
 * ============================================================
 *  PaymentChannel 支付渠道接口（对应博客第 04 篇「面向接口而非实现编程」）
 *  ------------------------------------------------------------
 *  【核心决策】接口要按"调用场景"切，而不是照抄底层 SDK：
 *    - 错的做法：抄微信 SDK 的 90 个方法（sign/verifyNotify/queryOrder…）
 *    - 对的做法：只暴露业务真正需要的 pay/refund 两个方法
 *
 *  【变化轴预判】：支付宝、云闪付、Apple Pay……都属于"支付渠道"
 *  这一轴的变化。所以这一层必须抽象。
 * ============================================================
 */
public interface PaymentChannel {

    /**
     * 发起支付。
     * @return 支付流水号（用于对账 & 后续退款）
     * @throws PaymentException 支付失败
     */
    String pay(String orderNo, Money amount);

    /** 渠道名称，用于日志 */
    String channelName();
}
