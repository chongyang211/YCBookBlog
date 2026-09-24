package flowkit.engine;

import flowkit.domain.Ticket;

/**
 * 职责链节点 —— 审批链上的一个 Handler。
 * 链负责流转顺序，策略（ApproverFinder）负责每一站的算法。
 */
public class ApproverNode {
    protected final String nodeName;
    protected final ApproverFinder finder;
    protected final NodeDef def;              // 流程定义侧的节点（装饰器挂载点）
    protected ApproverNode next;              // 链的下一个
    protected String approvedBy;              // 谁批的（null = 未批）

    public ApproverNode(String nodeName, ApproverFinder finder, NodeDef def) {
        this.nodeName = nodeName;
        this.finder = finder;
        this.def = def;
    }

    public ApproverNode then(ApproverNode next) { this.next = next; return next; }

    // 包私有访问器：链的"手术"（NodeSurgery / 回退）在同包内直访——有意的封装让步
    ApproverNode next()          { return next; }
    void setNext(ApproverNode n) { this.next = n; }
    void clearApproval()         { this.approvedBy = null; }     // undo 的前提

    public String nodeName()     { return nodeName; }
    public boolean isDone()      { return approvedBy != null; }
    public NodeDef nodeDef()     { return def; }
    public String currentApprover(Ticket t) { return finder.find(t); }

    /** 进入节点：触发 NodeDef.enter（催办/抄送装饰器的挂载点）。加签节点无 def，安全跳过。 */
    public void enter(Ticket t, ApprovalContext ctx) {
        if (def != null) def.enter(t, ctx);
    }

    /** 处理审批：我批过了就交给 next，没批就在我这里等着。 */
    public void approve(Ticket t, ApprovalContext ctx) {
        if (approvedBy == null) {
            String expect = finder.find(t);            // 当前该谁审（策略）
            if (!expect.equals(ctx.getOperator())) {
                System.out.println("[拒绝] 本节点应由 " + expect + " 审批");
                return;
            }
            this.approvedBy = ctx.getOperator();
            System.out.println("[OK] " + nodeName + " 通过 (" + approvedBy + ")");
            advance(t, ctx);
        } else if (next != null) {
            next.approve(t, ctx);                      // 我批过了，接力
        }
    }

    /** 流转到下一节点；到链尾则终审通过。 */
    protected void advance(Ticket t, ApprovalContext ctx) {
        if (next != null) ctx.enterNode(next, t);
        else ctx.finish(t);
    }
}
