//
// Created by 杨充 on 2026/5/25.
//

#pragma once

#include "Account.h"
#include <vector>
#include <string>


class Bank {
private:
    std::vector<Account*> accounts;
    std::string dataFile = "bank";
    Account* findAccount(const std::string& id);
public:
    Bank();
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



