//
// Created by 杨充 on 2026/5/25.
//

#ifndef VIPACCOUNT_H
#define VIPACCOUNT_H
#include "Account.h"


class VipAccount : public Account{
private:
    double interestRate;        // 年利率（如 0.05 = 5%）
    static constexpr double OVERDRAFT_LIMIT = 1000.0;  // 透支额度
public:
    VipAccount(const std::string& id, const std::string& name, double initBalance, double rate);

    void showInfo() const override;
    bool withdraw(double amount) override;
    void deposit(double amount) override;       // VIP 重写：存款带利息
    char typeTag() const override { return 'V'; }
    std::string toCsv() const override;

    double getInterestRate() const { return interestRate; }
};



#endif //VIPACCOUNT_H
