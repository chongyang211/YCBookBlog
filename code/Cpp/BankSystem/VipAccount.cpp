//
// Created by 杨充 on 2026/5/25.
//

#include "VipAccount.h"
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

VipAccount::VipAccount(const string& id, const string& name,
                       double initBalance, double rate)
    : Account(id, name, initBalance), interestRate(rate) {
}

void VipAccount::showInfo() const {
    cout << "[VIP 账户] 账号: " << accountId
         << " | 户主: " << ownerName
         << " | 余额: ¥" << balance
         << " | 年利率: " << fixed << setprecision(2) << (interestRate * 100) << "%"
         << " | 透支额度: ¥" << OVERDRAFT_LIMIT << "\n";
}

bool VipAccount::withdraw(double amount) {
    if (amount <= 0) {
        cout << "[VipAccount] 取款金额必须大于 0\n";
        return false;
    }
    if (amount > balance + OVERDRAFT_LIMIT) {     // VIP 允许透支 1000
        cout << "[VipAccount] 超出透支额度，取款失败\n";
        return false;
    }
    balance -= amount;
    cout << "[VipAccount] 取款成功，剩余 ¥" << balance;
    if (balance < 0) cout << "（已透支）";
    cout << "\n";
    return true;
}

void VipAccount::deposit(double amount) {
    if (amount <= 0) {
        cout << "[VipAccount] 存款金额必须大于 0\n";
        return;
    }
    double interest = amount * interestRate / 12.0;   // 当月利息
    balance += amount + interest;
    cout << "[VipAccount] 存款 ¥" << amount
         << " 利息 ¥" << fixed << setprecision(2) << interest
         << " 新余额 ¥" << balance << "\n";
}

string VipAccount::toCsv() const {
    ostringstream oss;
    oss << typeTag() << "," << accountId << "," << ownerName
        << "," << balance << "," << interestRate;
    return oss.str();
}