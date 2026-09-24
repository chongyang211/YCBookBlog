package com.yc.payflow.v1feature;

/**
 * 余额不足异常（对应博客第 02 篇「封装」的不变量守卫）。
 *
 * 【为什么用异常而不是返回 boolean】
 *   - 语义显式：调用方必须处理，编译器强制（如果是 checked）或阅读者显眼
 *   - 阻断错误后续操作：扣款失败还继续走下单流程是灾难
 *   - 携带上下文：可以捎带 balance / required 用于日志与告警
 */
public class InsufficientFundsException extends RuntimeException {
    public InsufficientFundsException(String message) {
        super(message);
    }
}
