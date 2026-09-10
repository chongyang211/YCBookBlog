package com.orders.service;

/** 银行账户（synchronized 版，死锁场景用）——id 用于固定锁顺序 */
public class Account {
    private final long id;
    private double balance;

    public Account(long id, double balance) {
        this.id = id;
        this.balance = balance;
    }

    public long getId()          { return id; }
    public double getBalance()   { return balance; }
    public void debit(double v)  { balance -= v; }
    public void credit(double v) { balance += v; }
}
