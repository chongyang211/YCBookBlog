//
// Created by 杨充 on 2026/5/25.
//
// ============================================================================
// 【文件定位】SavingAccount.h —— 实体层·储蓄账户子类（定期 + 违约金）
// ----------------------------------------------------------------------------
// 【设计思路】定期储蓄的规则核心是"时间"：到期前取款扣违约金、到期后全额可取。
//   所以它比别的子类多两个状态：定期月数、到期时间戳 —— 以及两个构造函数：
//   ① 正常开户（现在算出到期日）② 从文件恢复（到期日已在数据里，不用重算）。
//   "同一个类的两个构造函数服务两种来源" —— 构造函数重载的典型场景。
// ============================================================================

#ifndef SAVINGACCOUNT_H    // 头文件保护宏
#define SAVINGACCOUNT_H
#include "Account.h"       // 基类完整定义


class SavingAccount : public Account {    // public 继承：储蓄账户 is-a 账户
private:
    // 【可见性】与 VipAccount 同理：私有成员收在自己手里，不再下传
    int term;                                    // 定期月数（如 12 = 一年期）
    long long maturityTimestamp;                 // 到期时间戳（秒）
    // 【技术原理】long long（64 位）：Unix 秒级时间戳到 2922 亿年才溢出，
    //   32 位 int 只够到 2038 年（"2038 问题"）—— 时间戳一律 64 位。
    // static constexpr：类内编译期常量（C++17 隐式 inline），全类共享一份
    static constexpr double PENALTY_RATE = 0.01; // 提前支取违约金率 1%

public:
    // 构造 ①（正常开户）：只给定期月数，到期日 = 当前时间 + term 个月（内部算）
    SavingAccount(const std::string& id, const std::string& name,
                  double initBalance, int termMonths);
    // 构造 ②（文件恢复）：到期时间戳是保存前就算好的，直接传入避免重复计算
    // 【设计对比】重载构造而非默认参数/静态工厂：两种来源的语义差异
    //   （"算出来" vs "给定的"）体现在参数列表上，调用处一目了然。
    SavingAccount(const std::string& id, const std::string& name,
                  double initBalance, int termMonths, long long maturity);

    void showInfo() const override;
    bool withdraw(double amount) override;    // 规则：未到期取款扣 1% 违约金
    char typeTag() const override { return 'S'; }   // 类型标签 'S'（CSV 路标）
    std::string toCsv() const override;

    // 新增成员的只读访问器
    int getTerm() const { return term; }
};



#endif //SAVINGACCOUNT_H  // 头文件保护宏结束
