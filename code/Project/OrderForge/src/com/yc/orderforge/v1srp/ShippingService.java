package com.yc.orderforge.v1srp;

import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;

/** 运费服务（V1 拆出）：满 99 包邮，否则 12 元。 */
public class ShippingService {

    public Money fee(Order order) {
        if (order.subtotal().greaterOrEqual(Money.ofYuan(99))) {
            return Money.ZERO;
        }
        return Money.ofYuan(12);
    }
}
