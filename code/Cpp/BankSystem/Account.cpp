//
// Created by 杨充 on 2026/5/25.
//
// ============================================================================
// 【文件定位】Account.cpp —— 抽象基类的实现（构造 + 存款默认实现）
// 【设计思路】只实现"三种账户完全一致"的部分；带差异的行为留给子类各自写。
// ============================================================================

#include "Account.h"
#include <iostream>
using namespace std;

// 类外定义构造函数：作用域解析符 "Account::" 表明这是类外实现的成员
// 【技术原理】初始化列表 ": accountId(id), ownerName(name), balance(initBalance)"：
//   成员在进入函数体之前直接用实参拷贝构造（一步到位）；
//   写在函数体内赋值则是"先默认构造空串/随机值，再赋值"（多一步开销）。
//   基类成员只能由基类构造函数初始化 —— 子类无权代劳（封装边界）。
Account::Account(const std::string &id, const std::string &name, double initBalance)
        : accountId(id), ownerName(name), balance(initBalance) {
    // 构造即审计：打印创建日志便于观察对象生命周期（教学用）
    cout << "[Account] 创建账户 " << id << " - " << name
     << " 初始余额 " << initBalance << endl;
}

// 存款默认实现：金额守卫 → 入账 → 回显
// 【为什么放基类】规则对所有子类一致（金额必须为正），
//   NormalAccount/SavingAccount 直接继承使用；VipAccount 重写它追加利息。
// 注意此函数未声明 const：它要修改 balance，const 成员函数无法写成员。
void Account::deposit(double amount) {
    if (amount <=0) {                       // 卫语句：非正数金额直接拒绝（防呆）
        cout << "[Account] 存款金额必须大于 0\n";
        return;
    }
    balance+= amount;                       // 入账：直接修改 protected 成员
    cout << "[Account] 存款成功，新余额 " << balance << endl;
}


