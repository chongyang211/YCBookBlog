package com.orders.service;

import java.util.concurrent.TimeUnit;

/** ✅ 修复 B：tryLock(timeout) 超时回退（破坏"不可抢占"条件，教程 §5.4）*/
public class BankLockable {

    public boolean transferTryLock(AccountLockable from, AccountLockable to,
                                   double amount) throws InterruptedException {
        for (int retry = 0; retry < 10; retry++) {
            if (from.lock.tryLock(50, TimeUnit.MILLISECONDS)) {
                try {
                    if (to.lock.tryLock(50, TimeUnit.MILLISECONDS)) {
                        try {
                            from.debit(amount);
                            to.credit(amount);
                            return true;
                        } finally {
                            to.lock.unlock();
                        }
                    }
                } finally {
                    from.lock.unlock();
                }
            }
            // 都没拿到 → 随机退避再试（防两线程同步空转成活锁）
            Thread.sleep((long) (Math.random() * 50));
        }
        return false;     // 重试 10 次仍失败
    }
}
