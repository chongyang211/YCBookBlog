package flowkit.action;

import flowkit.domain.Ticket;
import flowkit.engine.ApprovalContext;
import flowkit.engine.ApproverNode;
import flowkit.engine.NodeSurgery;

/** 加签命令：execute 插节点，undo 拔节点——返回值即 undo 凭证。 */
public class AddSignCommand implements TaskAction {
    private final String person;
    private ApproverNode inserted;                    // 记住自己插了哪个，undo 才能拔
    private ApproverNode target;                      // 加签时的当前节点（undo 时回到它）

    public AddSignCommand(String person) { this.person = person; }

    @Override public void execute(Ticket t, ApprovalContext ctx) {
        target = ctx.currentNode();
        inserted = NodeSurgery.addSignBefore(ctx.head(), target, person);
        System.out.println("[OK] 已在\"" + target.nodeName() + "\"节点前加签: " + person);
        ctx.setCurrentNode(inserted);                 // 流转由加签人先行审批
        System.out.println("    -> 流转到: [加签] " + person);
    }

    @Override public void undo(Ticket t, ApprovalContext ctx) {
        NodeSurgery.remove(ctx.head(), inserted);     // 拔掉自己（不在链上则幂等返回）
        ctx.setCurrentNode(target);                   // 回到加签前的节点
    }

    @Override public String describe() { return "ADDSIGN " + person; }
    @Override public String operator() { return person; }
}
