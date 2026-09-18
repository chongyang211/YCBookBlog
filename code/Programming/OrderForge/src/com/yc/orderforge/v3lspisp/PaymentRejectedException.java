package com.yc.orderforge.v3lspisp;

/**
 * 统一支付拒绝异常（支付契约的一部分）。
 *
 * LSP 第 3 条规则：子类不能抛出父类未声明的异常。
 * 契约规定：任何支付方式的失败，都以【本类型】抛出——
 * 调用方只需要 catch 一种异常，不会"换个渠道就换种崩法"。
 */
public class PaymentRejectedException extends RuntimeException {
    public PaymentRejectedException(String message) {
        super(message);
    }
}
