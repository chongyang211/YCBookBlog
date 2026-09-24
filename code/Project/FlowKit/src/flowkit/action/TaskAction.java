package flowkit.action;

import flowkit.domain.Ticket;
import flowkit.engine.ApprovalContext;

/**
 * 命令模式 —— 把审批"动作"本身变成对象。
 * 方法调用发完即忘；对象可以存进队列、排进日志、延迟执行、逐个撤销。
 * 审批系统四样全要。
 */
public interface TaskAction {
    void execute(Ticket t, ApprovalContext ctx);
    void undo(Ticket t, ApprovalContext ctx);
    String describe();               // 用于审计日志（也是重放的序列化格式）
    default String operator() { return "?"; }
}
