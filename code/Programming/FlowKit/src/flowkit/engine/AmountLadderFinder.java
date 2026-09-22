package flowkit.engine;

import flowkit.domain.Ticket;

/** 策略三：金额阶梯（1000 以下主管、5000 以下财务总监、以上 CFO）。 */
public class AmountLadderFinder implements ApproverFinder {
    @Override public String find(Ticket t) {
        long yuan = t.getAmount() / 100;
        if (yuan > 5000) return "cfo";
        if (yuan > 1000) return "finance_director";
        return "manager";
    }
}
