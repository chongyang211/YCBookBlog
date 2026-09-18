package com.yc.payflow.v3composition;

import com.yc.payflow.v3composition.ability.*;

import java.util.Optional;

/**
 * ============================================================
 *  Product 商品（能力字段组合，而非继承体系）
 *  ------------------------------------------------------------
 *  对应博客第 05 篇「组合优于继承」的"能力接口 + 字段持有"三步：
 *    ① 能力抽成接口（ability/*）
 *    ② 每种能力独立实现（impl/*）
 *    ③ 用 Builder 把能力装配进 Product
 *
 *  【极端对比】：
 *    继承派会写：
 *      class PhysicalProduct    { ship() }
 *      class ColdChainProduct extends PhysicalProduct { coldChain() }
 *      class CrossBorderProduct extends PhysicalProduct { tax() }
 *      class ColdChainCrossBorderProduct extends ??? { ... }
 *      —— 笛卡尔积爆炸，5 个能力就要 32 个子类
 *
 *    组合派：Product 持有可选的能力字段，"要什么装什么"，
 *           5 个能力 = 5 个类 + 1 个 Product。
 *
 *  【新增"虚拟课程"商品】：只需要 Refundable（7 天无理由）+ 不持有 Shippable
 *  ——绝不会被迫实现无意义的 ship()（企鹅会飞的反面）。
 * ============================================================
 */
public class Product {
    private final String sku;
    private final String name;
    private final long priceCents;

    // 能力字段：可空表示"不具备该能力"（比继承+null 对象模式更清晰）
    private final Shippable shipping;
    private final ColdChainable coldChain;
    private final CrossBorderable crossBorder;
    private final Refundable refundable;

    private Product(Builder b) {
        this.sku = b.sku;
        this.name = b.name;
        this.priceCents = b.priceCents;
        this.shipping = b.shipping;
        this.coldChain = b.coldChain;
        this.crossBorder = b.crossBorder;
        this.refundable = b.refundable;
    }

    public String sku() { return sku; }
    public String name() { return name; }
    public long priceCents() { return priceCents; }

    /** 能力查询：Optional 让"是否具备该能力"变成显式意图 */
    public Optional<Shippable> shipping()      { return Optional.ofNullable(shipping); }
    public Optional<ColdChainable> coldChain() { return Optional.ofNullable(coldChain); }
    public Optional<CrossBorderable> crossBorder() { return Optional.ofNullable(crossBorder); }
    public Optional<Refundable> refundable()   { return Optional.ofNullable(refundable); }

    public static Builder builder(String sku, String name, long priceCents) {
        return new Builder(sku, name, priceCents);
    }

    public static class Builder {
        private final String sku;
        private final String name;
        private final long priceCents;
        private Shippable shipping;
        private ColdChainable coldChain;
        private CrossBorderable crossBorder;
        private Refundable refundable;

        private Builder(String sku, String name, long priceCents) {
            this.sku = sku;
            this.name = name;
            this.priceCents = priceCents;
        }

        public Builder withShipping(Shippable s)          { this.shipping = s; return this; }
        public Builder withColdChain(ColdChainable c)     { this.coldChain = c; return this; }
        public Builder withCrossBorder(CrossBorderable cb){ this.crossBorder = cb; return this; }
        public Builder withRefundable(Refundable r)       { this.refundable = r; return this; }

        public Product build() { return new Product(this); }
    }
}
