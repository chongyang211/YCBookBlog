package flowkit.rule;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

/**
 * AST 缓存 —— 解释器模式在工程里的生死线：
 * 规则不变，AST 就不变。缓存了它是"规则引擎"，不缓存它是"性能事故"。
 */
public final class ExprCache {
    private static final Map<String, Expr> CACHE = new ConcurrentHashMap<>();

    private ExprCache() {}

    public static Expr of(String source) {
        return CACHE.computeIfAbsent(source, ExprParser::parse);
    }

    /** 教学观测：当前缓存的表达式数量。 */
    public static int size() { return CACHE.size(); }
}
