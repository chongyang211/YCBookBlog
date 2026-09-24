package flowkit.engine;

import flowkit.domain.Ticket;

/** 审批执行器：核心逻辑 = 交给职责链节点。 */
public class ApproveExecutor extends AbstractNodeExecutor {
    @Override
    protected void doExecute(ApproverNode node, Ticket t, ApprovalContext ctx) {
        node.approve(t, ctx);           // 全部结构代码都在父类，子类只剩业务差异
    }
}
