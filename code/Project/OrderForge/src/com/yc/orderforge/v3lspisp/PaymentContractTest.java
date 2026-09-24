package com.yc.orderforge.v3lspisp;

import com.yc.orderforge.shared.Money;
import com.yc.orderforge.shared.testkit.Assert;
import com.yc.orderforge.shared.testkit.TestRunner;

/**
 * 支付契约测试：用统一的测试套件验证【所有】实现是否守住 PaymentMethod 契约。
 *
 * 这是 LSP 的可执行形态——04 篇的"契约检查表"变成代码：
 * 只要新写一个 PaymentMethod 实现，把它加进 IMPLS 数组，
 * 契约测试立刻告诉你它有没有违约。
 */
public class PaymentContractTest {

    /** 参与契约测试的全部实现（新增实现 → 加进这里 → 自动接受体检） */
    private static final PaymentMethod[] IMPLS = {
        new BalancePayment(Money.ofYuan(10000)),
        new CreditCardPayment(Money.ofYuan(5000)),
        // new BrokenCreditCardPayment(),  ← 故意不放：它是"标本"，见第 1 个用例
    };

    public static void main(String[] args) {
        new TestRunner("v3.LSP 支付契约")
            .test("违反标本：BrokenCreditCard 抛了契约外的异常类型", () -> {
                // 04 篇原案现场：按契约应抛 PaymentRejected，实际抛 IllegalArgumentException
                Assert.throwsException(IllegalArgumentException.class,
                    () -> new BrokenCreditCardPayment().pay(Money.ofYuan(6000)));
                System.out.println("     → 若上游统一 catch PaymentRejected，此异常会漏网炸穿 —— LSP 违反实锤");
            })
            .test("违反标本：singleLimit 未声明限额（前置条件被暗中加强）", () -> {
                Money limit = new BrokenCreditCardPayment().singleLimit();
                Assert.trueThat(limit.greaterThan(Money.ofYuan(5000)),
                    "契约声称不限额，pay 却拒绝 >5000 —— 言行不一");
            })
            .test("修复版：CreditCardPayment 超限抛统一 PaymentRejected", () -> {
                Assert.throwsException(PaymentRejectedException.class,
                    () -> new CreditCardPayment(Money.ofYuan(5000)).pay(Money.ofYuan(6000)));
            })
            .test("修复版：限额成为契约一部分（singleLimit=5000）", () -> {
                Assert.equals(Money.ofYuan(5000),
                    new CreditCardPayment(Money.ofYuan(5000)).singleLimit(),
                    "限额公开可查，路由器可提前拦截");
            })
            .test("契约：所有实现正数金额要么成功要么统一异常（逐个体检）", () -> {
                for (PaymentMethod m : IMPLS) {
                    // 契约内的合法输入（<= 限额）必须被正常处理
                    String tx = m.pay(Money.ofYuan(100));
                    Assert.trueThat(tx != null && !tx.isEmpty(),
                        m.name() + " 应返回非空流水号（后置条件）");
                }
            })
            .test("契约：失败不改变内部状态（不变式：余额不足时余额不变）", () -> {
                BalancePayment balance = new BalancePayment(Money.ofYuan(100));
                Assert.throwsException(PaymentRejectedException.class,
                    () -> balance.pay(Money.ofYuan(200)));
                Assert.equals(Money.ofYuan(100), balance.wallet(),
                    "失败的支付不得改变余额（不变式）");
            })
            .test("路由器：超限在下单前拦截（BUG 2 的结构性消灭）", () -> {
                PaymentRouter router = new PaymentRouter();
                Assert.throwsException(PaymentRejectedException.class,
                    () -> router.pay(new CreditCardPayment(Money.ofYuan(5000)),
                        Money.ofYuan(6000)));
                System.out.println("     → 拦截发生在扣库存之前，V0 的'库存凭空消失'不再可能");
            })
            .run();
    }
}
