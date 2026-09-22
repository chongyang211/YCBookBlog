package flowkit.rule;

import java.util.List;

/** AND 表达式：全真才真。 */
public record AndExpr(List<Expr> children) implements Expr {
    @Override
    public boolean eval(RuleContext ctx) {
        return children.stream().allMatch(e -> e.eval(ctx));
    }
}
