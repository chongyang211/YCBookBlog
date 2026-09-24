package flowkit.domain.state;

import flowkit.domain.Ticket;
import flowkit.domain.TicketState;
import flowkit.domain.TicketStatus;
import flowkit.engine.ApprovalContext;

/** 已撤回：等价于回到草稿前的暂态。 */
public class WithdrawnState implements TicketState {
    @Override public TicketStatus name() { return TicketStatus.WITHDRAWN; }

    @Override public void submit(Ticket t) { t.setStatus(TicketStatus.REVIEWING); }
    @Override public void approve(Ticket t, ApprovalContext ctx) { System.out.println("已撤回，不能审批"); }
    @Override public void reject(Ticket t, String reason)        { System.out.println("已撤回，不能驳回"); }
    @Override public void withdraw(Ticket t)                     { System.out.println("已撤回"); }
}
