package flowkit.facade.service;

import flowkit.domain.Ticket;
import flowkit.engine.ApproverNode;

/** 任务服务桩：待办的生成与作废。 */
public class TaskService {
    public void createPending(Ticket t, ApproverNode node) {
        System.out.println("[任务] 已为 " + node.nodeName() + " 生成待办");
    }
    public void cancelAll(Ticket t, String why) {
        System.out.println("[任务] 工单#" + t.getId() + " 全部待办已作废: " + why);
    }
    public void cancelPending(Ticket t, String why) {
        System.out.println("[任务] 工单#" + t.getId() + " 未完成待办已作废: " + why);
    }
}
