package com.yc.payflow.v3composition.impl;

import com.yc.payflow.v3composition.ability.CrossBorderable;

/** 跨境税（11.9%）。 */
public class CrossBorderTax implements CrossBorderable {
    @Override public double taxRate() { return 0.119; }
}
