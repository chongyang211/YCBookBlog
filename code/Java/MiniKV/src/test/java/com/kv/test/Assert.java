package com.kv.test;

import java.util.Objects;

/**
 * 自写断言工具（阶段⑪）—— 不用 Java assert 关键字（JVM 默认禁用，需 -ea 开启），
 * 失败统一抛 AssertionError（与 JUnit 同名）。
 */
public final class Assert {
    private Assert() {}

    public static void isTrue(boolean cond, String msg) {
        if (!cond) throw new AssertionError(msg);
    }

    public static void equals(Object expected, Object actual) {
        if (!Objects.equals(expected, actual)) {
            throw new AssertionError("Expected: <" + expected + "> but got: <" + actual + ">");
        }
    }

    public static void notNull(Object o, String msg) {
        if (o == null) throw new AssertionError(msg);
    }

    /** 断言某段代码抛出指定类型的异常，返回该异常（可继续断言 message） */
    public static <T extends Throwable> T throwsException(Class<T> expectedType, Runnable code) {
        try {
            code.run();
        } catch (Throwable t) {
            if (expectedType.isInstance(t)) return expectedType.cast(t);
            throw new AssertionError(
                    "Expected " + expectedType.getSimpleName() + " but got " + t.getClass().getSimpleName());
        }
        throw new AssertionError("Expected " + expectedType.getSimpleName() + " but no exception thrown");
    }
}
