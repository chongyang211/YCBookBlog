package flowkit.engine;

import flowkit.domain.Ticket;
import flowkit.domain.TicketStatus;
import flowkit.rule.ExprCache;
import flowkit.rule.RuleContext;

/**
 * 条件路由节点 —— 解释器模式的消费方。
 * 规则（"amount > 5000"）是数据，运行时解释执行；AST 走 ExprCache 缓存。
 */
public class ConditionNode extends ApproverNode {
    private final String exprSource;
    private ApproverNode trueNode;            // true 分支目标（null = 直接 END）

    public ConditionNode(String nodeName, String exprSource, NodeDef def) {
        super(nodeName, t -> "(条件节点)", def);
        this.exprSource = exprSource;
    }

    void setTrueNode(ApproverNode n) { this.trueNode = n; }

    @Override
    public String currentApprover(Ticket t) { return "（自动路由）"; }

    @Override
    public void approve(Ticket t, ApprovalContext ctx) {
        boolean hit = ExprCache.of(exprSource).eval(RuleContext.of(t));   // 缓存命中则不再 parse
        System.out.println("[条件路由] " + exprSource + " -> " + hit);
        if (hit && trueNode != null)      ctx.enterNode(trueNode, t);
        else if (!hit && next() != null)  ctx.enterNode(next(), t);
        else                              ctx.finish(t);
    }
}
