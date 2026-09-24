package com.yc.orderforge.v4diplod;

import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;
import com.yc.orderforge.v1srp.InventoryService;
import com.yc.orderforge.v1srp.ShippingService;
import com.yc.orderforge.v2ocp.StrategyPricingService;
import com.yc.orderforge.v3lspisp.PaymentRouter;

/**
 * 订单门面（07 篇 OrderFacade 原案的落地 + 全部手术的装配现场）。
 *
 * 【LOD】客户端（页面/接口层）只认识 OrderFacade 一个"朋友"，
 *        库存/计价/支付/通知/仓储全是门面的私人朋友。
 *
 * 【DIP】五个依赖全部构造注入——门面不 new 任何具体类，
 *        组装根（main）想换渠道/换仓储/换支付，门面零修改。
 *
 * 【手术汇总】此处运行的链条已包含前几版全部修复：
 *        V1 补偿 / V2 策略链(Bug1) / V3 契约路由(Bug2) / V4 注入+LOD(Bug3)
 */
public class OrderFacade {

    private final InventoryService inventory;
    private final StrategyPricingService pricing;
    private final ShippingService shipping;
    private final PaymentRouter paymentRouter;
    private final com.yc.orderforge.v3lspisp.PaymentMethod paymentMethod;
    private final NotificationService notification;
    private final OrderRepository repository;

    public OrderFacade(InventoryService inventory,
                       StrategyPricingService pricing,
                       ShippingService shipping,
                       PaymentRouter paymentRouter,
                       com.yc.orderforge.v3lspisp.PaymentMethod paymentMethod,
                       NotificationService notification,
                       OrderRepository repository) {
        this.inventory = inventory;
        this.pricing = pricing;
        this.shipping = shipping;
        this.paymentRouter = paymentRouter;
        this.paymentMethod = paymentMethod;
        this.notification = notification;
        this.repository = repository;
    }

    /** 客户端唯一入口：一个方法完成履约全链路 */
    public String placeOrder(Order order) {
        if (order == null || order.getLines().isEmpty()) {
            throw new IllegalArgumentException("订单不能为空");
        }
        // ① 计价（V2 策略链，BUG 1 已修）
        Money total = pricing.price(order);
        // ② 运费
        total = total.plus(shipping.fee(order));
        // ③ 支付（V3 契约路由：超限/余额不足在扣库存之前失败，BUG 2 已修）
        String txId = paymentRouter.pay(paymentMethod, total);
        // ④ 扣库存（走到这里支付已成功，才会动库存；失败自动补偿）
        if (!inventory.tryDeduct(order)) {
            throw new IllegalStateException("库存不足（支付已成功，需人工退款 " + total + "）");
        }
        // ⑤ 通知（V4 LOD：getShippingCityName 空地址安全，BUG 3 已修）
        notification.notifyPaid(order, total);
        // ⑥ 仓储（对账）
        repository.save(order.getOrderNo(), total);

        return order.getOrderNo() + " / 流水 " + txId;
    }

    public InventoryService inventory()  { return inventory; }
    public OrderRepository repository()   { return repository; }
}
