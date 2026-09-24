package com.yc.payflow.v2structure;

import com.yc.payflow.shared.testkit.Assert;
import com.yc.payflow.shared.testkit.TestRunner;
import com.yc.payflow.v2structure.notifier.*;

/**
 * NotifierTest：验证双层范式的骨架代码复用 & 扩展点独立。
 */
public class NotifierTest {

    /** 测试用假通道：只收集调用参数不真发 */
    static class RecordingNotifier extends AbstractBillingNotifier {
        String lastRecipient;
        String lastBody;
        int sentCount = 0;
        @Override protected void doSend(String recipient, String body) {
            this.lastRecipient = recipient;
            this.lastBody = body;
            this.sentCount++;
        }
        @Override public String channelName() { return "RECORD"; }
        @Override protected void onSent(String recipient, String orderNo) {} // 静默
    }

    public static void main(String[] args) {
        new TestRunner("v2.Notifier")
            .test("空接收方被拒", () -> {
                Assert.throwsException(IllegalArgumentException.class,
                    () -> new SmsNotifier().notify("", "O1", "¥1.00"));
            })
            .test("模板方法：三通道共享文案格式", () -> {
                RecordingNotifier r = new RecordingNotifier();
                r.notify("bob", "O2", "¥88.00");
                Assert.trueThat(r.lastBody.contains("O2"), "文案应含订单号");
                Assert.trueThat(r.lastBody.contains("¥88.00"), "文案应含金额");
                Assert.trueThat(r.lastBody.contains("感谢惠顾"), "文案应含骨架层公共尾语");
            })
            .test("黑名单命中不发送（骨架层横切能力）", () -> {
                RecordingNotifier r = new RecordingNotifier();
                r.addBlacklist("banned");
                r.notify("banned", "O3", "¥1.00");
                Assert.equals(0, r.sentCount, "黑名单接收方应被拦截");
                r.notify("normal", "O4", "¥1.00");
                Assert.equals(1, r.sentCount, "非黑名单应正常发送");
            })
            .test("新增通道零改动主流程（EmailNotifier 就是明证）", () -> {
                BillingNotifier n = new EmailNotifier();
                Assert.equals("EMAIL", n.channelName(), "邮件通道应正确注册");
                n.notify("a@b.com", "O5", "¥10.00"); // 不抛异常即通过
            })
            .run();
    }
}
