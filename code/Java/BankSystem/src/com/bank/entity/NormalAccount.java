package com.bank.entity;

import com.bank.exception.InsufficientFundsException;
import com.bank.exception.InvalidAmountException;
import com.bank.persist.Persistable;

/**
 * 普通账户：不允许透支，月利率 0.1%。
 */
public class NormalAccount extends Account implements Persistable {
    private static final double MONTHLY_RATE = 0.001;   // 0.1%

    public NormalAccount(String id, String ownerName, double initialBalance) {
        super(id, ownerName, initialBalance);
    }

    @Override
    public void withdraw(double amount) {
        if (amount <= 0) throw new InvalidAmountException(amount);
        if (getBalance() < amount) {
            throw new InsufficientFundsException(amount, getBalance());
        }
        setBalanceInternal(getBalance() - amount);
    }

    @Override
    public double calcInterest() {
        return getBalance() * MONTHLY_RATE;
    }

    @Override
    public char typeTag() { return 'N'; }

    @Override
    public String toCsv() {
        return String.join(",", String.valueOf(typeTag()), getId(), getOwnerName(),
                String.valueOf(getBalance()));
    }
}
