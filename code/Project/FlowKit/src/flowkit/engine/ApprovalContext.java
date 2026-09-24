package flowkit.engine;

import flowkit.domain.Ticket;
import flowkit.domain.TicketStatus;

import java.util.ArrayList;
import java.util.List;

/**
 * 审批的"现场"：链在哪、当前到谁、谁在操作。
 * 状态类、命令、装饰器、中介者都通过它协作。
 */
public class ApprovalContext {
    ApproverNode head;            // 链头（包私有：NodeSurgery 手术要直访）
    ApproverNode currentNode;     // 当前停留节点
    private String operator;      // 本次操作人（命令执行时写入）

    public void initChain(ApproverNode head) {
        this.head = head;
        this.currentNode = head;
    }

    public void enterNode(ApproverNode n, Ticket t) {
        this.currentNode = n;
        System.out.println("    -> 流转到: " + n.nodeName() + " (" + n.currentApprover(t) + ")");
        n.enter(t, this);
        // 条件节点无需人审批：进入即自动路由（true/false 分支或直达 END）
        if (n instanceof ConditionNode c) c.approve(t, this);
    }

    /** 链尾到达 → 终审通过。 */
    public void finish(Ticket t) {
        t.setStatus(TicketStatus.APPROVED);
        System.out.println("[OK] 审批完成");
    }

    /** §07 备忘录联动要用的"回退到上一节点"：沿链找 current 的前驱。 */
    public void rollbackToPrevNode(Ticket t) {
        ApproverNode prev = null;
        for (ApproverNode p = head; p != null; p = p.next()) {
            if (p == currentNode) break;
            prev = p;
        }
        if (prev == null) prev = head;      // 已在链头 → 退回链头重审
        currentNode = prev;
        prev.clearApproval();               // 前驱的"已批"标记一并清掉
    }

    /** 撤回/重置：整条链的审批标记清空，current 回链头。 */
    public void resetChain() {
        for (ApproverNode n = head; n != null; n = n.next()) n.clearApproval();
        this.currentNode = head;
    }

    /** 供状态展示 / 统计遍历：从链头到链尾的节点快照。 */
    public List<ApproverNode> chainNodes() {
        List<ApproverNode> list = new ArrayList<>();
        for (ApproverNode n = head; n != null; n = n.next()) list.add(n);
        return list;
    }

    public void setCurrentNode(ApproverNode n) { this.currentNode = n; }
    public ApproverNode currentNode() { return currentNode; }
    public ApproverNode head() { return head; }
    public String getOperator() { return operator; }
    public void setOperator(String operator) { this.operator = operator; }
}
