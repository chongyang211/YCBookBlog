package com.yc.orderforge.shared.testkit;

/** 测试体函数式接口（替代 JUnit 的 Executable）。 */
@FunctionalInterface
public interface Executable {
    void execute() throws Throwable;
}
