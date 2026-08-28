//
// Created by 杨充 on 2026/5/25.
//

#include "NormalAccount.h"
#include <sstream>

NormalAccount::NormalAccount(const std::string &id, const std::string &name, double initBalance)
    : Account(id,name,initBalance) {
    // 调用基类构造
}

void NormalAccount::showInfo() const {
    std::cout << "[普通账户] 账号: " << accountId
     << " | 户主: " << ownerName
     << " | 余额: ¥" << balance << "\n";
}

bool NormalAccount::withdraw(double amount) {
    if (amount < 0) {
        std::cout << "[NormalAccount] 取款金额必须大于 0\n";
        return false;
    }
    if (amount > balance) {
        std::cout << "[NormalAccount] 余额不足，取款失败\n";
        return false;
    }
    balance -= amount;
    std::cout << "[NormalAccount] 取款成功，剩余 ¥" << balance << "\n";
    return true;
}

std::string NormalAccount::toCsv() const {
    std::ostringstream oss;
    oss << typeTag() << "," << accountId << "," << ownerName << "," << balance;
    return oss.str();
}


