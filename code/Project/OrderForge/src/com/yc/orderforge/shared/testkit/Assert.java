package com.yc.orderforge.shared.testkit;

import java.util.Objects;

/** 极简断言工具（教学场景零依赖，API 对齐 JUnit 风格）。 */
public final class Assert {
    private Assert() {}

    public static void equals(Object expected, Object actual, String hint) {
        if (!Objects.equals(expected, actual)) {
            throw new AssertionError(
                "断言失败：" + hint + " | 期望=" + expected + " | 实际=" + actual);
        }
    }

    public static void trueThat(boolean condition, String hint) {
        if (!condition) throw new AssertionError("断言失败：" + hint);
    }

    public static void falseThat(boolean condition, String hint) {
        if (condition) throw new AssertionError("断言失败：" + hint);
    }

    public static <E extends Throwable> E throwsException(Class<E> expected, Executable action) {
        try {
            action.execute();
        } catch (Throwable t) {
            if (expected.isInstance(t)) return expected.cast(t);
            throw new AssertionError("断言失败：期望抛 " + expected.getSimpleName()
                + "，实际抛 " + t.getClass().getSimpleName() + ": " + t.getMessage());
        }
        throw new AssertionError("断言失败：期望抛 " + expected.getSimpleName() + "，但未抛出任何异常");
    }
}
