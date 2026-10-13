//
// Created by 杨充 on 2026/5/25.
//
// ============================================================================
// 【文件定位】SavingAccount.cpp —— 储蓄账户实现（到期日计算/违约金取款）
// ============================================================================

#include "SavingAccount.h"
#include <iostream>
#include <sstream>    // ostringstream 拼 CSV
#include <ctime>      // time_t / time() / localtime() / strftime：C 时间库
using namespace std;

// 构造 ①（正常开户）：定期月数已知 → 内部计算到期时间戳
SavingAccount::SavingAccount(const std::string &id, const std::string &name,
    double initBalance, int termMonths) : Account(id,name,initBalance) , term(termMonths){
    // 当前时间 + termMonths * 30 天 = 到期日
    // 【技术原理】time(nullptr) 返回当前 Unix 时间戳（1970-01-01 起的秒数，64 位）；
    //   "月"统一按 30 天近似（12*30=360 ≠ 365 天，教学简化，误差约 5 天/年）；
    //   (long long)termMonths 先转型再乘：防止 int * int 在极端值下先溢出再扩宽。
    maturityTimestamp = time(nullptr) + (long long)termMonths * 30 * 24 * 3600;
}

// 构造函数 2：从 CSV 恢复（直接传入到期时间戳）
// 【设计动机】保存的记录里已存了当初算好的到期日 —— 恢复时如果重新按
//   "现在 + term" 计算，每加载一次到期日就往后推一次，数据就失真了。
//   "计算类字段持久化后以'给定值'恢复" 是序列化/反序列化的通用原则。
SavingAccount::SavingAccount(const string& id, const string& name,
                             double initBalance, int termMonths, long long maturity)
    : Account(id, name, initBalance), term(termMonths), maturityTimestamp(maturity) {
}

// 展示：把"到期时间戳"翻译成人类可读的日期，并标注是否已到期
void SavingAccount::showInfo() const {
    char buf[64];                               // 足够容纳 "YYYY-MM-DD"（10 字节绰绰有余）
    time_t t = (time_t)maturityTimestamp;       // long long → time_t（64 位安全转换）
    // 【技术原理】localtime()：把 UTC 时间戳转换为"本地时区"的 struct tm
    //   （返回静态缓冲区指针，非线程安全 —— 控制台单线程程序无碍）；
    //   strftime()：按格式串 "%Y-%m-%d" 把 tm 渲染进 buf。
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&t));

    cout << "[储蓄账户] 账号: " << accountId
         << " | 户主: " << ownerName
         << " | 余额: ¥" << balance
         << " | 定期: " << term << " 个月"
         << " | 到期日: " << buf
         // 实时判断到期状态：当前时间 ≥ 到期时间即"已到期"（三目的输出标注）
         << (time(nullptr) >= maturityTimestamp ? "（已到期）" : "（未到期）")
         << "\n";
}

// 取款规则：未到期扣 1% 违约金，到期全额可取
bool SavingAccount::withdraw(double amount) {
    if (amount <= 0 || amount > balance) {     // 合并卫语句：金额非法或超额都拒绝
        cout << "[SavingAccount] 取款金额无效\n";
        return false;
    }

    // 时刻判定：同一行代码在不同时间点运行，结果可以不同（状态随时间演化）
    bool isMature = (time(nullptr) >= maturityTimestamp);
    if (!isMature) {
        double penalty = amount * PENALTY_RATE;   // 违约金 = 取款额 × 1%
        balance -= (amount + penalty);            // 取款额 + 违约金一起扣
        cout << "[SavingAccount] 未到期取款，扣违约金 ¥" << penalty
             << " 剩余 ¥" << balance << "\n";
    } else {
        balance -= amount;                        // 到期取款：无任何附加费用
        cout << "[SavingAccount] 到期取款，剩余 ¥" << balance << "\n";
    }
    return true;
}

// 序列化：格式 "S,账号,户主,余额,定期月数,到期时间戳" —— 三种账户里最长的一行
// 【契约要点】必须保存 maturityTimestamp（而非只存 term）：下次启动重新算会
//   改变语义（见构造 ② 的设计动机）—— 持久化"事实数据"，不持久化"可推导数据"
//   时的例外：时间类数据必须存结果，因为推导基准（当前时间）会变。
string SavingAccount::toCsv() const {
    ostringstream oss;
    oss << typeTag() << "," << accountId << "," << ownerName
        << "," << balance << "," << term << "," << maturityTimestamp;
    return oss.str();
}

