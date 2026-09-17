//
// Created by 杨充 on 2026/5/25.
//

#pragma once

#include "Account.h"
#include <vector>
#include <string>


class Bank {
private:
    // 唯一的"数据库"
    std::vector<Account*> accounts;
    // file文件
    std::string dataFile = "bank";
    // 查找
    Account* findAccount(const std::string& id);
public:
    // 构造函数
    Bank();
    // 析构函数
    ~Bank();
    // 开户
    void openAccount();
    // 存款
    void deposit();
    // 取款
    void withdraw();
    // 查询余额
    void queryBalance();
    void transfer();
    void showAll() const;
    void saveAll();
    void loadAll();
};



