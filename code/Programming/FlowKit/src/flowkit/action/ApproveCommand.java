package flowkit.action;

import flowkit.domain.Ticket;
import flowkit.engine.ApprovalContext;

import java.time.Instant;

/** 审批通过命令：execute 沿职责链流转；undo 回退到上一节点。 */
public class ApproveCommand implements TaskAction {
    private final String operator;

    public ApproveCommand(String operator) { this.operator = operator; }

    @Override public void execute(Ticket t, ApprovalContext ctx) {
        ctx.setOperator(operator);                    // 记录操作人（节点校验要用）
        t.approve(ctx);                               // 状态机 → 模板方法 → 职责链
    }

    @Override public void undo(Ticket t, ApprovalContext ctx) {
        ctx.rollbackToPrevNode(t);                    // 结构性回滚（数据现场靠备忘录）
    }

    @Override public String describe() {
        return "APPROVE by " + operator;
    }
    @Override public String operator() { return operator; }
}
