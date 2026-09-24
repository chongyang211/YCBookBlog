package flowkit.engine;

import flowkit.domain.Ticket;

/**
 * 装饰器基座 —— 同接口 + 持有被包裹者 + 默认全转发。
 * 接口有多少方法，基座就要转发多少（这正是装饰器的成本）；
 * 子类只需覆写它要"加料"的那一个。
 */
public abstract class NodeDecorator implements NodeDef {
    protected final NodeDef inner;                       // 被包裹者

    protected NodeDecorator(NodeDef inner) { this.inner = inner; }

    @Override public String name() { return inner.name(); }
    @Override public String type() { return inner.type(); }
    @Override public void enter(Ticket t, ApprovalContext ctx) { inner.enter(t, ctx); }
}
