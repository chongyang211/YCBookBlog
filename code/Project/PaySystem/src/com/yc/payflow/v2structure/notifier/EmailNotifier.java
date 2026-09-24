package com.yc.payflow.v2structure.notifier;

/**
 * 邮件通道——本节的验收点：
 *   新增此通道只写这一个文件，主流程零修改。
 *   → 这就是"接口+骨架抽象类"双层范式的价值。
 */
public class EmailNotifier extends AbstractBillingNotifier {
    @Override
    protected void doSend(String recipient, String body) {
        System.out.println("  [EMAIL→" + recipient + "] Subject: 账单通知");
        System.out.println("                                       Body: " + body);
    }
    @Override public String channelName() { return "EMAIL"; }
}
