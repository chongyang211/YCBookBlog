package com.orders.service;

import java.util.concurrent.locks.ReentrantLock;

/** ReentrantLock 版账户：配合 BankLockable 的 tryLock 超时方案（教程 §5.4）*/
public class AccountLockable {
    final ReentrantLock lock = new ReentrantLock();
    private final long id;
    private double balance;

    public AccountLockable(long id, double balance) {
        this.id = id;
        this.balance = balance;
    }

    public long getId()          { return id; }
    public double getBalance()   { return balance; }
    public void debit(double v)  { balance -= v; }
    public void credit(double v) { balance += v; }
}
