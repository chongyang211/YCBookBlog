package com.bank.dao;

import com.bank.entity.Account;
import com.bank.exception.BankException;
import com.bank.persist.Persistable;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.ArrayList;
import java.util.List;

import static java.nio.charset.StandardCharsets.UTF_8;

/**
 * 数据访问层：账户的 CSV 文件读写。
 */
public class AccountDao {

    private final Path filePath;

    public AccountDao(Path filePath) {
        this.filePath = filePath;
    }

    /** 把所有账户写入 CSV（覆盖原文件） */
    public void saveAll(Account[] accounts) {
        try (BufferedWriter bw = Files.newBufferedWriter(filePath, UTF_8,
                StandardOpenOption.CREATE, StandardOpenOption.TRUNCATE_EXISTING)) {
            for (Account a : accounts) {
                if (a == null) continue;            // 末尾填洞法可能留 null
                if (a instanceof Persistable p) {
                    bw.write(p.toCsv());
                    bw.newLine();
                }
            }
        } catch (IOException e) {
            throw new BankException("保存账户失败: " + filePath, e);
        }
    }

    /** 从 CSV 读取所有账户 */
    public List<Account> loadAll() {
        List<Account> result = new ArrayList<>();
        if (!Files.exists(filePath)) {
            return result;       // 文件不存在 = 没有历史数据，不算错
        }
        try (BufferedReader br = Files.newBufferedReader(filePath, UTF_8)) {
            String line;
            int lineNo = 0;
            while ((line = br.readLine()) != null) {
                lineNo++;
                line = line.strip();
                if (line.isEmpty()) continue;        // 跳过空行
                try {
                    result.add(Persistable.fromCsv(line));
                } catch (Exception parseEx) {
                    // 单行损坏不拖垮整批，记录警告 + 继续
                    System.err.printf("[警告] 第 %d 行解析失败，已跳过: %s%n", lineNo, line);
                }
            }
        } catch (IOException e) {
            throw new BankException("加载账户失败: " + filePath, e);
        }
        return result;
    }
}
