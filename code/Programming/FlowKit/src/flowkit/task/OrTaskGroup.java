package flowkit.task;

import flowkit.stats.TaskVisitor;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/** 或签（OR）：任一人通过即通过。 */
public class OrTaskGroup implements TaskComponent {
    private final List<TaskComponent> children = new ArrayList<>();

    public void add(TaskComponent c) { children.add(c); }

    @Override public boolean isApproved() {
        return children.stream().anyMatch(TaskComponent::isApproved);
    }
    @Override public List<TaskComponent> pending() {
        // 或签的特殊性：已有人通过 → 全组完工；否则所有未完成子任务并列等待
        if (isApproved()) return List.of();
        return children.stream().flatMap(c -> c.pending().stream()).toList();
    }
    @Override public String pretty(int d) {
        return "  ".repeat(d) + "或签(OR)\n" +
               children.stream().map(c -> c.pretty(d + 1)).collect(Collectors.joining("\n"));
    }
    @Override public void accept(TaskVisitor v) {
        v.visit(this);
        children.forEach(c -> c.accept(v));
    }
}
