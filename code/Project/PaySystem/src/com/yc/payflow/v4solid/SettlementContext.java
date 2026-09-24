package com.yc.payflow.v4solid;

import com.yc.payflow.shared.Money;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * ============================================================
 *  SettlementContext 结算上下文（对应 09 篇「以参数对象取代长参数」）
 *  ------------------------------------------------------------
 *  V0 的 calc() 有 10 个参数、内部读写 total 17 次。
 *  V4 把"每一步的输入 + 累计金额"沉到 Context，每个 PromotionStep
 *  接收 Context、修改 currentTotal，链条清晰。
 *
 *  【设计选择】：为什么 currentTotal 用可变字段而不是 immutable？
 *    - 每步返回新 Money 会导致每步单独 GC 一次（可接受）
 *    - 但更重要：调试时希望能"在断点上看到当前累计值"
 *    - 保留 auditTrail 记录每步扣减明细，便于对账
 * ============================================================
 */
public class SettlementContext {
    private final List<OrderItem> items;
    private final int vipLevel;
    private final String couponId;
    private final int usePoints;
    private final String city;
    private final String orderNo;

    private Money currentTotal = Money.ZERO;
    private Money subtotalSnapshot = Money.ZERO; // 商品小计快照（用于满减判定）
    private final List<String> auditTrail = new ArrayList<>();

    private SettlementContext(Builder b) {
        this.items = List.copyOf(b.items);
        this.vipLevel = b.vipLevel;
        this.couponId = b.couponId;
        this.usePoints = b.usePoints;
        this.city = b.city;
        this.orderNo = b.orderNo;
    }

    public List<OrderItem> items() { return items; }
    public int vipLevel() { return vipLevel; }
    public String couponId() { return couponId; }
    public int usePoints() { return usePoints; }
    public String city() { return city; }
    public String orderNo() { return orderNo; }

    public Money currentTotal() { return currentTotal; }
    public Money subtotalSnapshot() { return subtotalSnapshot; }

    public void setCurrentTotal(Money total) { this.currentTotal = total; }
    public void setSubtotalSnapshot(Money total) { this.subtotalSnapshot = total; }

    public void audit(String stepName, Money delta, Money after) {
        auditTrail.add(String.format("  [%s] Δ=%s → %s", stepName, delta, after));
    }
    public List<String> auditTrail() { return Collections.unmodifiableList(auditTrail); }

    public static Builder builder(String orderNo) {
        return new Builder(orderNo);
    }

    public static class Builder {
        private final String orderNo;
        private List<OrderItem> items = new ArrayList<>();
        private int vipLevel = 0;
        private String couponId;
        private int usePoints;
        private String city = "OTHER";
        Builder(String orderNo) { this.orderNo = orderNo; }
        public Builder addItem(OrderItem it) { items.add(it); return this; }
        public Builder vipLevel(int l) { this.vipLevel = l; return this; }
        public Builder couponId(String c) { this.couponId = c; return this; }
        public Builder usePoints(int p) { this.usePoints = p; return this; }
        public Builder city(String c) { this.city = c; return this; }
        public SettlementContext build() { return new SettlementContext(this); }
    }
}
