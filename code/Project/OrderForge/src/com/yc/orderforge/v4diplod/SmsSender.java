package com.yc.orderforge.v4diplod;

/** 短信渠道（演示桩：真实场景调运营商 SDK）。 */
public class SmsSender implements MessageSender {
    @Override
    public void send(String to, String message) {
        System.out.println("  [SMS→" + to + "] " + message);
    }
    @Override public String channel() { return "SMS"; }
}
