package flowkit.engine;

/** 条件节点定义：规则是数据（表达式字符串），运行时解释执行。 */
public record ConditionNodeDef(String name, String expr) implements NodeDef {
    @Override public String type() { return "CONDITION"; }
}
