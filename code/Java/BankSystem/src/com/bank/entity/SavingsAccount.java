package com.bank.entity;

import com.bank.exception.InsufficientFundsException;
import com.bank.exception.InvalidAmountException;
import com.bank.persist.Persistable;

import java.time.LocalDate;

/**
 * 储蓄账户：锁定期内取款扣 1% 违约金，月利率 0.3%。
 */
public class SavingsAccount extends Account implements Persistable {
    private static final double MONTHLY_RATE = 0.003;             // 0.3%
    private static final double EARLY_WITHDRAW_PENALTY = 0.01;    // 1% 违约金

    private LocalDate lockedUntil;     // 锁定到期日

    public SavingsAccount(String id, String ownerName, double initialBalance,
                          LocalDate lockedUntil) {
        super(id, ownerName, initialBalance);
        this.lockedUntil = lockedUntil;
    }

    @Override
    public void withdraw(double amount) {
        if (amount <= 0) throw new InvalidAmountException(amount);

        boolean inLockPeriod = LocalDate.now().isBefore(lockedUntil);
        double penalty = inLockPeriod ? amount * EARLY_WITHDRAW_PENALTY : 0;
        double total = amount + penalty;

        // 储蓄账户不允许透支
        if (getBalance() < total) {
            throw new InsufficientFundsException(total, getBalance());
        }
        setBalanceInternal(getBalance() - total);
        if (inLockPeriod) {
            System.out.printf("[储蓄] 锁定期内取款，扣违约金 %.2f%n", penalty);
        }
    }

    @Override
    public double calcInterest() {
        return getBalance() * MONTHLY_RATE;
    }

    @Override
    public char typeTag() { return 'S'; }

    public LocalDate getLockedUntil() { return lockedUntil; }

    @Override
    public String toCsv() {
        return String.join(",", String.valueOf(typeTag()), getId(), getOwnerName(),
                String.valueOf(getBalance()), lockedUntil.toString());
    }
}
