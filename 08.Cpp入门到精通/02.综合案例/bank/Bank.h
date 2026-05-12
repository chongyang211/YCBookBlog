//
// Created by 杨充 on 2025/7/16.
//

#include <string>
#include <iostream>
#include <vector>

#include "Account.h"
#ifndef BANK_H
#define BANK_H

// Bank类是一个管理类/容器类，它的主要职责是：
// 账户集合管理：利用集合来管理所有的用户
// 业务流程控制：控制用户的开户，查找，存款，取款等操作
// 这里思考一下，为什么需要Bank类？
// 业务需求：银行需要管理成千上万个账户；需要提供跨账户服务（转账）；需要统计和查询功能
// 面向对象设计原则：单一职责：Account负责单账户，Bank负责多账户管理；开闭原则：可以轻松添加新的银行级功能；组合优于继承：Bank通过组合使用Account

class Bank {
private:
    std::vector<Account> accounts; // 所有账户
public:
    // 开户
    void createAccount(const std::string& accNumber, const std::string& accName, double initialBalance);
    // 根据账户号查找账户
    Account* findAccount(const std::string& accNumber);
    // 存款
    void deposit(const std::string& accNumber, double amount);
    // 取款
    void withdraw(const std::string& accNumber, double amount);
    // 查询余额
    void queryBalance(const std::string& accNumber) ;
    // 转账
    void transfer(const std::string& fromAccNumber, const std::string& toAccNumber, double amount);
    // 显示所有账户
    void displayAllAccounts() const ;
    // 获取所有账户
    const std::vector<Account>& getAccounts() const ;
    // 设置所有账户
    void setAccounts(const std::vector<Account>& accList) ;
};



#endif //BANK_H
