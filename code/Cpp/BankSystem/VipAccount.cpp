//
// Created by 杨充 on 2026/5/25.
//
// ============================================================================
// 【文件定位】VipAccount.cpp —— VIP 账户实现（利息存款 + 透支取款）
// ============================================================================

#include "VipAccount.h"
#include <iostream>
#include <sstream>    // ostringstream 拼 CSV
#include <iomanip>    // fixed / setprecision：浮点输出格式化
using namespace std;

// 子类构造：先基类（三件套），再自己的新成员（利率）
// 【技术原理】初始化列表顺序必须与"基类 → 成员声明序"一致；
//   基类子对象构造完成，VIP 特有字段才有机会初始化。
VipAccount::VipAccount(const string& id, const string& name,
                       double initBalance, double rate)
    : Account(id, name, initBalance), interestRate(rate) {
}

// 展示：在共性字段之外多打"利率 + 透支额度" —— 多态下每个子类的"自画像"
void VipAccount::showInfo() const {
    // fixed + setprecision(2) 是"粘性"流操纵符（设置后一直生效）：
    // 利率 0.05 → 显示 "5.00%"（两位小数的定点表示，而非科学计数）
    cout << "[VIP 账户] 账号: " << accountId
         << " | 户主: " << ownerName
         << " | 余额: ¥" << balance
         << " | 年利率: " << fixed << setprecision(2) << (interestRate * 100) << "%"
         << " | 透支额度: ¥" << OVERDRAFT_LIMIT << "\n";
}

// 取款规则：余额 + 透支额度 = 可用额度，允许余额为负
bool VipAccount::withdraw(double amount) {
    if (amount <= 0) {      // 卫语句：非正数金额拒绝（注意与 NormalAccount 的
                            // "< 0" 不同，这里统一成 <= 0 —— 0 元取款同样无意义）
        cout << "[VipAccount] 取款金额必须大于 0\n";
        return false;
    }
    if (amount > balance + OVERDRAFT_LIMIT) {     // VIP 允许透支 1000
        // 【核心规则】可用额度 = balance + 1000：余额 500 时最多可取 1500。
        // 这是与 NormalAccount 唯一的判定差异 —— 面向对象让"规则差异"
        // 收敛在各子类内部，调用方（Bank）完全无感知。
        cout << "[VipAccount] 超出透支额度，取款失败\n";
        return false;
    }
    balance -= amount;      // 余额可以变负（透支）：不变量从"非负"放宽为"≥ -1000"
    cout << "[VipAccount] 取款成功，剩余 ¥" << balance;
    if (balance < 0) cout << "（已透支）";    // 负余额给出显式提示（可读性）
    cout << "\n";
    return true;
}

// VIP 重写存款：入账之外再发"当月利息"（存款额 × 年利率 / 12）
void VipAccount::deposit(double amount) {
    if (amount <= 0) {      // 与基类相同的金额守卫（重写不复用基类实现，各自校验）
        cout << "[VipAccount] 存款金额必须大于 0\n";
        return;
    }
    double interest = amount * interestRate / 12.0;   // 当月利息
    // 【教学简化】按"单笔存入"立即计一个月利息，真实银行按日计息/复利
    balance += amount + interest;      // 本金 + 利息一起入账
    cout << "[VipAccount] 存款 ¥" << amount
         << " 利息 ¥" << fixed << setprecision(2) << interest
         << " 新余额 ¥" << balance << "\n";
}

// 序列化：格式 "V,账号,户主,余额,年利率" —— 比 NormalAccount 多一列
// 【契约要点】首列 'V' 与字段数（≥5）共同构成 VIP 行的识别依据，
//   FileManager::load 依赖这两个信息决定构造参数。
string VipAccount::toCsv() const {
    ostringstream oss;
    oss << typeTag() << "," << accountId << "," << ownerName
        << "," << balance << "," << interestRate;
    return oss.str();
}
