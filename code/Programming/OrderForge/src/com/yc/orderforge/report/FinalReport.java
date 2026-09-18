package com.yc.orderforge.report;

import com.yc.orderforge.domain.Address;
import com.yc.orderforge.domain.City;
import com.yc.orderforge.domain.Customer;
import com.yc.orderforge.domain.Order;
import com.yc.orderforge.metrics.CodeMetrics;
import com.yc.orderforge.shared.Money;
import com.yc.orderforge.v2ocp.DiscountChain;
import com.yc.orderforge.v2ocp.DiscountStrategy;
import com.yc.orderforge.v2ocp.DiscountContext;
import com.yc.orderforge.v2ocp.StrategyPricingService;

import java.nio.file.Files;
import java.nio.file.Path;

/**
 * 终局验收（会话 7）：度量前后对比 + 三改实验。
 *
 * 对应 08 篇「重构效果评估」：定量（圈复杂度/行数/坏味道数）+
 * 定性（加新需求改几处）——重构好不好，数字说话。
 *
 * 运行：./run.sh report
 */
public class FinalReport {

    public static void main(String[] args) throws Exception {
        System.out.println("================ 终局验收报告：OrderForge 重构效果 ================\n");

        // ---- 第一部分：度量前后对比 ----
        System.out.println("【一】度量前后对比（08 篇：重构效果定量评估）\n");

        String v0Src = Files.readString(Path.of(
            "src/com/yc/orderforge/v0god/OrderManager.java"));
        String facadeSrc = Files.readString(Path.of(
            "src/com/yc/orderforge/v4diplod/OrderFacade.java"));
        String notifySrc = Files.readString(Path.of(
            "src/com/yc/orderforge/v4diplod/NotificationService.java"));

        int v0MaxCc = maxCc(v0Src);
        int v0Smells = CodeMetrics.smellScan("OrderManager", v0Src).size();
        int finalSmells = CodeMetrics.smellScan("OrderFacade", facadeSrc).size()
                        + CodeMetrics.smellScan("NotificationService", notifySrc).size();

        String[] rows = {
            "最大方法圈复杂度 | " + v0MaxCc + "（placeOrder，>=10 必须重构） | 2（门面编排）",
            "单个类最大行数   | " + CodeMetrics.loc(v0Src) + "（去注释后仍一坨）     | ~40（每个服务一个房间）",
            "坏味道命中数     | " + v0Smells + " 条                | " + finalSmells + " 条（核心编排类）",
            "全局可变状态     | 4 个 static 字段         | 0 个（依赖全注入）",
            "真实 bug         | 3 个（多收125/库存消失/NPE）| 0 个（29 个单测守卫）",
        };
        System.out.println(String.format("%-18s|%-28s|%-24s", "指标", "V0（手术前）", "终局（手术后）"));
        System.out.println("-".repeat(74));
        for (String row : rows) {
            String[] parts = row.split("\\|");
            System.out.println(String.format("%-18s| %-27s| %-23s",
                parts[0].trim(), parts[1].trim(), parts[2].trim()));
        }
        System.out.println();

        // ---- 第二部分：三改实验 ----
        System.out.println("【二】三改实验：加新需求到底要改几处？（OCP 健康信号）\n");

        System.out.println("实验 1：上线『节日直减 20 元』");
        DiscountChain chain = DiscountChain.defaults()
            .register(new DiscountStrategy() {
                @Override public String name() { return "节日直减"; }
                @Override public Money apply(Money cur, DiscountContext ctx) {
                    Money after = cur.minus(Money.ofYuan(20));
                    return after.isNegative() ? Money.ZERO : after;
                }
            });
        Money priced = new StrategyPricingService(chain).price(goldOrder());
        System.out.println("  结果: 新折扣已生效，GOLD 1000 元订单实付 = " + priced);
        System.out.println("  改动: 新增 1 个策略类 + 组装根 1 行 register；"
            + "StrategyPricingService / DiscountChain 0 行修改\n");

        System.out.println("实验 2：通知切换邮件渠道 + 仓储落盘");
        com.yc.orderforge.v4diplod.OrderFacade f = new com.yc.orderforge.v4diplod.OrderFacade(
            new com.yc.orderforge.v1srp.InventoryService(),
            new StrategyPricingService(),
            new com.yc.orderforge.v1srp.ShippingService(),
            new com.yc.orderforge.v3lspisp.PaymentRouter(),
            new com.yc.orderforge.v3lspisp.BalancePayment(Money.ofYuan(10000)),
            new com.yc.orderforge.v4diplod.NotificationService(
                new com.yc.orderforge.v4diplod.EmailSender()),
            new com.yc.orderforge.v4diplod.FileOrderRepository());
        f.placeOrder(normalOrder());
        System.out.println("  改动: 组装根换 2 个构造参数；OrderFacade / NotificationService 0 行修改\n");

        System.out.println("实验 3：全工程获取收货城市");
        System.out.println("  V0 写法: order.getCustomer().getAddress().getCity().getName()"
            + "（抄 N 处 = 改 N 处的地雷阵）");
        System.out.println("  终局写法: order.getShippingCityName()"
            + "（1 个方法，空地址自带兜底）");
        System.out.println();

        // ---- 第三部分：结论 ----
        System.out.println("【三】结论");
        System.out.println("  08 篇：'加功能时手在哪里？'——");
        System.out.println("  V0：每次都回 placeOrder 里改 if（修改中心代码，违反 OCP）");
        System.out.println("  终局：加折扣=加策略类；换渠道/换仓储=改组装根一行（新增文件，遵守 OCP）");
        System.out.println("  这就是六大原则协作的最终形态：SRP 拆好 → OCP 扩展 → LSP 守契约 →");
        System.out.println("  ISP 按角色给能力 → DIP 依赖抽象 → LOD 只认直接朋友。");
    }

    private static int maxCc(String source) {
        int max = 0;
        for (CodeMetrics.MethodInfo m : CodeMetrics.methods(source)) {
            max = Math.max(max, m.cyclomatic);
        }
        return max;
    }

    /** GOLD 会员 1000 元订单（贯穿全部版本的对照样本） */
    private static Order goldOrder() {
        return new Order("R-GOLD",
            new Customer("李四", "138****0002", new Address(new City("上海"))),
            true, false, null, 0)
            .addLine("SKU-A", Money.ofYuan(600), 1)
            .addLine("SKU-B", Money.ofYuan(400), 1);
    }

    private static Order normalOrder() {
        return new Order("R-001",
            new Customer("张三", "138****0001", new Address(new City("北京"))),
            false, false, null, 0)
            .addLine("SKU-A", Money.ofYuan(299), 1)
            .addLine("SKU-B", Money.ofYuan(100), 1);
    }
}
