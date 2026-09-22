package flowkit.rule;

import java.util.List;

/** OR 表达式：一真即真。 */
public record OrExpr(List<Expr> children) implements Expr {
    @Override
    public boolean eval(RuleContext ctx) {
        return children.stream().anyMatch(e -> e.eval(ctx));
    }
}
