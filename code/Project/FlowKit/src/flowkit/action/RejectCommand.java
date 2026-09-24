package flowkit.action;

import flowkit.domain.Ticket;
import flowkit.domain.TicketStatus;
import flowkit.engine.ApprovalContext;

/** 驳回命令：execute 走状态机；undo 交回 REVIEWING。 */
public class RejectCommand implements TaskAction {
    private final String operator;
    private final String reason;

    public RejectCommand(String operator, String reason) {
        this.operator = operator;
        this.reason = reason;
    }

    @Override public void execute(Ticket t, ApprovalContext ctx) {
        ctx.setOperator(operator);
        t.reject(reason);                             // 状态机接管
    }

    @Override public void undo(Ticket t, ApprovalContext ctx) {
        t.setStatus(TicketStatus.REVIEWING);          // 数据恢复交给备忘录，这里只做语义标记
    }

    @Override public String describe() {
        return "REJECT by " + operator + " reason=" + reason;
    }
    @Override public String operator() { return operator; }
}
