package flowkit.rule;

/**
 * 解释器模式 —— AST 节点接口。
 * 每个节点都是一个小对象，这棵表达式树天然是组合模式（树形结构、统一求值接口）。
 */
public interface Expr {
    boolean eval(RuleContext ctx);
}
