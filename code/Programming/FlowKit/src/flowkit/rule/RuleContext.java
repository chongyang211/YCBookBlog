package flowkit.rule;

import flowkit.domain.Ticket;

import java.util.HashMap;
import java.util.Map;

/** 表达式的数据源：把工单字段适配成表达式变量。 */
public class RuleContext {
    private final Map<String, Object> vars = new HashMap<>();

    public static RuleContext of(Ticket t) {
        RuleContext c = new RuleContext();
        c.vars.put("amount", t.getAmount() / 100);      // 统一成"元"
        c.vars.put("dept", "研发");
        c.vars.put("applicant", t.getApplicant());
        return c;
    }

    public Object get(String field) {
        Object v = vars.get(field);
        if (v == null) throw new IllegalStateException("未知字段: " + field);
        return v;
    }
}
