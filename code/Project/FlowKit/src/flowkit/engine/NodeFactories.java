package flowkit.engine;

import java.util.Map;

/**
 * 工厂注册表 —— 简单工厂是工厂方法的退化形式，注册表是两者的折中。
 * 每个内部静态类都是工厂方法模式的一个实现。
 */
public final class NodeFactories {
    private static final Map<String, NodeFactory> REG = Map.of(
        "START",     new StartNodeFactory(),
        "END",       new EndNodeFactory(),
        "APPROVE",   new ApproveNodeFactory(),
        "CONDITION", new ConditionNodeFactory(),
        "AND_SIGN",  new AndSignNodeFactory(),
        "OR_SIGN",   new OrSignNodeFactory(),
        "CC",        new CcNodeFactory());

    private NodeFactories() {}

    public static NodeFactory of(String type) {
        NodeFactory f = REG.get(type);
        if (f == null) throw new IllegalArgumentException("未知节点类型: " + type);
        return f;
    }

    static class StartNodeFactory implements NodeFactory {
        @Override public NodeDef create(NodeSpec spec) { return new SimpleNodeDef("START", "START"); }
    }

    static class EndNodeFactory implements NodeFactory {
        @Override public NodeDef create(NodeSpec spec) { return new SimpleNodeDef("END", "END"); }
    }

    /** 审批节点：组装策略（有名单用固定人，有名用注册表策略）。 */
    static class ApproveNodeFactory implements NodeFactory {
        @Override public NodeDef create(NodeSpec spec) {
            if (spec.person() != null) {
                return new ApproveNodeDef(spec.name(), ticket -> spec.person());
            }
            if (spec.finder() != null) {
                return new ApproveNodeDef(spec.name(), FinderRegistry.of(spec.finder()));
            }
            throw new IllegalArgumentException("审批节点必须指定 finder 或 person: " + spec.name());
        }
    }

    /** 条件节点：表达式原样入定义（解析与缓存推迟到运行时）。 */
    static class ConditionNodeFactory implements NodeFactory {
        @Override public NodeDef create(NodeSpec spec) {
            if (spec.expr() == null)
                throw new IllegalArgumentException("条件节点必须指定 expr: " + spec.name());
            return new ConditionNodeDef(spec.name(), spec.expr());
        }
    }

    /** 会签：创建时校验成员非空。 */
    static class AndSignNodeFactory implements NodeFactory {
        @Override public NodeDef create(NodeSpec spec) {
            if (spec.members().isEmpty())
                throw new IllegalArgumentException("会签节点成员不能为空: " + spec.name());
            return new GroupNodeDef(spec.name(), true, spec.members());
        }
    }

    /** 或签：成员非空。 */
    static class OrSignNodeFactory implements NodeFactory {
        @Override public NodeDef create(NodeSpec spec) {
            if (spec.members().isEmpty())
                throw new IllegalArgumentException("或签节点成员不能为空: " + spec.name());
            return new GroupNodeDef(spec.name(), false, spec.members());
        }
    }

    static class CcNodeFactory implements NodeFactory {
        @Override public NodeDef create(NodeSpec spec) { return new SimpleNodeDef(spec.name(), "CC"); }
    }
}
