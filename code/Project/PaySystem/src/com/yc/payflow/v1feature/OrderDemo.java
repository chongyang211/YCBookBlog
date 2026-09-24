package com.yc.payflow.v1feature;

import com.yc.payflow.shared.Money;

/**
 * V1 Demo：验证封装 + 多态两大特性。
 *
 * 输出重点：
 *   [封装] withdraw 超额扣款 → 抛异常，余额守恒
 *   [多态] 主流程只调 policy.applyDiscount()，运行时按等级分发
 */
public class OrderDemo {
    public static void main(String[] args) {
        System.out.println("================ V1 特性版 Demo ================");
        demoEncapsulation();
        System.out.println();
        demoPolymorphism();
    }

    /** 封装现场：Wallet 守住"余额不能为负"的不变量 */
    private static void demoEncapsulation() {
        System.out.println("[封装] Wallet 不变量守卫");
        Wallet w = new Wallet(Money.ofYuan(100));
        w.withdraw(Money.ofYuan(30));
        System.out.println("  扣款 30 后余额: " + w.balance());
        try {
            w.withdraw(Money.ofYuan(1000));
        } catch (InsufficientFundsException e) {
            System.out.println("  超额扣款被拒绝: " + e.getMessage());
        }
        System.out.println("  余额未被破坏: " + w.balance());
    }

    /** 多态现场：同一段调用代码，运行时按等级分发到不同策略 */
    private static void demoPolymorphism() {
        System.out.println("[多态] VipPolicy 消灭 switch");
        Money subTotal = Money.ofYuan(500);
        for (int level = 0; level <= 3; level++) {
            VipPolicy policy = VipPolicies.of(level);
            Money paid = policy.applyDiscount(subTotal);
            System.out.println("  " + policy.levelName() + " 折后: " + paid);
        }
    }
}
