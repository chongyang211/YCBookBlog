package com.yc.orderforge.v3lspisp;

import com.yc.orderforge.shared.Money;

/**
 * V3 LSP+ISP 手术 Demo。
 *
 * 运行：./run.sh v3
 */
public class OrderDemo {
    public static void main(String[] args) {
        System.out.println("========== V3 LSP + ISP 手术演示 ==========");

        System.out.println("[第一幕 LSP] 支付契约：限额不再是'暗坑'而是'契约'\n");
        System.out.print("  带病实现（V0 写法）: ");
        try {
            new BrokenCreditCardPayment().pay(Money.ofYuan(6000));
        } catch (IllegalArgumentException e) {
            System.out.println("抛出 IllegalArgumentException（契约外类型！）");
        }
        System.out.print("  修复实现（V3 写法）: ");
        try {
            new CreditCardPayment(Money.ofYuan(5000)).pay(Money.ofYuan(6000));
        } catch (PaymentRejectedException e) {
            System.out.println("抛出 PaymentRejectedException（契约统一类型 ✓）");
        }
        System.out.print("  路由器提前拦截:     ");
        try {
            new PaymentRouter().pay(new CreditCardPayment(Money.ofYuan(5000)),
                Money.ofYuan(6000));
        } catch (PaymentRejectedException e) {
            System.out.println(e.getMessage());
        }
        System.out.println("  → 超限失败发生在扣库存【之前】，BUG 2 被结构性消灭\n");

        System.out.println("[第二幕 ISP] 角色接口：买家不该看见'导报表'\n");
        System.out.println("  胖接口世界（OrderOperations 7 个方法）:");
        new FatInterfaceBuyer().placeOrder();
        System.out.println("    ……还有 5 个方法在抛 UnsupportedOperationException 空壳\n");
        System.out.println("  角色接口世界（每个接口只给自己的角色用）:");
        new BuyerUser().placeOrder();
        new AdminUser().exportFinancialReport();
        System.out.println();
        System.out.println("========== V3 结束：契约守 LSP，角色接口守 ISP ==========");
        System.out.println("完整契约测试：./run.sh v3 的测试部分（PaymentContractTest + RoleInterfaceTest）");
    }
}
