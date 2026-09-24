package com.yc.payflow.v3composition.impl;

import com.yc.payflow.v3composition.ability.ColdChainable;

/** 冷链附加费（8 元/单）。 */
public class ColdChainSurcharge implements ColdChainable {
    private static final long SURCHARGE_CENTS = 800;
    @Override
    public long coldChainSurchargeCents() {
        return SURCHARGE_CENTS;
    }
}
