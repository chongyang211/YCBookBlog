package com.yc.payflow.v1feature;

import com.yc.payflow.shared.Money;

/**
 * ============================================================
 *  VipPolicy 会员折扣策略（对应博客第 02 篇「多态」）
 *  ------------------------------------------------------------
 *  V0 的问题（回顾）：
 *    if (vipLevel == 3)  total *= 0.85;
 *    else if (vipLevel == 2) total *= 0.9;
 *    else if (vipLevel == 1) total *= 0.95;
 *    ↑ Switch 惊悚：每加一档会员都要改这段代码
 *
 *  V1 的改造（多态消灭 switch）：
 *    - 抽象一个 VipPolicy 接口
 *    - 每个等级一个实现类：Normal / Silver / Gold / Platinum
 *    - 主流程只调用 policy.applyDiscount(m)，运行时多态分发
 *
 *  【新增会员等级只加一个类，主流程零修改】——这就是"对扩展开放"
 * ============================================================
 */
public interface VipPolicy {
    /**
     * 应用会员折扣，返回打折后的金额。
     * @param original 折扣前金额
     */
    Money applyDiscount(Money original);

    /** 会员等级标签，用于日志与打印 */
    String levelName();
}
