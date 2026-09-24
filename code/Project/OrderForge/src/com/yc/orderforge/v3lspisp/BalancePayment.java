package com.yc.orderforge.v3lspisp;

import com.yc.orderforge.shared.Money;

import java.util.UUID;
import java.util.concurrent.atomic.AtomicLong;

/**
 * 余额支付：遵守契约的标准实现。
 * - 任何正数金额都接受（前置条件未加强）
 * - 余额不足抛统一 PaymentRejected 且余额不变（不变式）
 */
public class BalancePayment implements PaymentMethod {

    private static final AtomicLong TX_SEQ = new AtomicLong();

    private Money wallet;

    public BalancePayment(Money initial) {
        this.wallet = initial;
    }

    @Override
    public String pay(Money amount) {
        if (amount == null || !amount.isPositive()) {
            throw new PaymentRejectedException("支付金额必须为正: " + amount);
        }
        if (amount.greaterThan(wallet)) {
            throw new PaymentRejectedException(
                "余额不足：需要 " + amount + "，当前 " + wallet);
        }
        wallet = wallet.minus(amount);
        return "BAL-" + TX_SEQ.incrementAndGet();
    }

    @Override public String name() { return "余额"; }

    public Money wallet() { return wallet; }
}
