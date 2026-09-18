package com.yc.payflow.v2structure;

import com.yc.payflow.shared.Money;
import com.yc.payflow.v2structure.notifier.*;
import com.yc.payflow.v2structure.payment.*;

import java.util.List;

/**
 * V2 Demo：验证"接口+骨架抽象类"双层范式 & 支付渠道+装饰器。
 *
 * 输出重点：
 *   [通知] 三个通道共享同一段模板方法流程，各自 doSend
 *   [支付] Retry 装饰器让"抖动 2 次"变成"最终成功"，不改渠道实现
 */
public class OrderDemo {
    public static void main(String[] args) {
        System.out.println("================ V2 结构版 Demo ================");
        demoNotifier();
        System.out.println();
        demoPayment();
    }

    private static void demoNotifier() {
        System.out.println("[通知双层范式] 三种通道共享同一模板");
        List<BillingNotifier> channels = List.of(
            new SmsNotifier(),
            new PushNotifier(),
            new EmailNotifier() // ← 新增的：写这一个类就够，主流程 0 修改
        );
        for (BillingNotifier n : channels) {
            n.notify(recipientOf(n), "V2-2024001", "¥382.50");
        }
    }

    private static void demoPayment() {
        System.out.println("[支付渠道 + 重试装饰器]");
        // 单纯换支付宝：new AlipayChannel() 即可
        PaymentChannel wechat = new WechatChannel();
        String txn1 = wechat.pay("V2-2024002", Money.ofYuan(99.90));
        System.out.println("  微信流水号: " + txn1);

        // 用 RetryableChannel 装饰一个"前 2 次失败"的通道
        PaymentChannel retryable = new RetryableChannel(new FlakyChannel("UNIONPAY", 2), 3);
        String txn2 = retryable.pay("V2-2024003", Money.ofYuan(200));
        System.out.println("  最终流水号: " + txn2);
    }

    private static String recipientOf(BillingNotifier n) {
        return switch (n.channelName()) {
            case "SMS" -> "138****8888";
            case "PUSH" -> "abc-token-xyz";
            case "EMAIL" -> "user@example.com";
            default -> "unknown";
        };
    }
}
