package com.bank.entity;

import com.bank.exception.InvalidAmountException;

import java.time.LocalDateTime;
import java.util.concurrent.atomic.AtomicLong;

/**
 * 账户抽象基类：公共字段 + 公共业务 + 抽象方法（强制子类实现）。
 */
public abstract class Account {

    // ============ 静态成员：自增 ID 工厂 ============
    private static final AtomicLong SEQ = new AtomicLong(1000);

    public static String generateId() {
        return "A" + SEQ.incrementAndGet();
    }

    // ============ 实例字段（全部 private）============
    private final String id;
    private String ownerName;
    private double balance;
    private final LocalDateTime createdAt;

    // ============ 构造方法重载 ============
    public Account(String id, String ownerName, double balance, LocalDateTime createdAt) {
        this.id = id;
        this.ownerName = ownerName;
        this.balance = balance;
        this.createdAt = createdAt;
    }

    public Account(String id, String ownerName, double balance) {
        this(id, ownerName, balance, LocalDateTime.now());
    }

    public Account(String id, String ownerName) {
        this(id, ownerName, 0.0);
    }

    // ============ 全套 getter（balance 故意不给 setter）============
    public String getId() { return id; }
    public String getOwnerName() { return ownerName; }
    public void setOwnerName(String ownerName) { this.ownerName = ownerName; }
    public double getBalance() { return balance; }
    public LocalDateTime getCreatedAt() { return createdAt; }

    // ============ 公共业务方法（默认实现，子类可不重写）============
    public void deposit(double amount) {
        if (amount <= 0) throw new InvalidAmountException(amount);
        balance += amount;
    }

    // ============ 抽象方法（强制子类实现）============
    public abstract void withdraw(double amount);

    /** 计算当月利息金额（不修改余额，只算） */
    public abstract double calcInterest();

    /** 类型标签（V / S / N），CSV 序列化用 */
    public abstract char typeTag();

    // ============ 受保护字段访问 ============
    /** 让子类能"动余额"——但只通过这个口子，便于以后加日志 */
    protected void setBalanceInternal(double newBalance) {
        this.balance = newBalance;
    }

    // ============ Object 三件套 ============
    @Override
    public String toString() {
        return String.format("Account{id=%s, owner=%s, balance=%.2f, createdAt=%s}",
                id, ownerName, balance, createdAt);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof Account that)) return false;   // JDK 16+ instanceof 模式匹配
        return java.util.Objects.equals(this.id, that.id); // 账号相同 = 同一个账户
    }

    @Override
    public int hashCode() {
        return java.util.Objects.hash(id);
    }
}
