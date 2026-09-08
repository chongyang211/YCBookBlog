#ifndef BANK_SYSTEM_H
#define BANK_SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNTS 100
#define NAME_LEN      50
#define FILENAME     "bank_accounts.txt"

// 账户结构体：卡号、姓名、余额
typedef struct {
    int    accountNo;       // 卡号
    char   name[NAME_LEN];  // 姓名
    double balance;         // 余额
} Account;

// 全局账户数组 + 计数器（C 没有类成员变量，用全局变量共享数据）
extern Account accounts[MAX_ACCOUNTS];
extern int     accountCount;

// ===== 菜单 =====
void showMenu();

// ===== 业务函数 =====
void openAccount();    // 开户（自动递增卡号）
void deposit();        // 存款
void withdraw();       // 取款（不能透支）
void checkBalance();   // 查询余额
void transfer();       // 转账（事务一致性）
void saveToFile();     // 保存到文本文件
void loadFromFile();   // 从文件加载

// ===== 辅助函数 =====
int findAccount(int accountNo);   // 按卡号查找索引，找不到返回 -1

#endif // BANK_SYSTEM_H
