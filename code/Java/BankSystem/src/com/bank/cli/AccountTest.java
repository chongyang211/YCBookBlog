package com.bank.cli;

import com.bank.dao.AccountDao;
import com.bank.entity.Account;
import com.bank.entity.NormalAccount;
import com.bank.entity.SavingsAccount;
import com.bank.entity.VipAccount;
import com.bank.exception.BankException;
import com.bank.exception.InsufficientFundsException;
import com.bank.persist.Persistable;
import com.bank.service.BankService;

import java.nio.file.Files;
import java.nio.file.Path;
import java.time.LocalDate;

/**
 * 端到端自测（不依赖 JUnit，自家 Assert 工具）。
 */
public class AccountTest {

    static int passed = 0, failed = 0;

    static void assertTrue(String name, boolean cond) {
        if (cond) { passed++; System.out.println("✅ " + name); }
        else      { failed++; System.out.println("❌ " + name); }
    }

    static <T extends Throwable> void assertThrows(String name,
            Class<T> expected, Runnable code) {
        try {
            code.run();
            failed++;
            System.out.println("❌ " + name + "（未抛异常）");
        } catch (Throwable t) {
            if (expected.isInstance(t)) {
                passed++;
                System.out.println("✅ " + name + " (" + t.getClass().getSimpleName() + ")");
            } else {
                failed++;
                System.out.println("❌ " + name + "（抛了 " + t.getClass().getSimpleName() + "）");
            }
        }
    }

    public static void main(String[] args) throws Exception {
        // 测试 1：开户成功
        Account a = new NormalAccount("T001", "Test", 1000);
        a.deposit(500);
        assertTrue("开户 + 存款，余额 1500", Math.abs(a.getBalance() - 1500) < 0.001);

        // 测试 2：透支抛异常
        Account n = new NormalAccount("T002", "Normal", 100);
        assertThrows("普通账户透支抛 InsufficientFundsException",
                InsufficientFundsException.class, () -> n.withdraw(200));

        // 测试 3：VIP 允许透支
        Account v = new VipAccount("T003", "VIP", 100, 1000);
        v.withdraw(500);
        assertTrue("VIP 允许透支至 -400", Math.abs(v.getBalance() + 400) < 0.001);

        // 测试 4：序列化 round-trip
        Account s = new SavingsAccount("T004", "Save", 8000, LocalDate.of(2026, 12, 31));
        Persistable p = (Persistable) s;
        Account back = Persistable.fromCsv(p.toCsv());
        assertTrue("Savings round-trip 余额相同",
                Math.abs(back.getBalance() - 8000) < 0.001);
        assertTrue("Savings round-trip ID 相同", back.getId().equals("T004"));

        // 测试 5：转账原子性
        Path tmp = Files.createTempFile("bank-test", ".csv");
        BankService svc = new BankService(new AccountDao(tmp));
        svc.openAccount(new NormalAccount("T100", "From", 1000));
        svc.openAccount(new NormalAccount("T200", "To", 1000));
        try { svc.transfer("T100", "T200", 5000); } catch (BankException e) { /* ok */ }
        assertTrue("转账失败 from 余额不变",
                Math.abs(svc.find("T100").getBalance() - 1000) < 0.001);
        assertTrue("转账失败 to 余额不变",
                Math.abs(svc.find("T200").getBalance() - 1000) < 0.001);
        Files.deleteIfExists(tmp);

        System.out.printf("%n=== 测试结果：%d passed, %d failed ===%n", passed, failed);
        System.exit(failed == 0 ? 0 : 1);
    }
}
