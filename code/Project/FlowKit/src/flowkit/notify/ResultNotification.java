package flowkit.notify;

import flowkit.domain.Ticket;

/** 结果通知。 */
public class ResultNotification extends Notification {
    public ResultNotification(Channel ch) { super(ch); }

    @Override public void notify(String userId, Ticket t) {
        channel.send(userId, "审批结果", "工单#" + t.getId() + " 最终状态: " + t.getStatus());
    }
}
