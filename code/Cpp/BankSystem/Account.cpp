//
// Created by 杨充 on 2026/5/25.
//

#include "Account.h"
#include <iostream>
using namespace std;

Account::Account(const std::string &id, const std::string &name, double initBalance)
        : accountId(id), ownerName(name), balance(initBalance) {
    cout << "[Account] 创建账户 " << id << " - " << name
     << " 初始余额 " << initBalance << endl;
}

void Account::deposit(double amount) {
    if (amount <=0) {
        cout << "[Account] 存款金额必须大于 0\n";
        return;
    }
    balance+= amount;
    cout << "[Account] 存款成功，新余额 " << balance << endl;
}


