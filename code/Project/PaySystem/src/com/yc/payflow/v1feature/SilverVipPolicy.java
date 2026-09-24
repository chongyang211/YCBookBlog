package com.yc.payflow.v1feature;

import com.yc.payflow.shared.Money;

/** 白银会员：95 折。 */
public class SilverVipPolicy implements VipPolicy {
    @Override
    public Money applyDiscount(Money original) {
        return original.percentOff(5);
    }

    @Override
    public String levelName() { return "SILVER"; }
}
