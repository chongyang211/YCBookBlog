package flowkit.engine;

/** 审批节点定义：持有审批人策略（工厂创建时组装）。 */
public record ApproveNodeDef(String name, ApproverFinder finder) implements NodeDef {
    @Override public String type() { return "APPROVE"; }
}
