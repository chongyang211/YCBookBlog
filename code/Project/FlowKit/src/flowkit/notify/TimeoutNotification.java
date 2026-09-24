package flowkit.notify;

import flowkit.domain.Ticket;

/** 超时催办。 */
public class TimeoutNotification extends Notification {
    public TimeoutNotification(Channel ch) { super(ch); }

    @Override public void notify(String userId, Ticket t) {
        channel.send(userId, "催办", "工单#" + t.getId() + " 已超时未处理，请尽快审批");
    }
}
