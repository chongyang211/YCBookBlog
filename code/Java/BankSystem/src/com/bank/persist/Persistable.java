package com.bank.persist;

import com.bank.entity.Account;
import com.bank.entity.NormalAccount;
import com.bank.entity.SavingsAccount;
import com.bank.entity.VipAccount;

import java.time.LocalDate;

/**
 * 持久化能力接口：对象 ↔ CSV 字符串双向转换。
 */
public interface Persistable {

    /** 实例方法：把自己序列化成 CSV 行 */
    String toCsv();

    /** 默认方法：序列化成 JSON 简单形式 —— 演示 default */
    default String toJson() {
        return "{\"raw\":\"" + toCsv() + "\"}";
    }

    /** 静态工厂：从 CSV 行创建 Account 子类（多态创建） */
    static Account fromCsv(String line) {
        String[] parts = line.split(",", -1);   // -1 保留末尾空字段
        char tag = parts[0].charAt(0);
        return switch (tag) {                   // switch 表达式
            case 'N' -> new NormalAccount(parts[1], parts[2], Double.parseDouble(parts[3]));
            case 'V' -> new VipAccount(parts[1], parts[2], Double.parseDouble(parts[3]),
                    Double.parseDouble(parts[4]));
            case 'S' -> new SavingsAccount(parts[1], parts[2], Double.parseDouble(parts[3]),
                    LocalDate.parse(parts[4]));
            default -> throw new IllegalArgumentException("未知账户类型: " + tag);
        };
    }
}
