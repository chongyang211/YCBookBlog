package flowkit.stats;

import flowkit.task.AndTaskGroup;
import flowkit.task.OrTaskGroup;
import flowkit.task.SingleTask;

/** 财务视角：金额汇总（只累计"已通过"任务的金额）。 */
public class FinanceVisitor implements TaskVisitor {
    private long totalAmount;

    @Override public void visit(SingleTask t) {
        if (t.isApproved()) totalAmount += t.ticketAmount();
    }
    @Override public void visit(AndTaskGroup g) { /* 会签按组计一次，不重复累加子任务 */ }
    @Override public void visit(OrTaskGroup g)  { /* 或签取首个通过的子任务金额 */ }

    public long totalAmount() { return totalAmount; }
}
