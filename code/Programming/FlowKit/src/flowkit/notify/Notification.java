package flowkit.notify;

import flowkit.domain.Ticket;

/**
 * 桥接 —— 两个独立变化的维度：
 * 渠道（钉钉/企微/邮件）× 消息类型（待办/催办/结果）。
 * 把一个维度（渠道）注入为字段，M×N 子类爆炸交给组合：3+3 个类取代 9 个。
 */
public abstract class Notification {
    protected final Channel channel;          // 桥——实现维度

    protected Notification(Channel channel) { this.channel = channel; }

    public abstract void notify(String userId, Ticket t);
}
