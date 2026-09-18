package com.yc.orderforge.v1srp;

import com.yc.orderforge.domain.Address;
import com.yc.orderforge.domain.City;
import com.yc.orderforge.domain.Customer;
import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;

/**
 * V1 SRP 手术 Demo：
 *   ① 六件事拆六个类，OrderService 只做编排（圈复杂度 16 → 4）
 *   ② 支付失败自动回滚库存（V0 的 BUG 2 后遗症止血）
 *   ③ 诚实展示：BUG 1 / BUG 3 仍在——SRP 不修业务 bug，只修结构
 *
 * 运行：./run.sh v1
 */
public class OrderDemo {
    public static void main(String[] args) {
        System.out.println("========== V1 SRP 手术演示 ==========");
        OrderService service = new OrderService();

        // 订单 1：正常单
        Order o1 = new Order("O-101",
            new Customer("张三", "138****0001", new Address(new City("北京"))),
            false, false, null, 0)
            .addLine("SKU-A", Money.ofYuan(299), 1)
            .addLine("SKU-B", Money.ofYuan(100), 1);
        service.placeOrder(o1);
        System.out.println("  [验收①] 正常单走通，库存=" + service.inventory().stock()
            + "，钱包=" + service.payment().wallet() + "\n");

        // 订单 2：GOLD 1000 元 —— BUG 1 仍在（SRP 只修结构不修 bug）
        Order o2 = new Order("O-102",
            new Customer("李四", "138****0002", new Address(new City("上海"))),
            true, false, null, 0)
            .addLine("SKU-A", Money.ofYuan(600), 1)
            .addLine("SKU-B", Money.ofYuan(400), 1);
        service.placeOrder(o2);
        System.out.println("  [诚实展示] GOLD 1000 元仍收 ¥800 —— BUG 1 属于计价逻辑，留给 V2 (OCP)\n");

        // 订单 3：信用卡 6000 元 —— V1 核心修复：库存补偿
        Order o3 = new Order("O-103",
            new Customer("王五", "138****0003", new Address(new City("深圳"))),
            false, false, null, 1)
            .addLine("SKU-A", Money.ofYuan(3000), 2);
        int stockBefore = service.inventory().stock();
        try {
            service.placeOrder(o3);
        } catch (IllegalStateException e) {
            System.out.println("  [验收②] 支付异常: " + e.getMessage());
            System.out.println("  [验收②] 库存 " + stockBefore + " → "
                + service.inventory().stock() + "（自动回滚，不再凭空消失）\n");
        }

        // 订单 4：游客无地址 —— BUG 3 仍在
        Order o4 = new Order("O-104",
            new Customer("游客赵六", "138****0004", null),
            false, true, null, 0)
            .addLine("SKU-B", Money.ofYuan(88), 1);
        try {
            service.placeOrder(o4);
        } catch (NullPointerException e) {
            System.out.println("  [诚实展示] 通知 NPE 仍发生 —— 火车残骸属于耦合问题，留给 V4 (DIP+LOD)");
        }

        System.out.println("\n  [验收③] 每个服务可独立单测："
            + "./run.sh v1 的测试部分见 V1SrpTest");
        System.out.println("========== V1 结束：6 个类各司其职，placeOrder 圈复杂度 16 → 4 ==========");
    }
}
