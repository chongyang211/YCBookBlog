//
// Created by 杨充 on 2026/5/25.
//
// ============================================================================
// 【文件定位】VipAccount.h —— 实体层·VIP 账户子类（带利息 + 透支额度）
// ----------------------------------------------------------------------------
// 【设计思路】VIP 是三种账户里"特权"最多的：存款有利息（重写 deposit 展示
//   "普通虚函数选择性重写"）、取款可透支 1000（重写 withdraw 换一套规则）。
//   它证明：同一个方法名，不同子类可以长出完全不同的行为 —— 多态的价值。
// ============================================================================

#ifndef VIPACCOUNT_H    // 头文件保护宏
#define VIPACCOUNT_H
#include "Account.h"   // 基类完整定义


class VipAccount : public Account{     // public 继承：VIP 账户 is-a 账户
private:
    // 【可见性选择】私有成员放子类自己（private 而非 protected）：
    //   "VIP 的利率"没有"孙子类"再继承的现实语义，收到最紧的可见性；
    //   基类成员则放 protected 供三个子类共用 —— 可见性按语义分层。
    double interestRate;        // 年利率（如 0.05 = 5%）
    // 【技术原理】static constexpr double：类内直接初始化的编译期常量
    //   （C++17 起 constexpr 静态成员隐式 inline，无需再类外定义）。
    //   属于"所有 VIP 账户共享的一个值"，不该每个对象存一份。
    static constexpr double OVERDRAFT_LIMIT = 1000.0;  // 透支额度
public:
    // 子类构造：4 参（比基类多一个利率）—— 子类新增字段由子类自己初始化
    VipAccount(const std::string& id, const std::string& name, double initBalance, double rate);

    void showInfo() const override;
    bool withdraw(double amount) override;      // 注意 VIP 这个取钱可以透支
    void deposit(double amount) override;       // VIP 重写：存款带利息
    // 【对比 NormalAccount】这里必须重写 deposit：基类默认实现不认利息；
    //   同一个"存钱"动作在 VIP 这里语义升级了 —— 这就是"普通虚函数"
    //   设计的用武之地（想覆盖就覆盖，不想覆盖的子类白拿默认实现）。
    char typeTag() const override { return 'V'; }   // 类型标签 'V'（CSV 反序列化路标）
    std::string toCsv() const override;

    // 新增成员的只读访问器（外部代码不该改利率，只该看）
    double getInterestRate() const { return interestRate; }
};



#endif //VIPACCOUNT_H  // 头文件保护宏结束
