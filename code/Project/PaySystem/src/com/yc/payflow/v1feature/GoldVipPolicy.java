package com.yc.payflow.v1feature;

import com.yc.payflow.shared.Money;

/** 黄金会员：9 折。 */
public class GoldVipPolicy implements VipPolicy {
    @Override
    public Money applyDiscount(Money original) {
        return original.percentOff(10);
    }

    @Override
    public String levelName() { return "GOLD"; }
}
