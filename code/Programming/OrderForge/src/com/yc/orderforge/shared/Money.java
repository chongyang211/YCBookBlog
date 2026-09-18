package com.yc.orderforge.shared;

import java.util.Objects;

/**
 * Money 值对象：以"分"为单位存储，避免 double 精度陷阱（0.1 + 0.2 != 0.3）。
 * 不可变 + 相等基于内容 + 业务语义方法。
 */
public final class Money {
    public static final Money ZERO = new Money(0);

    private final long cents;

    private Money(long cents) { this.cents = cents; }

    public static Money ofYuan(double yuan) { return new Money(Math.round(yuan * 100)); }
    public static Money ofCents(long cents) { return new Money(cents); }

    public Money plus(Money o)      { return new Money(cents + o.cents); }
    public Money minus(Money o)     { return new Money(cents - o.cents); }
    public Money multiply(double f) { return new Money(Math.round(cents * f)); }
    public Money percentOff(double percent) { return multiply(1.0 - percent / 100.0); }
    public Money min(Money o)       { return cents <= o.cents ? this : o; }

    public boolean isPositive()            { return cents > 0; }
    public boolean isNegative()            { return cents < 0; }
    public boolean greaterThan(Money o)    { return cents > o.cents; }
    public boolean greaterOrEqual(Money o) { return cents >= o.cents; }

    public long cents()    { return cents; }
    public double toYuan() { return cents / 100.0; }

    @Override public boolean equals(Object o) {
        return o instanceof Money && ((Money) o).cents == cents;
    }
    @Override public int hashCode() { return Objects.hash(cents); }
    @Override public String toString() { return String.format("¥%.2f", cents / 100.0); }
}
