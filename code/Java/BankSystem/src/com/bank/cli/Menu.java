package com.bank.cli;

import com.bank.dao.AccountDao;
import com.bank.entity.Account;
import com.bank.entity.NormalAccount;
import com.bank.entity.SavingsAccount;
import com.bank.entity.VipAccount;
import com.bank.exception.BankException;
import com.bank.service.BankService;

import java.nio.file.Path;
import java.time.LocalDate;
import java.util.Scanner;

/**
 * CLI 主菜单（含 main 入口）。
 */
public class Menu {
    private static final Scanner SC = new Scanner(System.in);
    private static final BankService SERVICE = new BankService(
            new AccountDao(Path.of("accounts.csv")));

    public static void main(String[] args) {
        SERVICE.load();
        while (true) {
            showMenu();
            String choice = SC.nextLine().trim();
            try {
                switch (choice) {
                    case "1" -> openAccount();
                    case "2" -> closeAccount();
                    case "3" -> deposit();
                    case "4" -> withdraw();
                    case "5" -> transfer();
                    case "6" -> query();
                    case "7" -> applyInterest();
                    case "0" -> { SERVICE.save(); System.out.println("再见"); return; }
                    default -> System.out.println("无效选项");
                }
            } catch (BankException e) {        // 业务异常统一兜底
                System.out.println("✗ " + e.getMessage());
            } catch (NumberFormatException e) {
                System.out.println("✗ 数字格式错误");
            }
        }
    }

    static void showMenu() {
        System.out.println("\n========= 银行账户管理系统 =========");
        System.out.println(" 1. 开户   2. 销户   3. 存款   4. 取款");
        System.out.println(" 5. 转账   6. 查询   7. 月结息  0. 退出");
        System.out.println("===================================");
        System.out.print("请选择: ");
    }

    static void openAccount() {
        System.out.print("类型(1=普通 2=VIP 3=储蓄): ");
        int type = Integer.parseInt(SC.nextLine().trim());
        System.out.print("账号: ");      String id   = SC.nextLine().trim();
        System.out.print("姓名: ");      String name = SC.nextLine().trim();
        System.out.print("初始余额: ");  double bal  = Double.parseDouble(SC.nextLine().trim());

        Account a = switch (type) {
            case 1 -> new NormalAccount(id, name, bal);
            case 2 -> new VipAccount(id, name, bal);
            case 3 -> {
                System.out.print("锁定到期(yyyy-MM-dd): ");
                LocalDate until = LocalDate.parse(SC.nextLine().trim());
                yield new SavingsAccount(id, name, bal, until);
            }
            default -> throw new BankException("未知账户类型: " + type);
        };
        SERVICE.openAccount(a);
        System.out.println("✅ 开户成功");
    }

    static void closeAccount() {
        System.out.print("销户账号: ");
        SERVICE.closeAccount(SC.nextLine().trim());
        System.out.println("✅ 销户成功");
    }

    static void deposit() {
        System.out.print("账号: ");      String id  = SC.nextLine().trim();
        System.out.print("金额: ");      double amt = Double.parseDouble(SC.nextLine().trim());
        SERVICE.find(id).deposit(amt);
        System.out.println("✅ 存款成功");
    }

    static void withdraw() {
        System.out.print("账号: ");      String id  = SC.nextLine().trim();
        System.out.print("金额: ");      double amt = Double.parseDouble(SC.nextLine().trim());
        SERVICE.find(id).withdraw(amt);  // 多态调用
        System.out.println("✅ 取款成功");
    }

    static void transfer() {
        System.out.print("转出账号: ");  String from = SC.nextLine().trim();
        System.out.print("转入账号: ");  String to   = SC.nextLine().trim();
        System.out.print("金额: ");      double amt  = Double.parseDouble(SC.nextLine().trim());
        SERVICE.transfer(from, to, amt);
        System.out.println("✅ 转账成功");
    }

    static void query() {
        System.out.print("账号: ");
        Account a = SERVICE.find(SC.nextLine().trim());
        System.out.println(a);
        if (a instanceof VipAccount v) {
            System.out.printf("  专属：透支额度 %.2f%n", v.getCreditLimit());
        } else if (a instanceof SavingsAccount s) {
            System.out.printf("  专属：锁定至 %s%n", s.getLockedUntil());
        }
    }

    static void applyInterest() {
        SERVICE.applyMonthlyInterest();
        System.out.println("✅ 月结息完成");
    }
}
