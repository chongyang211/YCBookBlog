package com.yc.payflow.v3composition.impl;

import com.yc.payflow.v3composition.ability.Shippable;

/** 标准配送（普通物流，满 99 包邮，否则 12 元）。 */
public class StandardShipping implements Shippable {
    private static final long FREE_THRESHOLD_CENTS = 9900;
    private static final long FLAT_FEE_CENTS = 1200;

    @Override
    public long shippingFeeCents(String city, long subtotalCents) {
        return subtotalCents >= FREE_THRESHOLD_CENTS ? 0 : FLAT_FEE_CENTS;
    }
}
