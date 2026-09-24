package com.yc.orderforge.v4diplod;

/**
 * 邮件渠道（V4 会话现场新增）：
 * 只写这一个类 + 组装时注入 —— NotificationService 一行不改。
 * 这就是 06 篇"换库只写一个适配实现"的现场版。
 */
public class EmailSender implements MessageSender {
    @Override
    public void send(String to, String message) {
        System.out.println("  [EMAIL→" + to + "] " + message);
    }
    @Override public String channel() { return "EMAIL"; }
}
