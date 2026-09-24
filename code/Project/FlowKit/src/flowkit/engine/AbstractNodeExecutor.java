package flowkit.engine;

import flowkit.domain.Ticket;
import flowkit.domain.TicketStatus;

/**
 * 模板方法 —— 骨架锁定执行顺序（进入 → 校验 → 核心逻辑 → 钩子），
 * 子类不可改流程只能填空。
 */
public abstract class AbstractNodeExecutor implements NodeExecutor {

    @Override
    public final void execute(ApproverNode node, Ticket t, ApprovalContext ctx) {
        enter(node, t, ctx);                            // ① 进入（触发装饰器）
        if (!checkPermission(node, t, ctx)) {           // ② 通用校验
            return;
        }
        doExecute(node, t, ctx);                        // ③ 变化点：子类填空
        afterExecute(node, t, ctx);                     // ④ 钩子（默认空实现）
    }

    protected void enter(ApproverNode node, Ticket t, ApprovalContext ctx) {
        node.enter(t, ctx);
    }

    protected boolean checkPermission(ApproverNode node, Ticket t, ApprovalContext ctx) {
        if (t.getStatus() != TicketStatus.REVIEWING) {
            System.out.println("[权限不足] 工单状态=" + t.getStatus() + "，无法审批");
            return false;
        }
        return true;
    }

    /** 变化点：每种执行器只填这一格。 */
    protected abstract void doExecute(ApproverNode node, Ticket t, ApprovalContext ctx);

    /** 钩子：默认什么都不做，子类按需覆写（如发事件、打点）。 */
    protected void afterExecute(ApproverNode node, Ticket t, ApprovalContext ctx) { }
}
