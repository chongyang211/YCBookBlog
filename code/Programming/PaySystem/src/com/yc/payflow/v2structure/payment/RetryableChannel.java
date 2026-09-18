package com.yc.payflow.v2structure.payment;

import com.yc.payflow.shared.Money;

/**
 * ============================================================
 *  RetryableChannel 重试装饰器（对应博客第 04 篇「装饰器」）
 *  ------------------------------------------------------------
 *  【为什么用装饰器而不是把 retry 塞进 PaymentChannel 接口】
 *    ① retry 是"横切关注"——不属于业务逻辑，与"支付"无关
 *    ② 塞进接口后，每个渠道实现都要复制重试代码
 *    ③ 装饰器让"重试"变成可组合的胶水：
 *         new RetryableChannel(new WechatChannel(), 3)
 *         也能这样叠：new MetricChannel(new RetryableChannel(new WechatChannel()))
 *
 *  【指数退避序列】：本教学场景用 100ms → 200ms → 400ms → 800ms
 *  （真实场景可采用 2s → 4s → 8s → 16s → 32s → 60s → 120s → 300s 封顶）
 * ============================================================
 */
public class RetryableChannel implements PaymentChannel {

    private final PaymentChannel delegate;
    private final int maxAttempts;

    public RetryableChannel(PaymentChannel delegate, int maxAttempts) {
        this.delegate = delegate;
        this.maxAttempts = maxAttempts;
    }

    @Override
    public String pay(String orderNo, Money amount) {
        long backoffMs = 100;
        PaymentException lastError = null;
        for (int attempt = 1; attempt <= maxAttempts; attempt++) {
            try {
                return delegate.pay(orderNo, amount);
            } catch (PaymentException e) {
                lastError = e;
                if (attempt < maxAttempts) {
                    System.out.println("  [重试] 第 " + attempt + " 次失败，" + backoffMs
                        + "ms 后重试");
                    sleep(backoffMs);
                    backoffMs *= 2;
                }
            }
        }
        throw new PaymentException(
            "支付重试 " + maxAttempts + " 次后仍失败", lastError);
    }

    @Override
    public String channelName() {
        return "RETRY(" + delegate.channelName() + ")";
    }

    private static void sleep(long ms) {
        try { Thread.sleep(ms); } catch (InterruptedException e) { Thread.currentThread().interrupt(); }
    }
}
