package com.yc.orderforge.v3lspisp;

import com.yc.orderforge.shared.Money;

import java.util.concurrent.atomic.AtomicLong;

/**
 * 信用卡支付（V3 修复版）：限额成为契约的一部分。
 *
 * 对照 V0 的 BUG 2：
 *   V0：限额检查藏在支付中途，抛 IllegalStateException —— 前置加强 + 异常类型违约
 *   V3：singleLimit() 公开限额，超限抛统一 PaymentRejected —— 契约自洽
 * 调用方（PaymentRouter）拿到限额即可在下单前拦截，库存再也不会白扣。
 */
public class CreditCardPayment implements PaymentMethod {

    private static final AtomicLong TX_SEQ = new AtomicLong();

    private final Money limit;

    public CreditCardPayment(Money limit) {
        this.limit = limit;
    }

    @Override
    public String pay(Money amount) {
        if (amount == null || !amount.isPositive()) {
            throw new PaymentRejectedException("支付金额必须为正: " + amount);
        }
        if (amount.greaterThan(limit)) {
            throw new PaymentRejectedException(
                name() + "单笔限额 " + limit + "，当前 " + amount + "，请拆单或换渠道");
        }
        return "CARD-" + TX_SEQ.incrementAndGet();
    }

    @Override public String name() { return "信用卡"; }

    @Override public Money singleLimit() { return limit; }
}
