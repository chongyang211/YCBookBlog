package flowkit.rule;

/** 比较表达式：amount > 5000 / dept == '研发' */
public record CmpExpr(String field, String op, Object literal) implements Expr {
    @Override
    public boolean eval(RuleContext ctx) {
        Object v = ctx.get(field);                     // amount → 4800
        return switch (op) {
            case ">"  -> toLong(v) >  toLong(literal);
            case "<"  -> toLong(v) <  toLong(literal);
            case ">=" -> toLong(v) >= toLong(literal);
            case "<=" -> toLong(v) <= toLong(literal);
            case "==" -> String.valueOf(v).equals(literal.toString());
            case "!=" -> !String.valueOf(v).equals(literal.toString());
            default   -> throw new IllegalStateException("未知运算符: " + op);
        };
    }

    private static long toLong(Object o) { return Long.parseLong(o.toString()); }
}
