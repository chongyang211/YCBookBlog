package com.yc.orderforge.v1srp;

import com.yc.orderforge.shared.Money;

/**
 * 支付服务（V1 拆出）。
 *
 * ⚠️ BUG 2 的"抛异常时机"仍在（信用卡超限照样中途炸）——
 *    但现在 OrderService 能 catch 它并触发库存补偿（结构拆分带来的能力）。
 *    超限"契约化 + 提前拦截"是 V3（LSP）的活。
 */
public class PaymentService {
    private Money wallet = Money.ofYuan(10000);

    public void charge(int payType, Money amount) {
        if (payType == 1 && amount.greaterThan(Money.ofYuan(5000))) {
            throw new IllegalStateException("信用卡单笔限额 5000，支付失败");
        }
        if (amount.greaterThan(wallet)) {
            throw new IllegalStateException("余额不足");
        }
        wallet = wallet.minus(amount);
    }

    public Money wallet() { return wallet; }
}
