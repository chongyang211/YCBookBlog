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
    void openAccount();
    void deposit();
    void withdraw();
    void queryBalance();
    void transfer();
    void showAll() const;
    void saveAll();
    void loadAll();
};



