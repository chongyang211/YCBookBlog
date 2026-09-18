package com.yc.orderforge.v1srp;

import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;

/**
 * 订单协调者（V1 拆出）：只做编排，不做任何具体的事。
 *
 * 对照 02 篇「详情页再回顾」：
 *   DetailController 只剩"协调几位朋友 + 把结果扔给 UI"，2000 行缩到 300 行。
 *   这里 OrderService 只剩"六步编排 + 失败补偿"，placeOrder 圈复杂度从 16 降到 4。
 *
 * ⚠️ 留给 V4 的坏味道：构造器里 new 全部依赖（DIP 违反）——
 *    V1 先保证职责单一，"谁来提供依赖"是 DIP 手术的活。
 */
public class OrderService {

    private final InventoryService inventory = new InventoryService();
    private final PricingService pricing = new PricingService();
    private final PaymentService payment = new PaymentService();
    private final ShippingService shipping = new ShippingService();
    private final NotificationService notification = new NotificationService();
    private final ReconciliationService reconciliation = new ReconciliationService();

    public String placeOrder(Order order) {
        if (order == null || order.getLines().isEmpty()) {
            throw new IllegalArgumentException("订单不能为空");
        }
        if (!inventory.tryDeduct(order)) {
            throw new IllegalStateException("库存不足");
        }

        Money total;
        try {
            total = pricing.price(order);
            payment.charge(order.getPayType(), total);
        } catch (RuntimeException e) {
            inventory.restock(order);        // ← V1 核心修复：失败补偿，库存不再凭空消失
            throw e;
        }

        total = total.plus(shipping.fee(order));
        notification.notifyPaid(order, total);   // BUG 3 仍在（V4 修）
        reconciliation.record(order.getOrderNo(), total);
        return order.getOrderNo();
    }

    // ---- 只读访问器（供测试与验收） ----
    public InventoryService inventory()         { return inventory; }
    public PricingService pricing()              { return pricing; }
    public PaymentService payment()              { return payment; }
    public ShippingService shipping()            { return shipping; }
    public NotificationService notification()    { return notification; }
    public ReconciliationService reconciliation(){ return reconciliation; }
}
