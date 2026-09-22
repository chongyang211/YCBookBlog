package flowkit.task;

import flowkit.stats.TaskVisitor;

import java.util.List;

/** 叶子：一个人的审批任务。 */
public class SingleTask implements TaskComponent {
    private final String assignee;
    private final long ticketAmount;      // 统计道具（分）
    private boolean done = false;
    private long elapsedMillis;           // 完成耗时（教学演示值）

    public SingleTask(String assignee) { this(assignee, 0L); }

    public SingleTask(String assignee, long ticketAmount) {
        this.assignee = assignee;
        this.ticketAmount = ticketAmount;
    }

    public SingleTask(String assignee, long ticketAmount, boolean done) {
        this(assignee, ticketAmount);
        if (done) markDone();
    }

    public void markDone() {
        this.done = true;
        // 教学演示：耗时 = 名字哈希派生的稳定伪值（避免随机数导致输出不可复现）
        this.elapsedMillis = 300 + Math.abs(assignee.hashCode()) % 900;
    }

    public String assignee()      { return assignee; }
    public long ticketAmount()    { return ticketAmount; }
    public long elapsedMillis()   { return elapsedMillis; }
    public boolean overdue()      { return elapsedMillis > 800; }

    @Override public boolean isApproved() { return done; }
    @Override public List<TaskComponent> pending() { return done ? List.of() : List.of(this); }
    @Override public String pretty(int d) {
        return "  ".repeat(d) + "· " + assignee + (done ? " [done]" : " [todo]");
    }
    @Override public void accept(TaskVisitor v) { v.visit(this); }   // this 的静态类型即分派依据
}
