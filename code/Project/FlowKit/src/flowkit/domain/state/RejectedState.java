package flowkit.domain.state;

import flowkit.domain.Ticket;
import flowkit.domain.TicketState;
import flowkit.domain.TicketStatus;
import flowkit.engine.ApprovalContext;

/** 已驳回：只能重新提交。"已驳回的单不能撤回成草稿"——§03 事故二的洞在这里被堵上。 */
public class RejectedState implements TicketState {
    @Override public TicketStatus name() { return TicketStatus.REJECTED; }

    @Override public void submit(Ticket t) {
        t.setStatus(TicketStatus.REVIEWING);          // 重新走链（链进度由 ProcessService 重置）
        System.out.println("[OK] 已重新提交");
    }
    @Override public void approve(Ticket t, ApprovalContext ctx) { System.out.println("已驳回，不能审批"); }
    @Override public void reject(Ticket t, String reason)        { System.out.println("已驳回"); }
    @Override public void withdraw(Ticket t) {
        System.out.println("已驳回的单不能撤回成草稿");
    }
}
