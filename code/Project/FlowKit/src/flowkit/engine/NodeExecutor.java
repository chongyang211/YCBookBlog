package flowkit.engine;

/** 节点执行器接口（模板方法模式的抽象基座见 AbstractNodeExecutor）。 */
public interface NodeExecutor {
    void execute(ApproverNode node, flowkit.domain.Ticket t, ApprovalContext ctx);
}
