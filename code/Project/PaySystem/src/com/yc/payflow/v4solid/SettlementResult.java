package com.yc.payflow.v4solid;

import com.yc.payflow.shared.Money;

import java.util.List;

/**
 * 结算结果——不可变值对象。
 */
public class SettlementResult {
    private final String orderNo;
    private final Money finalAmount;
    private final List<String> auditTrail;

    public SettlementResult(String orderNo, Money finalAmount, List<String> auditTrail) {
        this.orderNo = orderNo;
        this.finalAmount = finalAmount;
        this.auditTrail = List.copyOf(auditTrail);
    }

    public String orderNo() { return orderNo; }
    public Money finalAmount() { return finalAmount; }
    public List<String> auditTrail() { return auditTrail; }
}
