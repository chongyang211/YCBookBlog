package flowkit.engine;

import flowkit.domain.Ticket;
import flowkit.task.AndTaskGroup;
import flowkit.task.OrTaskGroup;
import flowkit.task.SingleTask;
import flowkit.task.TaskComponent;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/**
 * 会签/或签节点 —— 组合模式的消费方：节点内部长出一棵任务树。
 * §05 的职责链是一维的（节点串），组合是二维的（节点内长树）——两个模式互相嵌套。
 */
public class GroupNode extends ApproverNode {
    private final TaskComponent tree;
    private final boolean andMode;
    private final List<SingleTask> memberTasks = new ArrayList<>();

    public GroupNode(String nodeName, boolean andMode, List<String> members, NodeDef def) {
        super(nodeName, null, def);
        this.andMode = andMode;
        TaskComponent group = andMode ? new AndTaskGroup() : new OrTaskGroup();
        for (String m : members) {
            SingleTask task = new SingleTask(m);
            memberTasks.add(task);
            if (group instanceof AndTaskGroup a) a.add(task);
            else ((OrTaskGroup) group).add(task);
        }
        this.tree = group;
    }

    @Override
    public String currentApprover(Ticket t) {
        List<TaskComponent> p = tree.pending();
        return p.isEmpty() ? "（已完成）" : ((SingleTask) p.get(0)).assignee();
    }

    @Override
    public void approve(Ticket t, ApprovalContext ctx) {
        if (tree.isApproved()) {                       // 组已完成 → 流转
            advance(t, ctx);
            return;
        }
        SingleTask task = findPending(ctx.getOperator());
        if (task == null) {
            System.out.println("[拒绝] " + ctx.getOperator() + " 不是本组待办人，等待: " + pendingNames());
            return;
        }
        task.markDone();
        System.out.println("[OK] " + task.assignee() + " 通过（" + (andMode ? "会签" : "或签")
            + " 进度 " + doneCount() + "/" + memberTasks.size() + "）");
        if (tree.isApproved()) advance(t, ctx);
        else System.out.println("    等待: " + pendingNames());
    }

    public String prettyTree() { return tree.pretty(1); }

    /** 供统计/展示：组内全部成员任务（含完成状态）。 */
    public List<SingleTask> members() { return List.copyOf(memberTasks); }

    private SingleTask findPending(String operator) {
        for (TaskComponent c : tree.pending()) {
            if (c instanceof SingleTask s && s.assignee().equals(operator)) return s;
        }
        return null;
    }

    private String pendingNames() {
        return tree.pending().stream()
            .map(c -> ((SingleTask) c).assignee())
            .collect(Collectors.joining(","));
    }

    private long doneCount() {
        return memberTasks.stream().filter(SingleTask::isApproved).count();
    }
}
