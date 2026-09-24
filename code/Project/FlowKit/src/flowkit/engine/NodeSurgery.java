package flowkit.engine;

/**
 * 链的"手术刀" —— 加签/拔签都在这一处。
 * 只动 O(1) 个引用，不通知链上其他节点，不改任何已完成的审批记录。
 */
public final class NodeSurgery {
    private NodeSurgery() {}

    /** 加签：在 target 前插入一个新节点 —— 只动 2 个引用。返回新节点引用 = undo 凭证。 */
    public static ApproverNode addSignBefore(ApproverNode head, ApproverNode target, String person) {
        if (head == target) throw new UnsupportedOperationException("链头不能前加签");
        ApproverNode p = head;
        while (p != null && p.next() != target) p = p.next();
        if (p == null) throw new IllegalStateException("目标节点不在链上");

        ApproverNode sign = new ApproverNode("[加签] " + person, ticket -> person, null);
        p.setNext(sign);        // 前驱指向新节点
        sign.setNext(target);   // 新节点指向目标
        return sign;
    }

    /** 拔签：把节点从链上摘除 —— 同样只动 2 个引用（undo 场景调用）。 */
    public static void remove(ApproverNode head, ApproverNode node) {
        if (head == node) throw new UnsupportedOperationException("链头不能拔");
        ApproverNode p = head;
        while (p != null && p.next() != node) p = p.next();
        if (p == null) return;              // 已不在链上 → 幂等返回（undo 场景常态）
        p.setNext(node.next());             // 前驱直接"跨过"我
        node.setNext(null);                 // 自断引用：防误用，也帮 GC
    }
}
