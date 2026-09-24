package com.yc.payflow.v3composition.ability;

/** 可退款能力（虚拟课程 7 天无理由、生鲜不可退——由业务决定谁持有）。 */
public interface Refundable {
    /** 返回退款窗口（天）；0 表示不可退。 */
    int refundWindowDays();
}
