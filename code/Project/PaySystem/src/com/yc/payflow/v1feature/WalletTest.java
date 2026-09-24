package com.yc.payflow.v1feature;

import com.yc.payflow.shared.Money;
import com.yc.payflow.shared.testkit.Assert;
import com.yc.payflow.shared.testkit.TestRunner;

/**
 * WalletTest：验证封装的不变量守卫。
 * 对应博客第 02 篇「四大特性 · 封装」+ 第 10 篇「可测试性」。
 */
public class WalletTest {

    public static void main(String[] args) {
        new TestRunner("v1.Wallet")
            .test("初始化正常余额", () -> {
                Wallet w = new Wallet(Money.ofYuan(100));
                Assert.equals(Money.ofYuan(100), w.balance(), "初始余额应为 100");
            })
            .test("初始化负数余额被拒", () -> {
                Assert.throwsException(IllegalArgumentException.class,
                    () -> new Wallet(Money.ofCents(-1)));
            })
            .test("扣款成功后余额减少", () -> {
                Wallet w = new Wallet(Money.ofYuan(100));
                w.withdraw(Money.ofYuan(30));
                Assert.equals(Money.ofYuan(70), w.balance(), "扣款 30 后应剩 70");
            })
            .test("超额扣款抛异常且余额不变（不变量守卫）", () -> {
                Wallet w = new Wallet(Money.ofYuan(100));
                Assert.throwsException(InsufficientFundsException.class,
                    () -> w.withdraw(Money.ofYuan(200)));
                Assert.equals(Money.ofYuan(100), w.balance(), "扣款失败后余额应保持 100");
            })
            .test("零金额扣款被拒（金额必须为正）", () -> {
                Wallet w = new Wallet(Money.ofYuan(100));
                Assert.throwsException(IllegalArgumentException.class,
                    () -> w.withdraw(Money.ZERO));
            })
            .test("退款后余额恢复（充放守恒）", () -> {
                Wallet w = new Wallet(Money.ofYuan(100));
                w.withdraw(Money.ofYuan(30));
                w.refund(Money.ofYuan(30));
                Assert.equals(Money.ofYuan(100), w.balance(), "扣款+退款应守恒");
            })
            .run();
    }
}
