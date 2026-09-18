package com.yc.payflow.shared;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.Objects;

/**
 * Money 值对象（跨版本共享）。
 *
 * 设计要点（对应博客第 11 篇「战术四件套 · 值对象」）：
 *   - 不可变（final 字段 + 无 setter）
 *   - 相等基于内容（equals/hashCode 基于 cents）
 *   - 以分为单位存储，避免 double 精度陷阱（0.1 + 0.2 != 0.3）
 *   - 提供业务意义的运算（plus/minus/multiply/percentOff）
 *
 * 从 V1 起就引入 Money，V0 保留 double 以保留双 11 事故现场的"精度陷阱"。
 */
public final class Money implements Comparable<Money> {
    public static final Money ZERO = new Money(0);

    private final long cents; // 以"分"为最小单位

    private Money(long cents) {
        this.cents = cents;
    }

    public static Money ofYuan(double yuan) {
        // 四舍五入到分
        return new Money(Math.round(yuan * 100));
    }

    public static Money ofCents(long cents) {
        return new Money(cents);
    }

    public Money plus(Money other) {
        return new Money(this.cents + other.cents);
    }

    public Money minus(Money other) {
        return new Money(this.cents - other.cents);
    }

    public Money multiply(double factor) {
        return new Money(Math.round(this.cents * factor));
    }

    /** 按比例打折。例如 percentOff(15) 表示打 85 折。 */
    public Money percentOff(double percent) {
        return this.multiply(1.0 - percent / 100.0);
    }

    /** 取两者较大。用于最低消费兜底。 */
    public Money max(Money other) {
        return this.cents >= other.cents ? this : other;
    }

    /** 取两者较小。用于优惠抵扣兜底（抵扣不能超过金额）。 */
    public Money min(Money other) {
        return this.cents <= other.cents ? this : other;
    }

    public boolean isNegative() {
        return cents < 0;
    }

    public boolean isPositive() {
        return cents > 0;
    }

    public boolean isZero() {
        return cents == 0;
    }

    public boolean greaterThan(Money other) {
        return this.cents > other.cents;
    }

    public boolean greaterOrEqual(Money other) {
        return this.cents >= other.cents;
    }

    public long cents() {
        return cents;
    }

    public double toYuan() {
        return cents / 100.0;
    }

    public BigDecimal toBigDecimal() {
        return BigDecimal.valueOf(cents).divide(BigDecimal.valueOf(100), 2, RoundingMode.HALF_UP);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof Money)) return false;
        return cents == ((Money) o).cents;
    }

    @Override
    public int hashCode() {
        return Objects.hash(cents);
    }

    @Override
    public int compareTo(Money o) {
        return Long.compare(this.cents, o.cents);
    }

    @Override
    public String toString() {
        return String.format("¥%.2f", cents / 100.0);
    }
}
