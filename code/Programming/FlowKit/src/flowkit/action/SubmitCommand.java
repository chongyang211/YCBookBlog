package flowkit.action;

import flowkit.domain.Ticket;
import flowkit.engine.ApprovalContext;

/** 提交命令：崩溃重放的起点就是它。 */
public class SubmitCommand implements TaskAction {
    private final Ticket tk;          // 引用工单（describe 提取字段，重放据此重建）

    public SubmitCommand(Ticket t) { this.tk = t; }

    @Override public void execute(Ticket t, ApprovalContext ctx) {
        t.submit();
        t.touchSubmit();
    }

    @Override public void undo(Ticket t, ApprovalContext ctx) { t.withdraw(); }

    @Override public String describe() {
        return "SUBMIT " + tk.getType() + " " + tk.getAmount()
             + " \"" + tk.getReason() + "\" by " + tk.getApplicant();
    }
    @Override public String operator() { return tk.getApplicant(); }
}
