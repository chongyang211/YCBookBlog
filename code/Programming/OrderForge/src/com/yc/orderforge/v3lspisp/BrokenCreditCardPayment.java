package com.yc.orderforge.v3lspisp;

import com.yc.orderforge.shared.Money;

/**
 * ⚠️ LSP 违反标本（04 篇 CreditCardPayment 原案，故意保留供契约测试抓获）：
 *
 *   1. singleLimit() 没有覆写 → 契约上"不限额"
 *   2. pay() 却在内部拒绝 > 5000 → 前置条件被加强（第 1 条规则违反）
 *   3. 抛的是 IllegalArgumentException → 契约外异常（第 3 条规则违反）
 *
 * 编译能过、单独测能过；但只要上游按 PaymentMethod 契约统一 catch
 * PaymentRejectedException，它就会"漏网"炸到最外层——
 * 这就是"看着合理、处处是坑"的继承（04 篇 PhoneInput 同款）。
 */
public class BrokenCreditCardPayment implements PaymentMethod {

    @Override
    public String pay(Money amount) {
        if (amount.greaterThan(Money.ofYuan(5000))) {
            // ← 契约说"不限额 + 只抛 PaymentRejected"，这里两条都违反
            throw new IllegalArgumentException("信用卡支付金额不能超过5000元");
        }
        return "BROKEN-CARD-TX";
    }

    @Override public String name() { return "带病信用卡"; }
}
