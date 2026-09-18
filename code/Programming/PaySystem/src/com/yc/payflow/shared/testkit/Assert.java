package com.yc.payflow.shared.testkit;

import java.util.Objects;

/**
 * 极简断言工具（替代 JUnit，教学场景零依赖）。
 *
 * 用法：
 *   Assert.equals(expected, actual, "余额应扣减 100");
 *   Assert.throwsException(IllegalStateException.class, () -> wallet.withdraw(...));
 *   Assert.trueThat(money.isPositive(), "金额必须为正");
 */
public final class Assert {
    private Assert() {}

    public static void equals(Object expected, Object actual, String hint) {
        if (!Objects.equals(expected, actual)) {
            throw new AssertionError(
                "断言失败：" + hint + " | 期望=" + expected + " | 实际=" + actual);
        }
    }

    public static void notEquals(Object unexpected, Object actual, String hint) {
        if (Objects.equals(unexpected, actual)) {
            throw new AssertionError(
                "断言失败：" + hint + " | 值不应为 " + unexpected);
        }
    }

    public static void trueThat(boolean condition, String hint) {
        if (!condition) {
            throw new AssertionError("断言失败：" + hint);
        }
    }

    public static void falseThat(boolean condition, String hint) {
        if (condition) {
            throw new AssertionError("断言失败：" + hint);
        }
    }

    public static void notNull(Object obj, String hint) {
        if (obj == null) {
            throw new AssertionError("断言失败：" + hint + " | 对象为 null");
        }
    }

    public static <E extends Throwable> E throwsException(Class<E> expected, Executable action) {
        try {
            action.execute();
        } catch (Throwable t) {
            if (expected.isInstance(t)) {
                return expected.cast(t);
            }
            throw new AssertionError(
                "断言失败：期望抛 " + expected.getSimpleName()
                + " 实际抛 " + t.getClass().getSimpleName() + " : " + t.getMessage());
        }
        throw new AssertionError(
            "断言失败：期望抛 " + expected.getSimpleName() + "，但未抛出任何异常");
    }
}
