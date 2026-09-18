package com.yc.payflow.v1feature;

import com.yc.payflow.shared.Money;

/** 铂金会员：85 折。 */
public class PlatinumVipPolicy implements VipPolicy {
    @Override
    public Money applyDiscount(Money original) {
        return original.percentOff(15);
    }

    @Override
    public String levelName() { return "PLATINUM"; }
}
