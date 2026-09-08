package com.bank.exception;

/**
 * 业务异常基类（继承 RuntimeException，非受检）。
 */
public class BankException extends RuntimeException {
    public BankException(String message) {
        super(message);
    }

    public BankException(String message, Throwable cause) {
        super(message, cause);
    }
}
