package flowkit.stats;

import flowkit.task.AndTaskGroup;
import flowkit.task.OrTaskGroup;
import flowkit.task.SingleTask;

/**
 * 访问者模式 —— 元素（任务树）稳定、操作（财务/审计/运营还会加）多变。
 * 新增"合规视角" = 新写一个 Visitor，任务树 0 修改。
 */
public interface TaskVisitor {
    void visit(SingleTask t);
    void visit(AndTaskGroup g);
    void visit(OrTaskGroup g);
}
