//
// Created by 杨充 on 2026/5/25.
//

#include "SavingAccount.h"
#include <iostream>
#include <sstream>
#include <ctime>
using namespace std;

SavingAccount::SavingAccount(const std::string &id, const std::string &name,
    double initBalance, int termMonths) : Account(id,name,initBalance) , term(termMonths){
    // 当前时间 + termMonths * 30 天 = 到期日
    maturityTimestamp = time(nullptr) + (long long)termMonths * 30 * 24 * 3600;
}

// 构造函数 2：从 CSV 恢复（直接传入到期时间戳）
SavingAccount::SavingAccount(const string& id, const string& name,
                             double initBalance, int termMonths, long long maturity)
    : Account(id, name, initBalance), term(termMonths), maturityTimestamp(maturity) {
}

void SavingAccount::showInfo() const {
    char buf[64];
    time_t t = (time_t)maturityTimestamp;
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&t));

    cout << "[储蓄账户] 账号: " << accountId
         << " | 户主: " << ownerName
         << " | 余额: ¥" << balance
         << " | 定期: " << term << " 个月"
         << " | 到期日: " << buf
         << (time(nullptr) >= maturityTimestamp ? "（已到期）" : "（未到期）")
         << "\n";
}

bool SavingAccount::withdraw(double amount) {
    if (amount <= 0 || amount > balance) {
        cout << "[SavingAccount] 取款金额无效\n";
        return false;
    }

    bool isMature = (time(nullptr) >= maturityTimestamp);
    if (!isMature) {
        double penalty = amount * PENALTY_RATE;
        balance -= (amount + penalty);
        cout << "[SavingAccount] 未到期取款，扣违约金 ¥" << penalty
             << " 剩余 ¥" << balance << "\n";
    } else {
        balance -= amount;
        cout << "[SavingAccount] 到期取款，剩余 ¥" << balance << "\n";
    }
    return true;
}

string SavingAccount::toCsv() const {
    ostringstream oss;
    oss << typeTag() << "," << accountId << "," << ownerName
        << "," << balance << "," << term << "," << maturityTimestamp;
    return oss.str();
}

