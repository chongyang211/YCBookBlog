package com.yc.payflow.v2structure.notifier;

/**
 * ============================================================
 *  BillingNotifier 账单通知接口（对应博客第 03 篇「接口 vs 抽象类」）
 *  ------------------------------------------------------------
 *  【契约层】：调用方只知道"能不能发账单通知"，不知道底层是短信/推送/邮件。
 *  【设计决策】：为什么"接口 + 骨架抽象类"双层？
 *    - interface：能力契约，允许多重实现（→ can-do）
 *    - AbstractBillingNotifier：共享行为（限流/黑名单/模板拼接）
 *    这就是 Spring `BeanFactory + AbstractApplicationContext`、
 *          Netty `ChannelHandler + ChannelInboundHandlerAdapter`、
 *          MyBatis `Executor + BaseExecutor` 的同款范式。
 * ============================================================
 */
public interface BillingNotifier {
    /**
     * 发送账单通知。
     * @param recipient 接收方（手机号 / device token / 邮箱地址）
     * @param orderNo   订单编号
     * @param amount    金额文本（如 "¥123.45"）
     */
    void notify(String recipient, String orderNo, String amount);

    /** 通道名称，用于日志与审计 */
    String channelName();
}
