//
// Created by 杨充 on 2025/7/16.
//
#include <string>
#include <iostream>
#ifndef ACCOUNT_H
#define ACCOUNT_H

//用户类
// Account类是一个实体类/数据类，单个账户的数据和行为。代表单个账户：
// 用户属性数据的封装，比如用户名，余额等
// 单账户操作。本账户存款，取款，或者转账等
class Account {
private:
    std::string accountNumber; // 账户号
    std::string name;       //姓名
    double balance;     //余额
public:
    // 构造函数
    // 当前的写法（推荐）
    // 使用 const std::string& 是C++的最佳实践，它提供了：
    // 性能优势：避免不必要的字符串复制
    // 安全性：防止意外修改参数
    // 灵活性：可以接受各种形式的字符串参数
    Account(const std::string& accNumber, const std::string& accName, double initialBalance);
    // 如果不用const和引用
    // Account(std::string accNumber, std::string accName, double initialBalance);
    // 获取账户号
    std::string getAccountNumber() const;
    // std::string getAccountNumber();
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



#endif //ACCOUNT_H
