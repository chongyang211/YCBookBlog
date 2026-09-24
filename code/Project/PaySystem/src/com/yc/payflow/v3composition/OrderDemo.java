package com.yc.payflow.v3composition;

/**
 * V3 Demo：展示"能力组合"如何按需拼装。
 *
 * 输出重点：
 *   ① 五种商品各自的能力清单
 *   ② 虚拟课程无 ship 能力（不再被迫抛 UOE，问题在编译期消失）
 *   ③ 新增"疫苗"这种"配送+冷链+跨境"三合一商品，无需继承任何父类
 */
public class OrderDemo {
    public static void main(String[] args) {
        System.out.println("================ V3 组合优于继承 Demo ================");

        Product[] catalog = new Product[]{
            ProductCatalog.physicalBook(),
            ProductCatalog.freshMilk(),
            ProductCatalog.crossBorderCup(),
            ProductCatalog.virtualCourse(),
            ProductCatalog.importedVaccine()
        };

        for (Product p : catalog) {
            System.out.println("[" + p.name() + "] 价格 " + fmt(p.priceCents()) + " 能力:");
            System.out.println("  - 可配送:   " + p.shipping().isPresent());
            System.out.println("  - 需冷链:   " + p.coldChain().isPresent());
            System.out.println("  - 跨境税:   " + p.crossBorder().isPresent());
            System.out.println("  - 可退款窗: "
                + p.refundable().map(r -> r.refundWindowDays() + " 天").orElse("不可退"));
        }

        System.out.println();
        System.out.println(">>> 关键观察：");
        System.out.println("    - 虚拟课程连 shipping 字段都没有，编译期就不可能调 ship()");
        System.out.println("    - 进口疫苗 = 配送+冷链+跨境三合一，只需组合，无需继承 5 个父类");
    }

    private static String fmt(long cents) {
        return String.format("¥%.2f", cents / 100.0);
    }
}
