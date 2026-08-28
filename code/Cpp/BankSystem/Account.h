//
// Created by 杨充 on 2026/5/25.
//

#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <string>
#include <iostream>

class Account {
protected:
    std::string accountId;
    std::string ownerName;
    double balance;
public:
    Account(const std::string& id, const std::string& name, double initBalance);
    virtual ~Account() = default;
    virtual void showInfo() const = 0;
    virtual bool withdraw(double amount) = 0;
    virtual char typeTag() const = 0;
    virtual std::string toCsv() const = 0;

    virtual void deposit(double amount);
    const std::string& getId()      const { return accountId; }
    const std::string& getOwner()   const { return ownerName; }
    double             getBalance() const { return balance; }
};



#endif //ACCOUNT_H
