package com.bank.exception;

/**
 * 金额非法异常，带金额。
 */
public class InvalidAmountException extends BankException {
    private final double amount;

    public InvalidAmountException(double amount) {
        super(String.format("金额非法: %.2f（必须 > 0）", amount));
        this.amount = amount;
    }

    public double getAmount() { return amount; }
}
