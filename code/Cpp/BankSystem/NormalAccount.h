//
// Created by 杨充 on 2026/5/25.
//

#ifndef NORMALACCOUNT_H
#define NORMALACCOUNT_H
#include "Account.h"


class NormalAccount : public Account {
public:
    NormalAccount(const std::string& id, const std::string& name, double initBalance);

    void showInfo() const override;
    bool withdraw(double amount) override;
    char typeTag() const override { return 'N'; }
    std::string toCsv() const override;
};



#endif //NORMALACCOUNT_H
