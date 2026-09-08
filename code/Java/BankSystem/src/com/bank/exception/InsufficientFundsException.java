package com.bank.exception;

/**
 * 余额不足异常，带申请金额与可用金额。
 */
public class InsufficientFundsException extends BankException {
    private final double requested;
    private final double available;

    public InsufficientFundsException(double requested, double available) {
        super(String.format("余额不足：申请 %.2f，可用 %.2f", requested, available));
        this.requested = requested;
        this.available = available;
    }

    public double getRequested() { return requested; }
    public double getAvailable() { return available; }
}
