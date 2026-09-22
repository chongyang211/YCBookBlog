package flowkit.stats;

import flowkit.task.AndTaskGroup;
import flowkit.task.OrTaskGroup;
import flowkit.task.SingleTask;

import java.util.ArrayList;
import java.util.List;

/** 审计视角：加签次数与异常任务（靠命名约定识别加签——教学简化）。 */
public class AuditVisitor implements TaskVisitor {
    private int addSignCount;
    private final List<String> anomalies = new ArrayList<>();

    @Override public void visit(SingleTask t) {
        if (t.assignee().startsWith("[加签]")) addSignCount++;
        if (t.overdue()) anomalies.add("超时任务: " + t.assignee());
    }
    @Override public void visit(AndTaskGroup g) { }
    @Override public void visit(OrTaskGroup g)  { }

    public int addSignCount() { return addSignCount; }
    public List<String> anomalies() { return anomalies; }
    public String report() {
        return "加签=" + addSignCount + "次 异常=" + anomalies.size() + "处";
    }
}
