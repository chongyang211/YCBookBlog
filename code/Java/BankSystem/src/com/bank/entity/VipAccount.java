package com.bank.entity;

import com.bank.exception.InsufficientFundsException;
import com.bank.exception.InvalidAmountException;
import com.bank.persist.Persistable;

/**
 * VIP 账户：允许透支至 -creditLimit（默认 1000），月利率 0.5%。
 */
public class VipAccount extends Account implements Persistable {
    private static final double DEFAULT_CREDIT_LIMIT = 1000.0;
    private static final double MONTHLY_RATE = 0.005;     // 0.5%

    private double creditLimit;     // 透支额度

    public VipAccount(String id, String ownerName, double initialBalance) {
        this(id, ownerName, initialBalance, DEFAULT_CREDIT_LIMIT);
    }

    public VipAccount(String id, String ownerName, double initialBalance, double creditLimit) {
        super(id, ownerName, initialBalance);
        this.creditLimit = creditLimit;
    }

    @Override
    public void withdraw(double amount) {
        if (amount <= 0) throw new InvalidAmountException(amount);
        // VIP 规则：余额 - 取款 ≥ -透支额度
        if (getBalance() - amount < -creditLimit) {
            throw new InsufficientFundsException(amount, getBalance() + creditLimit);
        }
        setBalanceInternal(getBalance() - amount);
    }

    @Override
    public double calcInterest() {
        // VIP 规则：负余额不计息（透支不返利）；正余额按 0.5% 月利率
        double bal = getBalance();
        return bal > 0 ? bal * MONTHLY_RATE : 0;
    }

    @Override
    public char typeTag() { return 'V'; }

    public double getCreditLimit() { return creditLimit; }

    /** VIP 专属方法（演示向下转型时用） */
    public void useVipBenefit() {
        System.out.println("[VIP] 享受机场贵宾室、免年费、专属客服");
    }

    @Override
    public String toCsv() {
        return String.join(",", String.valueOf(typeTag()), getId(), getOwnerName(),
                String.valueOf(getBalance()), String.valueOf(creditLimit));
    }
}
