package com.yc.payflow.v3composition;

import com.yc.payflow.v3composition.impl.*;

/**
 * 一堆预置的商品配方——展示"组合装配"如何按能力自由拼装。
 *
 * 注意五种商品各自持有的能力集合是完全不同的：
 *   实体书    : 配送 + 可退款
 *   生鲜牛奶  : 配送 + 冷链
 *   跨境保温杯: 配送 + 跨境 + 可退款
 *   虚拟课程  : 只有可退款（无 ship！不再被迫抛 UOE）
 *   进口疫苗  : 配送 + 冷链 + 跨境（三合一）
 */
public final class ProductCatalog {
    private ProductCatalog() {}

    /** 实体书：可配送 + 7 天可退 */
    public static Product physicalBook() {
        return Product.builder("SKU-BOOK-01", "《面向对象设计》纸质版", 6800)
            .withShipping(new StandardShipping())
            .withRefundable(new StandardRefundable(7))
            .build();
    }

    /** 生鲜牛奶：配送 + 冷链（因是生鲜所以不可退款） */
    public static Product freshMilk() {
        return Product.builder("SKU-MILK-01", "生鲜牛奶 1L", 1590)
            .withShipping(new StandardShipping())
            .withColdChain(new ColdChainSurcharge())
            .build();
    }

    /** 跨境保温杯：配送 + 跨境税 + 15 天可退 */
    public static Product crossBorderCup() {
        return Product.builder("SKU-CUP-01", "跨境保温杯 500ml", 12900)
            .withShipping(new StandardShipping())
            .withCrossBorder(new CrossBorderTax())
            .withRefundable(new StandardRefundable(15))
            .build();
    }

    /**
     * 虚拟课程：只有可退款能力，没有 shipping/coldChain/crossBorder
     * ↑ 关键：不会被迫实现无意义的 ship() 或 tax()。
     */
    public static Product virtualCourse() {
        return Product.builder("SKU-COURSE-01", "《OOP 12 篇》视频课程", 29900)
            .withRefundable(new StandardRefundable(7))
            .build();
    }

    /** 进口疫苗：配送 + 冷链 + 跨境（三种能力同时具备） */
    public static Product importedVaccine() {
        return Product.builder("SKU-VAX-01", "进口疫苗", 39900)
            .withShipping(new StandardShipping())
            .withColdChain(new ColdChainSurcharge())
            .withCrossBorder(new CrossBorderTax())
            .build();
    }
}
