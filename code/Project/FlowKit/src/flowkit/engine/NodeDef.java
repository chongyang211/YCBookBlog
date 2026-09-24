package flowkit.engine;

import flowkit.domain.Ticket;

/**
 * 节点定义 —— 流程定义侧的"产品"（由工厂创建、被装饰器包裹）。
 * enter() 在审批流进入该节点时被回调，是超时催办/抄送装饰器的挂载点。
 */
public interface NodeDef {
    String name();
    String type();
    default void enter(Ticket t, ApprovalContext ctx) {}
}
