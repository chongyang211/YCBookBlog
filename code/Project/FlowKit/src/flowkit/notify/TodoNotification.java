package flowkit.notify;

import flowkit.domain.Ticket;

/** 待办提醒。 */
public class TodoNotification extends Notification {
    public TodoNotification(Channel ch) { super(ch); }

    @Override public void notify(String userId, Ticket t) {
        channel.send(userId, "待办提醒", "工单#" + t.getId() + " 等待您审批");
    }
}
