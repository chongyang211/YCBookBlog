//
// Created by 杨充 on 2026/5/25.
//

#include "Bank.h"
#include <iostream>

#include "FileManager.h"
#include "NormalAccount.h"
#include "SavingAccount.h"
#include "VipAccount.h"
using namespace std;

Bank::Bank() {
    cout << "[Bank] 银行系统启动\n";
}

Bank::~Bank() {
    for (Account* a : accounts) delete a;
    accounts.clear();
    cout << "[Bank] 银行系统关闭\n";
}

void Bank::openAccount() {
    cout << "\n--- 开户 ---\n";
    cout << "选择账户类型: 1=普通 2=VIP 3=储蓄: ";
    int type;
    cin >> type;
    string id, name;
    double initBalance;
    cout << "账号: "; cin >> id;
    cout << "户主姓名: "; cin >> name;
    cout << "初始余额: "; cin >> initBalance;
    Account* account = nullptr;
    switch (type) {
        case 1:
            account = new NormalAccount(id, name, initBalance);
            break;
        case 2:
            double rate;
            cout << "年利率（如 0.05）: "; cin >> rate;
            account = new VipAccount(id, name, initBalance, rate);
            break;
        case 3:
            int term;
            cout << "定期月数（如 12）: "; cin >> term;
            account = new SavingAccount(id, name, initBalance, term);
            break;
        default:
            cout << "[Bank] 无效的账户类型\n";
            break;
    }
    accounts.push_back(account);
    cout << "[Bank] 开户成功，当前账户总数: " << accounts.size() << "\n";
}

void Bank::deposit() {
    cout << "\n--- 存款 ---\n";
    string id;
    double amount;
    // ⚠ 怎么从 vector 里找到这个账号？？？
    cout << "账号: "; cin >> id;
    cout << "金额: "; cin >> amount;
    Account* account = findAccount(id);
    if (!account) {
        cout << "[Bank] 账号不存在\n";
        return;
    }
    account->deposit(amount);
}

void Bank::withdraw() {
    cout << "\n--- 取款 ---\n";
    string id;
    double amount;
    cout << "账号: "; cin >> id;
    cout << "金额: "; cin >> amount;
    Account* account = findAccount(id);
    if (!account) {
        cout << "[Bank] 账号不存在\n";
        return;
    }
    account->withdraw(amount);
}

void Bank::queryBalance() {
    cout << "\n--- 查询 ---\n";
    string id;
    cout << "账号: "; cin >> id;
    Account* acc = findAccount(id);
    if (!acc) {
        cout << "[Bank] 账号不存在\n";
        return;
    }
    acc->showInfo();         // ⭐ 多态调用：不同子类显示不同字段
}

void Bank::transfer() {
    cout << "\n--- 转账 ---\n";
    string fromId, toId;
    double amount;
    cout << "转出账号: "; cin >> fromId;
    cout << "转入账号: "; cin >> toId;
    cout << "金额: "; cin >> amount;
    Account* from = findAccount(fromId);
    Account* to   = findAccount(toId);
    if (!from || !to) {
        cout << "[Bank] 账号不存在\n";
        return;
    }
    if (fromId == toId) {
        cout << "[Bank] 不能转账给自己\n";
        return;
    }
    // 简易事务：先扣后加，扣失败则取消
    if (!from->withdraw(amount)) {
        cout << "[Bank] 转出失败，转账取消\n";
        return;
    }
    to->deposit(amount);
    cout << "[Bank] 转账成功 " << fromId << " -> " << toId
         << " ¥" << amount << "\n";
}

void Bank::showAll() const {
    cout << "\n--- 所有账户 (共 " << accounts.size() << " 个) ---\n";
    if (accounts.empty()) {
        cout << "（空）\n";
        return;
    }
    for (const Account* a : accounts) {
        a->showInfo();        // ⭐ 多态遍历：每个账户用自己的格式打印
    }
}

void Bank::saveAll() {
    FileManager::save(dataFile,accounts);
}

void Bank::loadAll() {
    // 先释放原有数据
    for (Account* a : accounts) delete a;
    accounts.clear();
    // 加载新数据
    accounts = FileManager::load(dataFile);
}


Account *Bank::findAccount(const std::string &id) {
    for (Account* a : accounts) {
        if (a->getId() == id) {
            return a;
        }
    }
    return nullptr;
}




