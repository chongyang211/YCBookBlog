package com.yc.payflow.v3composition.ability;

/** 跨境能力（跨境商品持有，需征税与清关）。 */
public interface CrossBorderable {
    /** 返回跨境税率（如 0.119 表示 11.9%） */
    double taxRate();
}
