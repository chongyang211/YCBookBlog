package flowkit.facade.service;

import flowkit.domain.Ticket;
import flowkit.engine.ApprovalContext;

/** 流程服务桩：链的复位。 */
public class ProcessService {
    public void resetToHead(ApprovalContext ctx) {
        ctx.resetChain();                    // 清空整条链的审批标记，current 回链头
        System.out.println("[流程] 审批链已重置到链头");
    }
    public void reject(Ticket t) {
        System.out.println("[流程] 工单#" + t.getId() + " 流程终止（已驳回）");
    }
}
