//
// Created by 杨充 on 2026/5/25.
//
// ============================================================================
// 【文件定位】NormalAccount.cpp —— 普通账户子类的实现（展示/取款/序列化）
// ============================================================================

#include "NormalAccount.h"
#include <sstream>    // std::ostringstream：字符串流拼 CSV（比 + 拼接少临时对象）

// 子类构造：初始化列表只有一项 ": Account(id,name,initBalance)"
// 【技术原理】基类子对象必须通过初始化列表"委托"给基类构造；
//   函数体本身无事可做（子类没有新增成员）。
NormalAccount::NormalAccount(const std::string &id, const std::string &name, double initBalance)
    : Account(id,name,initBalance) {
    // 调用基类构造
}

// 展示：直接访问 protected 成员（accountId/ownerName/balance）
// 【技术原理】const 成员函数：承诺不修改对象；配合基类里 showInfo() const = 0
//   的签名（const 正确性自上而下贯彻，否则 override 编译不过）。
void NormalAccount::showInfo() const {
    std::cout << "[普通账户] 账号: " << accountId
     << " | 户主: " << ownerName
     << " | 余额: ¥" << balance << "\n";
}

// 取款规则：普通账户"禁透支"—— 余额不够就拒绝
bool NormalAccount::withdraw(double amount) {
    if (amount < 0) {       // 卫语句 1：负数取款无意义（教学简化：0 元同样无意义，
                            // 这里用 < 0，其他子类用 <= 0 —— 统一成 <= 0 更严谨）
        std::cout << "[NormalAccount] 取款金额必须大于 0\n";
        return false;       // 失败要"说出来"：返回 false 让调用方（如转账）决定取消
    }
    if (amount > balance) { // 卫语句 2：透支防线 —— 这是普通账户与 VIP 的本质区别
        std::cout << "[NormalAccount] 余额不足，取款失败\n";
        return false;
    }
    balance -= amount;      // 通过两道守卫才动余额（先校验后修改，保证不变量成立）
    std::cout << "[NormalAccount] 取款成功，剩余 ¥" << balance << "\n";
    return true;
}

// 序列化：对象 → CSV 一行，格式 "N,账号,户主,余额"
// 【设计思路】toCsv 与 typeTag 配对构成"持久化协议"：
//   首列类型标签是反序列化的"路标"，FileManager::load 读到 'N' 就 new NormalAccount。
std::string NormalAccount::toCsv() const {
    std::ostringstream oss;         // ostringstream：往"内存里的流"写，最后一次性取出字符串
    oss << typeTag() << "," << accountId << "," << ownerName << "," << balance;
    return oss.str();               // str() 导出流内累积的内容
}


