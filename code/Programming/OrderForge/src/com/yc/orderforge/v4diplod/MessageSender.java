package com.yc.orderforge.v4diplod;

/**
 * 消息发送者抽象（06 篇 MessageSender 原案）。
 *
 * DIP 要点：本接口由【高层】（通知业务）定义，只声明业务需要的 send 一个能力；
 * 低层的短信/邮件适配器反过来实现它——"不是业务求着渠道，而是渠道适配业务"。
 */
public interface MessageSender {
    /** 发送消息 */
    void send(String to, String message);

    /** 渠道名 */
    String channel();
}
