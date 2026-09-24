package com.yc.payflow.v3composition;

import com.yc.payflow.shared.testkit.Assert;
import com.yc.payflow.shared.testkit.TestRunner;

/**
 * ProductTypeTest：验证能力组合的核心特征——
 *   - 虚拟商品无 ship 能力（不用抛 UOE）
 *   - 各能力可自由拼装、互不干扰
 */
public class ProductTypeTest {

    public static void main(String[] args) {
        new TestRunner("v3.ProductComposition")
            .test("实体书具备配送+可退能力", () -> {
                Product p = ProductCatalog.physicalBook();
                Assert.trueThat(p.shipping().isPresent(), "实体书应可配送");
                Assert.trueThat(p.refundable().isPresent(), "实体书应可退款");
                Assert.falseThat(p.coldChain().isPresent(), "实体书无需冷链");
                Assert.falseThat(p.crossBorder().isPresent(), "实体书非跨境");
            })
            .test("虚拟课程无配送能力（企鹅会飞的反面）", () -> {
                Product p = ProductCatalog.virtualCourse();
                Assert.falseThat(p.shipping().isPresent(), "虚拟课程不应有 ship 能力");
                Assert.falseThat(p.coldChain().isPresent(), "虚拟课程无需冷链");
                Assert.trueThat(p.refundable().isPresent(), "虚拟课程应可退款");
            })
            .test("进口疫苗三能力叠加（配送+冷链+跨境）", () -> {
                Product p = ProductCatalog.importedVaccine();
                Assert.trueThat(p.shipping().isPresent(), "疫苗应可配送");
                Assert.trueThat(p.coldChain().isPresent(), "疫苗应需冷链");
                Assert.trueThat(p.crossBorder().isPresent(), "疫苗应征跨境税");
            })
            .test("组合装配可扩展：新增'数字商品+跨境'零改动 Product 类", () -> {
                // 直接用 builder 拼装一个从没定义过的商品类型
                Product custom = Product.builder("SKU-CUSTOM", "自定义能力商品", 10000)
                    .withCrossBorder(() -> 0.05)
                    .withRefundable(() -> 30)
                    .build();
                Assert.equals(0.05, custom.crossBorder().get().taxRate(), "税率");
                Assert.equals(30, custom.refundable().get().refundWindowDays(), "退款窗");
            })
            .test("冷链附加费固定 8 元", () -> {
                Product p = ProductCatalog.freshMilk();
                Assert.equals(800L, p.coldChain().get().coldChainSurchargeCents(), "冷链费 8 元");
            })
            .test("满 99 包邮", () -> {
                Product p = ProductCatalog.physicalBook();
                long fee = p.shipping().get().shippingFeeCents("BJ", 15000);
                Assert.equals(0L, fee, "150 元订单应包邮");
                long fee2 = p.shipping().get().shippingFeeCents("BJ", 5000);
                Assert.equals(1200L, fee2, "50 元订单收 12 元运费");
            })
            .run();
    }
}
