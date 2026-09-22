package flowkit.task;

import flowkit.stats.TaskVisitor;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/** 会签（AND）：所有人通过才通过。 */
public class AndTaskGroup implements TaskComponent {
    private final List<TaskComponent> children = new ArrayList<>();

    public void add(TaskComponent c) { children.add(c); }

    @Override public boolean isApproved() {
        return children.stream().allMatch(TaskComponent::isApproved);
    }
    @Override public List<TaskComponent> pending() {
        return children.stream().flatMap(c -> c.pending().stream()).toList();
    }
    @Override public String pretty(int d) {
        return "  ".repeat(d) + "会签(AND)\n" +
               children.stream().map(c -> c.pretty(d + 1)).collect(Collectors.joining("\n"));
    }
    @Override public void accept(TaskVisitor v) {
        v.visit(this);                          // 先访问组自己
        children.forEach(c -> c.accept(v));     // 再递归整棵子树
    }
}
