package com.yc.payflow.v1feature;

import com.yc.payflow.shared.Money;
import com.yc.payflow.shared.testkit.Assert;
import com.yc.payflow.shared.testkit.TestRunner;

/**
 * VipPolicyTest：验证多态分发（消灭 switch）。
 */
public class VipPolicyTest {

    public static void main(String[] args) {
        new TestRunner("v1.VipPolicy")
            .test("NORMAL 无折扣", () -> {
                Money paid = VipPolicies.of(0).applyDiscount(Money.ofYuan(100));
                Assert.equals(Money.ofYuan(100), paid, "普通用户不打折");
            })
            .test("SILVER 95 折", () -> {
                Money paid = VipPolicies.of(1).applyDiscount(Money.ofYuan(100));
                Assert.equals(Money.ofYuan(95), paid, "白银会员 95 折");
            })
            .test("GOLD 9 折", () -> {
                Money paid = VipPolicies.of(2).applyDiscount(Money.ofYuan(100));
                Assert.equals(Money.ofYuan(90), paid, "黄金会员 9 折");
            })
            .test("PLATINUM 85 折", () -> {
                Money paid = VipPolicies.of(3).applyDiscount(Money.ofYuan(100));
                Assert.equals(Money.ofYuan(85), paid, "铂金会员 85 折");
            })
            .test("未知等级降级为 NORMAL", () -> {
                Money paid = VipPolicies.of(999).applyDiscount(Money.ofYuan(100));
                Assert.equals(Money.ofYuan(100), paid, "未知等级不打折");
            })
            .test("多态可扩展：新增会员等级零修改主流程", () -> {
                // 验证接口开放：调用方代码永远只调用 VipPolicy.applyDiscount(m)
                // 新增等级只需实现接口，不需要改这个测试
                VipPolicy custom = new VipPolicy() {
                    @Override public Money applyDiscount(Money m) { return m.percentOff(30); }
                    @Override public String levelName() { return "DIAMOND"; }
                };
                Assert.equals(Money.ofYuan(70), custom.applyDiscount(Money.ofYuan(100)),
                    "钻石会员 7 折");
            })
            .run();
    }
}
