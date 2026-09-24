package com.yc.orderforge.v3lspisp;

import com.yc.orderforge.shared.Money;

/**
 * 支付路由器：依赖"限额是契约的一部分"这件事，把失败提前到扣库存之前。
 *
 * V0 的 BUG 2 在这里被结构性消灭：
 *   下单流程先问 router（router 先问 singleLimit()），
 *   超限/余额不足都在扣库存【之前】失败——库存分毫未动。
 */
public class PaymentRouter {

    public String pay(PaymentMethod method, Money amount) {
        if (amount.greaterThan(method.singleLimit())) {
            throw new PaymentRejectedException(
                "[" + method.name() + "] 单笔限额 " + method.singleLimit()
                + "，当前 " + amount + " —— 下单前拦截，未扣库存");
        }
        return method.pay(amount);
    }
}
