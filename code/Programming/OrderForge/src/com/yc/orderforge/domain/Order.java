package com.yc.orderforge.domain;

import com.yc.orderforge.shared.Money;

import java.util.ArrayList;
import java.util.List;

/**
 * 订单实体（跨版本共享的领域模型）。
 *
 * payType：0=余额支付，1=信用卡支付。
 * coupon：优惠券面值（可空表示无券）。
 */
public class Order {
    private final String orderNo;
    private final Customer customer;
    private final boolean vip;
    private final boolean newUser;
    private final Money coupon;
    private final int payType;
    private final List<OrderLine> lines = new ArrayList<>();

    public Order(String orderNo, Customer customer, boolean vip,
                 boolean newUser, Money coupon, int payType) {
        this.orderNo = orderNo;
        this.customer = customer;
        this.vip = vip;
        this.newUser = newUser;
        this.coupon = coupon;
        this.payType = payType;
    }

    public Order addLine(String sku, Money unitPrice, int qty) {
        lines.add(new OrderLine(sku, unitPrice, qty));
        return this;
    }

    /** 商品小计（未叠加任何优惠） */
    public Money subtotal() {
        Money t = Money.ZERO;
        for (OrderLine l : lines) t = t.plus(l.subtotal());
        return t;
    }

    public int totalQty() {
        int q = 0;
        for (OrderLine l : lines) q += l.qty;
        return q;
    }

    /**
     * 【V4 LOD 手术新增】直接朋友暴露业务语义方法，替代火车残骸：
     *   旧：order.getCustomer().getAddress().getCity().getName()  ← 穿透 4 层 + NPE 风险
     *   新：order.getShippingCityName()                             ← 一层 + 空值兜底
     */
    public String getShippingCityName() {
        if (customer == null || customer.getAddress() == null
                || customer.getAddress().getCity() == null) {
            return "未知城市";
        }
        return customer.getAddress().getCity().getName();
    }

    public String getOrderNo()   { return orderNo; }
    public Customer getCustomer(){ return customer; }
    public boolean isVip()       { return vip; }
    public boolean isNewUser()   { return newUser; }
    public Money getCoupon()     { return coupon; }
    public int getPayType()      { return payType; }
    public List<OrderLine> getLines() { return lines; }

    /** 订单行（值对象） */
    public static final class OrderLine {
        private final String sku;
        private final Money unitPrice;
        private final int qty;
        OrderLine(String sku, Money unitPrice, int qty) {
            this.sku = sku; this.unitPrice = unitPrice; this.qty = qty;
        }
        public Money subtotal() { return unitPrice.multiply(qty); }
        public int getQty()     { return qty; }
        public String getSku()  { return sku; }
    }
}
