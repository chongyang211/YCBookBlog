package flowkit.domain.state;

import flowkit.domain.Ticket;
import flowkit.domain.TicketState;
import flowkit.domain.TicketStatus;
import flowkit.engine.ApprovalContext;

/** 终态：一个方法都不许动。 */
public class ApprovedState implements TicketState {
    @Override public TicketStatus name() { return TicketStatus.APPROVED; }

    @Override public void submit(Ticket t)                     { System.out.println("已通过"); }
    @Override public void approve(Ticket t, ApprovalContext ctx) { System.out.println("已终审通过"); }
    @Override public void reject(Ticket t, String r)           { System.out.println("已通过，不能驳回"); }
    @Override public void withdraw(Ticket t)                   { System.out.println("已通过，不能撤回"); }
}
