package com.yc.payflow.shared.testkit;

/** 可抛异常的可执行函数式接口（用于断言"应抛异常"的 lambda）。 */
@FunctionalInterface
public interface Executable {
    void execute() throws Throwable;
}
