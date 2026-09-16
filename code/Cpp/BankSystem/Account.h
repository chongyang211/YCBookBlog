//
// Created by 杨充 on 2026/5/25.
//

#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <string>
#include <iostream>

// Account 是抽象基类，不能直接实例化
class Account {
protected:
    std::string accountId;   // 账号
    std::string ownerName;   // 户主姓名
    double balance;          // 余额
public:
    // 构造函数，子类通过基类构造列表调用
    Account(const std::string& id, const std::string& name, double initBalance);
    // 析构函数，为什么要设计成 virtual 虚函数？如果不写成虚函数，子类delete时候，那么就只调基类析构，无法释放子类自己
    virtual ~Account() = default;
    // 纯虚函数，子类必须实现
    // 有纯虚函数的类，不可以直接new
    virtual void showInfo() const = 0;
    virtual bool withdraw(double amount) = 0;
    virtual char typeTag() const = 0;
    virtual std::string toCsv() const = 0;

    // 普通虚函数，基类有默认实现，自己可以选择性重写
    virtual void deposit(double amount);

    // 普通函数，子类共享
    const std::string& getId()      const { return accountId; }
    const std::string& getOwner()   const { return ownerName; }
    double             getBalance() const { return balance; }
};



#endif //ACCOUNT_H
