package flowkit.rule;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * 递归下降 Parser —— 文法的每条规则对应一个方法，调用层次即优先级层次：
 * orExpr（最外层，优先级最低）→ andExpr → cmp（最内层，优先级最高）。
 * AND 在下层，所以绑得更紧：a OR b AND c 解析为 a OR (b AND c)。
 */
public class ExprParser {
    private final List<String> tokens;
    private int pos = 0;

    public static Expr parse(String source) { return new ExprParser(lex(source)).orExpr(); }

    private ExprParser(List<String> tokens) { this.tokens = tokens; }

    // ---------- 文法三层，一层一个方法 ----------
    private Expr orExpr() {
        List<Expr> list = new ArrayList<>(List.of(andExpr()));
        while (peek("OR")) { next(); list.add(andExpr()); }
        return list.size() == 1 ? list.get(0) : new OrExpr(list);
    }

    private Expr andExpr() {
        List<Expr> list = new ArrayList<>(List.of(cmp()));
        while (peek("AND")) { next(); list.add(cmp()); }
        return list.size() == 1 ? list.get(0) : new AndExpr(list);
    }

    private Expr cmp() {
        String field = next();                       // amount
        String op = next();                          // >
        String literal = next();                     // 5000 或 '研发'
        if (literal.startsWith("'"))                 // 去引号
            literal = literal.substring(1, literal.length() - 1);
        if (!op.matches("[><=!]=?"))
            throw new IllegalStateException("非法运算符: " + op);
        return new CmpExpr(field, op, literal);
    }

    // ---------- 词法：语法简单到正则切词就够 ----------
    private static List<String> lex(String source) {
        List<String> out = new ArrayList<>();
        Matcher m = Pattern.compile("'[^']*'|[><=!]=?|[\\w.]+").matcher(source);
        while (m.find()) out.add(m.group());
        if (out.isEmpty()) throw new IllegalStateException("空表达式");
        return out;
    }

    private boolean peek(String kw) {
        return pos < tokens.size() && tokens.get(pos).equals(kw);
    }

    private String next() {
        if (pos >= tokens.size()) throw new IllegalStateException("表达式不完整");
        return tokens.get(pos++);
    }
}
