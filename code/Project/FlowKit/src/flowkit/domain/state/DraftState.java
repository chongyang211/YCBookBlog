package flowkit.domain.state;

import flowkit.domain.Ticket;
import flowkit.domain.TicketState;
import flowkit.domain.TicketStatus;
import flowkit.engine.ApprovalContext;

/** 草稿：唯一能"提交"的状态。 */
public class DraftState implements TicketState {
    @Override public TicketStatus name() { return TicketStatus.DRAFT; }

    @Override public void submit(Ticket t) {
        t.setStatus(TicketStatus.REVIEWING);
        System.out.println("[OK] 已提交，进入审批");
    }
    @Override public void approve(Ticket t, ApprovalContext ctx) { System.out.println("草稿未提交，不能审批"); }
    @Override public void reject(Ticket t, String reason)        { System.out.println("草稿不能驳回"); }
    @Override public void withdraw(Ticket t)                     { System.out.println("草稿无需撤回"); }
}
