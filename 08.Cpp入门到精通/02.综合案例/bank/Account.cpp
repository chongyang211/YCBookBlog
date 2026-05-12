//
// Created by 杨充 on 2025/7/16.
//

#include "Account.h"

// 构造函数
Account::Account(const std::string &accNumber, const std::string &accName, double initialBalance) {
    this->accountNumber = accNumber;
    this->name = accName;
    this->balance = initialBalance;
}

// 获取账户号
std::string Account::getAccountNumber() const {
    return accountNumber;
}

// 获取姓名
std::string Account::getName() const {
    return name;
}

// 获取余额
double Account::getBalance() const {
    return balance;
}

// 存款
void Account::deposit(double amount) {
    if (amount > 0) {
        balance += amount;
        std::cout << "存款成功！当前余额: " << balance << std::endl;
    } else {
        std::cout << "存款金额必须大于 0！" << std::endl;
    }
}

// 取款
void Account::withdraw(double amount) {
    if (amount > 0 && amount <= balance) {
        balance -= amount;
        std::cout << "取款成功！当前余额: " << balance << std::endl;
    } else {
        std::cout << "取款金额无效或余额不足！" << std::endl;
    }
}

void test() {
    std::cout << "转账金额无效或余额不足！" << std::endl;
}

// 转账
bool Account::transfer(Account &targetAccount, double amount) {
    test();
    //首先要确保转出的金额大于0，并且是小于用户余额
    if (amount > 0 && amount <= balance) {
        balance -= amount;
        targetAccount.deposit(amount);
        std::cout << "转账成功！当前余额: " << balance << std::endl;
        return true;
    } else {
        std::cout << "转账金额无效或余额不足！" << std::endl;
        return false;
    }
}

// 显示账户信息
void Account::display() const {
    std::cout << "账户号: " << accountNumber << ", 姓名: " << name << ", 余额: " << balance << std::endl;
}





