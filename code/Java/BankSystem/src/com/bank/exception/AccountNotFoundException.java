package com.bank.exception;

/**
 * 账户不存在异常，带账号。
 */
public class AccountNotFoundException extends BankException {
    private final String accountId;

    public AccountNotFoundException(String accountId) {
        super("账户不存在: " + accountId);
        this.accountId = accountId;
    }

    public String getAccountId() { return accountId; }
}
