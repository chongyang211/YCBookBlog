package com.orders.cli;

import com.orders.service.Account;
import com.orders.service.AccountLockable;
import com.orders.service.Bank;
import com.orders.service.BankLockable;

/**
 * 阶段④死锁现场实录（教程 §05）：
 * ① transferOrdered 固定锁顺序 → 修复
 * ② transferTryLock 超时回退 → 修复
 * ③ transferBuggy 互转 → 死锁复现（jstack 可见 deadlock detected）
 * 死锁复现放最后：死锁线程无法恢复，只能 exit 退出 JVM */
public class DeadlockDemo {

    public static void main(String[] args) throws InterruptedException {
        Account a = new Account(1, 1000);
        Account b = new Account(2, 1000);
        Bank bank = new Bank();

        System.out.println("========== ① 修复 A：固定锁顺序（按 ID 大小）==========");
        Thread u1 = new Thread(() -> {
            for (int i = 0; i < 1000; i++) bank.transferOrdered(a, b, 1);
        }, "Ordered-A->B");
        Thread u2 = new Thread(() -> {
            for (int i = 0; i < 1000; i++) bank.transferOrdered(b, a, 1);
        }, "Ordered-B->A");
        u1.start(); u2.start();
        u1.join(); u2.join();
        System.out.println("a=" + a.getBalance() + ", b=" + b.getBalance()
                + "（互转 1000 次每次 1 元，应各自回到 1000）");

        System.out.println("\n========== ② 修复 B：tryLock 超时回退 ==========");
        AccountLockable x = new AccountLockable(1, 500);
        AccountLockable y = new AccountLockable(2, 500);
        BankLockable bl = new BankLockable();
        Thread v1 = new Thread(() -> repeat(bl, x, y), "TryLock-A->B");
        Thread v2 = new Thread(() -> repeat(bl, y, x), "TryLock-B->A");
        v1.start(); v2.start();
        v1.join(); v2.join();
        System.out.println("x=" + x.getBalance() + ", y=" + y.getBalance() + "（无死锁完成）");

        System.out.println("\n========== ③ Buggy 版：复现死锁 ==========");
        Thread t1 = new Thread(() -> bank.transferBuggy(a, b, 100), "Thread-A->B");
        Thread t2 = new Thread(() -> bank.transferBuggy(b, a, 50),  "Thread-B->A");
        t1.start();
        t2.start();
        t1.join(2000);
        t2.join(2000);

        System.out.println("Thread-A->B alive? " + t1.isAlive());
        System.out.println("Thread-B->A alive? " + t2.isAlive());
        if (t1.isAlive() && t2.isAlive()) {
            System.out.println("检测到死锁！两个线程都被阻塞");
            System.out.println("另开终端执行 jstack -l <pid> 可看到 \"Found one Java-level deadlock\"");
            // 死锁线程无法恢复，只能强制退出 JVM
            System.exit(0);
        }
    }

    private static void repeat(BankLockable bl, AccountLockable from, AccountLockable to) {
        try {
            for (int i = 0; i < 1000; i++) {
                while (!bl.transferTryLock(from, to, 1)) { /* 重试直到成功 */ }
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}
