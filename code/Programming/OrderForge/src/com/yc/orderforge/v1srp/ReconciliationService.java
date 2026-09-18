package com.yc.orderforge.v1srp;

import com.yc.orderforge.shared.Money;

import java.util.ArrayList;
import java.util.List;

/** 对账服务（V1 拆出）：台账独立于通知/支付，财务可单独验收。 */
public class ReconciliationService {

    private final List<String> ledger = new ArrayList<>();

    public void record(String orderNo, Money amount) {
        ledger.add("订单 " + orderNo + " 实付 " + amount);
    }

    public List<String> ledger() { return List.copyOf(ledger); }
}
