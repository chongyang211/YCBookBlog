package com.yc.payflow.v2structure.payment;

import com.yc.payflow.shared.Money;

/**
 * 抖动通道——教学用：可控地失败若干次后成功，配合装饰器演示重试。
 * 生产环境不会有这种通道，纯粹为了 RetryableChannel 的演示。
 */
public class FlakyChannel implements PaymentChannel {
    private final int failTimes;
    private int attempted;
    private final String base;

    public FlakyChannel(String base, int failTimes) {
        this.base = base;
        this.failTimes = failTimes;
    }

    @Override
    public String pay(String orderNo, Money amount) {
        attempted++;
        if (attempted <= failTimes) {
            System.out.println("  [" + base + "] 第 " + attempted + " 次尝试失败（模拟网络抖动）");
            throw new PaymentException("网络抖动，请重试");
        }
        System.out.println("  [" + base + "] 第 " + attempted + " 次尝试成功");
        return base + "-" + System.nanoTime();
    }

    @Override public String channelName() { return base; }
}
