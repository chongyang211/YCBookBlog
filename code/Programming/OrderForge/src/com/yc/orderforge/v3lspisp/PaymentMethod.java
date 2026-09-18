package com.yc.orderforge.v3lspisp;

import com.yc.orderforge.shared.Money;

/**
 * 支付方式契约（04 篇 DbC 契约式设计的落地）。
 *
 * 【本接口的契约】（每个实现者必须遵守，调用方可以放心依赖）：
 *
 *   前置条件：amount 必须为正；amount 不得超过 singleLimit()
 *   后置条件：pay() 正常返回 = 扣款成功并返回非空流水号；
 *             任何失败都抛 PaymentRejectedException（且不扣款）
 *   不变式  ：失败调用前后，实现内部余额不变
 *
 * 对照 04 篇 CreditCardPayment 案例：
 *   V0 的写法（金额>5000 抛 IllegalStateException）= 前置条件被加强 +
 *   抛契约外异常 —— 两连违反。
 *   V3 的修复：限额成为契约的一部分（singleLimit()），
 *   调用方（PaymentRouter）在下单前就知道每个渠道的边界。
 */
public interface PaymentMethod {

    /** 发起支付：成功返回流水号，失败抛 PaymentRejectedException 且不扣款 */
    String pay(Money amount);

    /** 渠道名 */
    String name();

    /** 单笔限额（契约的一部分，默认不限额；调用方须在下单前检查） */
    default Money singleLimit() {
        return Money.ofCents(Long.MAX_VALUE);
    }
}
