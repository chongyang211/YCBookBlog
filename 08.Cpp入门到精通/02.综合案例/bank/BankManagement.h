//
// 银行管理系统 - 合并版本
// Created by 杨充 on 2025/10/13
//

#ifndef BANK_MANAGEMENT_SYSTEM_H
#define BANK_MANAGEMENT_SYSTEM_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <fstream>
#include <sstream>
#include <cstdio>

// 账户类
class Account {
private:
    std::string accountNumber; // 账户号
    std::string name;          // 姓名
    double balance;            // 余额

public:
    // 构造函数
    Account(const std::string& accNumber, const std::string& accName, double initialBalance);
    
    // 获取账户号
    std::string getAccountNumber() const;
    
    // 获取姓名
    std::string getName() const;
    
    // 获取余额
    double getBalance() const;
    
    // 存款
    void deposit(double amount);
    
    // 取款
    void withdraw(double amount);
    
    // 转账
    bool transfer(Account& targetAccount, double amount);
    
    // 显示账户信息
    void display() const;
};

// 银行管理系统主类
class BankManagementSystem {
private:
    std::vector<Account> accounts;  // 所有账户
    std::string dataFileName;       // 数据文件名

    // 私有辅助方法
    void displayMenu();
    Account* findAccount(const std::string& accNumber);
    void pauseScreen();

public:
    // 构造函数
    BankManagementSystem(const std::string& filename = "accounts.txt");
    
    // 主运行方法
    void run();
    
    // 账户管理功能
    void openAccount();
    void deposit();
    void withdraw();
    void checkBalance();
    void transfer();
    void displayAllAccounts();
    void clearAllAccounts();
    
    // 文件操作功能
    void saveToFile();
    void loadFromFile();
    
    // 银行业务核心方法
    void createAccount(const std::string& accNumber, const std::string& accName, double initialBalance);
    void depositToAccount(const std::string& accNumber, double amount);
    void withdrawFromAccount(const std::string& accNumber, double amount);
    void queryBalance(const std::string& accNumber);
    void transferBetweenAccounts(const std::string& fromAccNumber, const std::string& toAccNumber, double amount);
    
    // 获取账户数量
    size_t getAccountCount() const;
};

#endif // BANK_MANAGEMENT_SYSTEM_H