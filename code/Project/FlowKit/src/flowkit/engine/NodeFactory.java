package flowkit.engine;

/**
 * 工厂方法模式 —— 每种产品一个工厂。
 * 相比 switch 出产品的简单工厂：每种节点的创建知识（校验、组装）各自独立，改一个不碰其他。
 */
public interface NodeFactory {
    NodeDef create(NodeSpec spec);
}
