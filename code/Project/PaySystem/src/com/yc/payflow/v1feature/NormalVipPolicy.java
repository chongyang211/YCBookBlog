package com.yc.payflow.v1feature;

import com.yc.payflow.shared.Money;

/** 普通用户：无折扣。 */
public class NormalVipPolicy implements VipPolicy {
    @Override
    public Money applyDiscount(Money original) {
        return original;
    }

    @Override
    public String levelName() { return "NORMAL"; }
}
