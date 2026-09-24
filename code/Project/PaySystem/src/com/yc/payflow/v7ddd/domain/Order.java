package com.yc.payflow.v7ddd.domain;

import com.yc.payflow.shared.Money;
import com.yc.payflow.v7ddd.event.EventBus;
import com.yc.payflow.v7ddd.event.OrderSettled;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * ============================================================
 *  Order 聚合根（对应博客第 11 篇「战术四件套 · 聚合根」）
 *  ------------------------------------------------------------
 *  【一致性边界】：
 *    Order 内部：orderLines / status / finalAmount 三者必须一致
 *    → 只能通过聚合根的意图方法修改
 *    → 外部不能直接改 lines 或 status
 *
 *  【为什么 Order 是聚合根而不是 OrderLine】：
 *    OrderLine 的存在必须依附 Order（订单删了行也没了）
 *    Order 的状态转换（草稿→已结算→已发货）跨行有效
 *    ↑ 一致性边界就画在 Order 上
 *
 *  【状态机】：DRAFT → SETTLED → SHIPPED / CANCELLED
 *    非法转换直接抛异常（不变量守卫）
 *
 *  【领域事件】：settle() 成功后发布 OrderSettled 事件
 *    ↑ 结算方法不关心谁订阅、订阅方做什么——完全解耦
 * ============================================================
 */
public class Order {

    public enum Status { DRAFT, SETTLED, SHIPPED, CANCELLED }

    private final OrderId id;
    private final String buyerId;
    private final String contactEmail;
    private final String contactPhone;
    private final List<OrderLine> lines = new ArrayList<>();
    private Status status = Status.DRAFT;
    private Money finalAmount;

    public Order(OrderId id, String buyerId, String contactEmail, String contactPhone) {
        this.id = id;
        this.buyerId = buyerId;
        this.contactEmail = contactEmail;
        this.contactPhone = contactPhone;
    }

    public void addLine(OrderLine line) {
        if (status != Status.DRAFT) {
            throw new IllegalStateException("只有草稿订单才能加行: 当前状态=" + status);
        }
        lines.add(line);
    }

    /**
     * 结算——聚合根的核心意图方法。
     *
     * 【不变量】：
     *   - 只有 DRAFT 状态才能结算
     *   - 至少 1 个订单行
     *   - finalAmount 必须为正
     *
     * 【领域事件】：结算成功后向 EventBus 发布 OrderSettled
     */
    public void settle(Money finalAmount, EventBus bus) {
        if (status != Status.DRAFT) {
            throw new IllegalStateException("订单已经结算或取消: " + status);
        }
        if (lines.isEmpty()) {
            throw new IllegalStateException("空订单不能结算");
        }
        if (finalAmount == null || !finalAmount.isPositive()) {
            throw new IllegalArgumentException("结算金额必须为正: " + finalAmount);
        }
        this.finalAmount = finalAmount;
        this.status = Status.SETTLED;

        // 发布领域事件（订阅方按需处理，聚合根不关心谁在监听）
        bus.publish(new OrderSettled(
            id.value(), buyerId, finalAmount, contactEmail, contactPhone));
    }

    public void cancel() {
        if (status == Status.SHIPPED) {
            throw new IllegalStateException("已发货订单不可取消");
        }
        this.status = Status.CANCELLED;
    }

    public OrderId id() { return id; }
    public Status status() { return status; }
    public Money finalAmount() { return finalAmount; }
    public List<OrderLine> lines() { return Collections.unmodifiableList(lines); }
    public String buyerId() { return buyerId; }

    public Money subtotal() {
        return lines.stream()
            .map(OrderLine::subtotal)
            .reduce(Money.ZERO, Money::plus);
    }
}
