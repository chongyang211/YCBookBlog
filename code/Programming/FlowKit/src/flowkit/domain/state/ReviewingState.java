package flowkit.domain.state;

import flowkit.domain.Ticket;
import flowkit.domain.TicketState;
import flowkit.domain.TicketStatus;
import flowkit.engine.ApproveExecutor;
import flowkit.engine.ApprovalContext;

/** 审批中：唯一能沿职责链流转的状态。 */
public class ReviewingState implements TicketState {
    @Override public TicketStatus name() { return TicketStatus.REVIEWING; }

    @Override public void submit(Ticket t) { System.out.println("审批中，无需重复提交"); }

    @Override public void approve(Ticket t, ApprovalContext ctx) {
        // 模板方法骨架（进入→校验→执行→钩子）→ 内部再交给职责链节点
        new ApproveExecutor().execute(ctx.currentNode(), t, ctx);
    }
    @Override public void reject(Ticket t, String reason) {
        t.setStatus(TicketStatus.REJECTED);
        System.out.println("[驳回] " + reason);
    }
    @Override public void withdraw(Ticket t) {
        t.setStatus(TicketStatus.DRAFT);
        System.out.println("[OK] 已撤回草稿");
    }
}
