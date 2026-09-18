package com.yc.payflow.v2structure.notifier;

/** 短信通道（子类只需实现"如何发出去"）。 */
public class SmsNotifier extends AbstractBillingNotifier {
    @Override
    protected void doSend(String recipient, String body) {
        // 真实场景：调用运营商短信 SDK。这里用 println 演示。
        System.out.println("  [SMS→" + recipient + "] " + body);
    }
    @Override public String channelName() { return "SMS"; }
}
