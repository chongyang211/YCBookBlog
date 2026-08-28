//
// Created by 杨充 on 2026/5/25.
//

#ifndef SAVINGACCOUNT_H
#define SAVINGACCOUNT_H
#include "Account.h"


class SavingAccount : public Account {
private:
    int term;                                    // 定期月数（如 12 = 一年期）
    long long maturityTimestamp;                 // 到期时间戳（秒）
    static constexpr double PENALTY_RATE = 0.01; // 提前支取违约金率 1%

public:
    SavingAccount(const std::string& id, const std::string& name,
                  double initBalance, int termMonths);
    SavingAccount(const std::string& id, const std::string& name,
                  double initBalance, int termMonths, long long maturity);

    void showInfo() const override;
    bool withdraw(double amount) override;
    char typeTag() const override { return 'S'; }
    std::string toCsv() const override;

    int getTerm() const { return term; }
};



#endif //SAVINGACCOUNT_H
