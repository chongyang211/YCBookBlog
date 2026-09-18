package com.yc.payflow.v3composition.impl;

import com.yc.payflow.v3composition.ability.Refundable;

public class StandardRefundable implements Refundable {
    private final int days;
    public StandardRefundable(int days) { this.days = days; }
    @Override public int refundWindowDays() { return days; }
}
