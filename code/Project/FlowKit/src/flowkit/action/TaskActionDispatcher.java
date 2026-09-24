package flowkit.action;

import flowkit.domain.Ticket;
import flowkit.engine.ApprovalContext;

/**
 * 调度器接口 —— 为 JDK 动态代理提供接口锚点：
 * AuditProxy.wrap() 之后，全接口所有方法自动留痕。
 */
public interface TaskActionDispatcher {
    void dispatch(TaskAction action, Ticket t, ApprovalContext ctx);
    void undo(Ticket t, ApprovalContext ctx);
}
