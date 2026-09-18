package com.yc.orderforge.v3lspisp;

import com.yc.orderforge.shared.testkit.Assert;
import com.yc.orderforge.shared.testkit.TestRunner;

/** V3 ISP 单元测试：角色接口的度量验收（05 篇四个量化信号的可执行版）。 */
public class RoleInterfaceTest {

    public static void main(String[] args) {
        new TestRunner("v3.ISP 角色接口")
            .test("胖接口：买家被迫实现 7 个方法，5 个是空壳", () -> {
                FatInterfaceBuyer buyer = new FatInterfaceBuyer();
                // 真正需要的 2 个方法正常工作
                buyer.placeOrder();
                buyer.cancelOrder();
                // 被迫营业的 5 个方法全是雷
                int mines = 0;
                String[] ops = {"退款", "查单", "改价", "调库存", "导报表"};
                for (String op : ops) {
                    try {
                        if (op.equals("退款")) buyer.refundOrder();
                        else if (op.equals("查单")) buyer.viewOrder();
                        else if (op.equals("改价")) buyer.modifyPrice();
                        else if (op.equals("调库存")) buyer.adjustStock();
                        else buyer.exportFinancialReport();
                    } catch (UnsupportedOperationException e) {
                        mines++;
                    }
                }
                Assert.equals(5, mines, "7 个方法里 5 个是空壳雷（使用比 2/7 = 29%）");
            })
            .test("角色接口：买家只依赖 2 个方法（使用比 100%）", () -> {
                BuyerOperations buyer = new BuyerUser();   // 客户端只见 BuyerOperations
                buyer.placeOrder();
                buyer.cancelOrder();
                // buyer.modifyPrice() ← 编译期就不可见，不再是运行期炸弹
            })
            .test("角色接口：运营按需组合多角色", () -> {
                AdminOperations admin = new AdminUser();
                admin.modifyPrice();
                admin.exportFinancialReport();
            })
            .test("ISP 度量：接口方法数均 <=3（健康范围 1~5）", () -> {
                Assert.equals(2, BuyerOperations.class.getMethods().length,
                    "BuyerOperations 应只有 2 个方法");
                Assert.equals(3, AdminOperations.class.getMethods().length,
                    "AdminOperations 应只有 3 个方法");
                Assert.equals(7, OrderOperations.class.getMethods().length,
                    "胖接口 OrderOperations 有 7 个方法（>5，超标）");
            })
            .run();
    }
}
