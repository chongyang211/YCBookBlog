package com.yc.payflow.v7ddd.domain;

import java.util.Objects;

/**
 * OrderId 值对象——避免"用 String 代表订单号"这种基本类型偏执。
 *
 * 【价值】：
 *   ① 类型系统防串号（不会把 UserId 传给需要 OrderId 的方法）
 *   ② 集中管理格式校验（订单号必须 8-32 位字母数字）
 *   ③ 不可变、equals 基于值
 */
public final class OrderId {
    private final String value;

    public OrderId(String value) {
        if (value == null || value.isBlank()) {
            throw new IllegalArgumentException("OrderId 不能为空");
        }
        if (value.length() < 4 || value.length() > 32) {
            throw new IllegalArgumentException("OrderId 长度应在 4-32 之间: " + value);
        }
        this.value = value;
    }

    public String value() { return value; }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof OrderId)) return false;
        return value.equals(((OrderId) o).value);
    }
    @Override public int hashCode() { return Objects.hash(value); }
    @Override public String toString() { return value; }
}
