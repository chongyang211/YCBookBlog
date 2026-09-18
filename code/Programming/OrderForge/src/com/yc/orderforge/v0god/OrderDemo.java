package com.yc.orderforge.v0god;

import com.yc.orderforge.domain.Address;
import com.yc.orderforge.domain.City;
import com.yc.orderforge.domain.Customer;
import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;

/**
 * V0 Demo：跑 4 个订单，亲眼看见 3 个真实 bug。
 *   订单 1：正常小单——屎山的伪装，一切良好
 *   订单 2：GOLD 会员 1000 元 → BUG 1：多收 125 元
 *   订单 3：信用卡 6000 元   → BUG 2：库存扣了、订单失败、库存消失
 *   订单 4：游客无地址       → BUG 3：已扣款、通知 NPE、订单卡死
 *
 * 运行：./run.sh v0
 */
public class OrderDemo {
    public static void main(String[] args) {
        System.out.println("========== V0 上帝类演示（订单履约链路）==========");

        // 订单 1：正常订单（399 元小单）
        Order o1 = new Order("O-001",
            new Customer("张三", "138****0001", new Address(new City("北京"))),
            false, false, null, 0)
            .addLine("SKU-A", Money.ofYuan(299), 1)
            .addLine("SKU-B", Money.ofYuan(100), 1);
        System.out.println("[订单 1] 399 元普通单，期望实付 349（满300减50）");
        new OrderManager().placeOrder(o1);
        System.out.println("  库存=" + OrderManager.STOCK_SKU_A
            + " 钱包=" + OrderManager.WALLET + "\n");

        // 订单 2：GOLD 会员 1000 元 → BUG 1
        Order o2 = new Order("O-002",
            new Customer("李四", "138****0002", new Address(new City("上海"))),
            true, false, null, 0)
            .addLine("SKU-A", Money.ofYuan(600), 1)
            .addLine("SKU-B", Money.ofYuan(400), 1);
        System.out.println("[订单 2] GOLD 会员 1000 元，期望实付 (1000-250)*0.9=¥675");
        new OrderManager().placeOrder(o2);
        System.out.println("  ↑ 实际收了 ¥800 —— 【BUG 1】会员折扣先于满减，多收 ¥125\n");

        // 订单 3：信用卡 6000 元 → BUG 2（库存已扣，异常抛出）
        Order o3 = new Order("O-003",
            new Customer("王五", "138****0003", new Address(new City("深圳"))),
            false, false, null, 1)
            .addLine("SKU-A", Money.ofYuan(3000), 2);
        System.out.println("[订单 3] 信用卡 6000 元（单笔限额 5000）");
        try {
            new OrderManager().placeOrder(o3);
        } catch (IllegalStateException e) {
            System.out.println("  支付异常: " + e.getMessage());
        }
        System.out.println("  异常后库存 = " + OrderManager.STOCK_SKU_A
            + "（已扣 2 件未回滚！）—— 【BUG 2】库存凭空消失\n");

        // 订单 4：游客无地址 → BUG 3（已扣款，通知 NPE）
        Order o4 = new Order("O-004",
            new Customer("游客赵六", "138****0004", null),   // ← 地址为空
            false, true, null, 0)
            .addLine("SKU-B", Money.ofYuan(88), 1);
        System.out.println("[订单 4] 游客 88 元（无地址，运费 12）");
        try {
            new OrderManager().placeOrder(o4);
        } catch (NullPointerException e) {
            System.out.println("  通知 NPE: getCustomer().getAddress().getCity()... 空地址");
        }
        System.out.println("  钱包 = " + OrderManager.WALLET
            + "（已扣 70）—— 【BUG 3】钱扣了、通知崩了、订单卡死\n");

        System.out.println("========== V0 结束：上帝类 1 个 / 坏味道 12 处 / 真实 bug 3 个 ==========");
        System.out.println("下一站：./run.sh metrics —— 用圈复杂度和坏味道扫描给 V0 出体检报告");
    }
}
