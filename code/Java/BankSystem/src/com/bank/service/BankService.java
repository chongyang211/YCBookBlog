package com.bank.service;

import com.bank.dao.AccountDao;
import com.bank.entity.Account;
import com.bank.exception.AccountNotFoundException;
import com.bank.exception.BankException;

import java.util.List;

/**
 * 业务层：把实体 + 数据访问串成业务编排。
 */
public class BankService {
    private static final int MAX_ACCOUNTS = 100;

    private final Account[] accounts = new Account[MAX_ACCOUNTS];
    private int count = 0;
    private final AccountDao dao;

    public BankService(AccountDao dao) {
        this.dao = dao;
    }

    // ============ 启动加载 / 退出保存 ============
    public void load() {
        List<Account> loaded = dao.loadAll();
        for (Account a : loaded) {
            if (count >= MAX_ACCOUNTS) break;
            accounts[count++] = a;
        }
        System.out.printf("[启动] 加载 %d 个账户%n", count);
    }

    public void save() {
        dao.saveAll(accounts);
        System.out.printf("[退出] 保存 %d 个账户%n", count);
    }

    // ============ 开户 / 销户 ============
    public void openAccount(Account a) {
        if (count >= MAX_ACCOUNTS) {
            throw new BankException("账户数量已达上限 " + MAX_ACCOUNTS);
        }
        if (findIndex(a.getId()) != -1) {
            throw new BankException("账号已存在: " + a.getId());
        }
        accounts[count++] = a;
    }

    public void closeAccount(String id) {
        int idx = findIndex(id);
        if (idx == -1) throw new AccountNotFoundException(id);
        // 末尾填洞法（沿用 01 案例）
        accounts[idx] = accounts[count - 1];
        accounts[count - 1] = null;
        count--;
    }

    // ============ 查询 ============
    public Account find(String id) {
        int idx = findIndex(id);
        if (idx == -1) throw new AccountNotFoundException(id);
        return accounts[idx];
    }

    private int findIndex(String id) {
        for (int i = 0; i < count; i++) {
            if (accounts[i].getId().equals(id)) return i;
        }
        return -1;
    }

    public Account[] all() {
        Account[] copy = new Account[count];
        System.arraycopy(accounts, 0, copy, 0, count);
        return copy;
    }

    // ============ 转账（补偿事务）============
    public void transfer(String fromId, String toId, double amount) {
        Account from = find(fromId);
        Account to = find(toId);

        // 关键：先扣再加，扣失败不影响 to
        from.withdraw(amount);
        try {
            to.deposit(amount);
        } catch (RuntimeException e) {
            // 极小概率：deposit 失败，回滚 from
            from.deposit(amount);
            throw new BankException("转账失败已回滚: " + e.getMessage(), e);
        }
    }

    // ============ 月结息（多态）============
    public void applyMonthlyInterest() {
        for (int i = 0; i < count; i++) {
            Account a = accounts[i];
            double interest = a.calcInterest();
            if (interest > 0) {
                a.deposit(interest);
            }
        }
    }
}
