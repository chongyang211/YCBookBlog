package com.yc.payflow.v2structure.notifier;

/** APP 推送通道。 */
public class PushNotifier extends AbstractBillingNotifier {
    @Override
    protected void doSend(String recipient, String body) {
        System.out.println("  [PUSH→device:" + recipient + "] " + body);
    }
    @Override public String channelName() { return "PUSH"; }
}
