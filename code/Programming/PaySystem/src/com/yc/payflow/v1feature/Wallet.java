package com.yc.payflow.v1feature;

import com.yc.payflow.shared.Money;

/**
 * ============================================================
 *  Wallet 钱包聚合（对应博客第 02 篇「四大特性 · 封装」）
 *  ------------------------------------------------------------
 *  V0 的问题（回顾）：
 *    - static WALLET_BALANCE 直接 -= amount，任何代码都能改
 *    - 没有不变量校验：可能扣成 -37.5，账不平
 *
 *  V1 的改造（封装的四个动作）：
 *    ① 字段 private final（不可被外部读写）
 *    ② 构造时校验（初始余额不能为负）
 *    ③ 意图方法而非步骤方法：withdraw/refund/deposit（隐藏实现）
 *    ④ 不变量在方法内强制守卫（withdraw 前校验余额）
 *
 *  ↑ 这四个动作是"真 OOP"和"伪 OOP"的分水岭。
 * ============================================================
 */
public class Wallet {
    private Money balance;

    public Wallet(Money initial) {
        if (initial == null || initial.isNegative()) {
            throw new IllegalArgumentException("初始余额不能为负: " + initial);
        }
        this.balance = initial;
    }

    /**
     * 扣款——业务意图方法（对应"意图方法而非步骤方法"）。
     *
     * 【不变量守卫】：
     *   - 金额必须为正
     *   - 扣款后余额不能为负
     *
     * 【原子性】：判断 + 修改在同一方法内完成，不给调用方任何可乘之机
     */
    public void withdraw(Money amount) {
        if (amount == null || !amount.isPositive()) {
            throw new IllegalArgumentException("扣款金额必须为正: " + amount);
        }
        if (amount.greaterThan(balance)) {
            throw new InsufficientFundsException(
                "余额不足：需要 " + amount + "，当前 " + balance);
        }
        this.balance = balance.minus(amount);
    }

    /** 退款（不变量：不允许负数退款） */
    public void refund(Money amount) {
        if (amount == null || !amount.isPositive()) {
            throw new IllegalArgumentException("退款金额必须为正: " + amount);
        }
        this.balance = balance.plus(amount);
    }

    /** 充值 */
    public void deposit(Money amount) {
        if (amount == null || !amount.isPositive()) {
            throw new IllegalArgumentException("充值金额必须为正: " + amount);
        }
        this.balance = balance.plus(amount);
    }

    public Money balance() {
        return balance;
    }
}
