package com.orders.service;

/**
 * 转账（教程 §05 死锁现场）：
 *   transferBuggy  —— ❌ 造 BUG：固定先锁 from 再锁 to，互转必死锁
 *   transferOrdered —— ✅ 修复 A：永远先锁 ID 小的账户（破坏循环等待）
 */
public class Bank {

    /** ❌ Buggy 版：固定先锁 from 再锁 to —— 互相转账时死锁 */
    public void transferBuggy(Account from, Account to, double amount) {
        synchronized (from) {
            try { Thread.sleep(10); } catch (Exception ignored) {}   // 放大死锁概率
            synchronized (to) {
                from.debit(amount);
                to.credit(amount);
            }
        }
    }

    /** ✅ 修复 A：固定锁顺序——按 ID 大小（业界默认，InnoDB 同款策略）*/
    public void transferOrdered(Account from, Account to, double amount) {
        Account first  = from.getId() < to.getId() ? from : to;
        Account second = from.getId() < to.getId() ? to   : from;

        synchronized (first) {
            synchronized (second) {
                from.debit(amount);
                to.credit(amount);
            }
        }
    }
}
