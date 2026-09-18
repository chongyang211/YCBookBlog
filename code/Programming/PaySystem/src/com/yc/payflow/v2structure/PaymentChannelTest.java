package com.yc.payflow.v2structure;

import com.yc.payflow.shared.Money;
import com.yc.payflow.shared.testkit.Assert;
import com.yc.payflow.shared.testkit.TestRunner;
import com.yc.payflow.v2structure.payment.*;

/**
 * PaymentChannelTest：验证支付渠道接口 + 装饰器可组合性。
 */
public class PaymentChannelTest {

    public static void main(String[] args) {
        new TestRunner("v2.PaymentChannel")
            .test("微信通道成功返回流水号", () -> {
                String txn = new WechatChannel().pay("O1", Money.ofYuan(10));
                Assert.notNull(txn, "流水号不应为空");
                Assert.trueThat(txn.startsWith("WX-"), "微信流水号应以 WX- 开头");
            })
            .test("重试装饰器：抖动 2 次后第 3 次成功", () -> {
                PaymentChannel ch = new RetryableChannel(new FlakyChannel("TEST", 2), 3);
                String txn = ch.pay("O2", Money.ofYuan(10));
                Assert.notNull(txn, "最终应成功返回流水号");
            })
            .test("重试装饰器：全部失败时抛异常", () -> {
                PaymentChannel ch = new RetryableChannel(new FlakyChannel("TEST", 10), 3);
                Assert.throwsException(PaymentException.class,
                    () -> ch.pay("O3", Money.ofYuan(10)));
            })
            .test("装饰器可嵌套（重试套重试）", () -> {
                // 内层允许 2 次，外层再套 2 次 → 总共 2*2=4 次机会
                PaymentChannel base = new FlakyChannel("BASE", 3);
                PaymentChannel inner = new RetryableChannel(base, 2);
                PaymentChannel outer = new RetryableChannel(inner, 2);
                String txn = outer.pay("O4", Money.ofYuan(1));
                Assert.notNull(txn, "嵌套装饰器应最终成功");
            })
            .run();
    }
}
