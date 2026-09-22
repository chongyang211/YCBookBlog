package flowkit.facade.service;

import flowkit.domain.Ticket;
import flowkit.notify.Channel;
import flowkit.notify.ResultNotification;
import flowkit.notify.TodoNotification;

/**
 * 通知服务桩 —— 内部用"桥接"组合渠道与消息类型：
 * 消息类型（待办/结果）× 渠道（钉钉/邮件）在这里自由搭配。
 */
public class NotifyService {
    private final Channel channel;

    public NotifyService(Channel channel) { this.channel = channel; }

    public void send(String userId, String title, String content) {
        channel.send(userId, title, content);
    }

    public void sendTodo(String userId, Ticket t) {
        new TodoNotification(channel).notify(userId, t);
    }

    public void sendResult(String userId, Ticket t) {
        new ResultNotification(channel).notify(userId, t);
    }
}
