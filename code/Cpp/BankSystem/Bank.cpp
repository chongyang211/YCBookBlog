//
// Created by 杨充 on 2026/5/25.
//
// ============================================================================
// 【文件定位】Bank.cpp —— 管理层实现：开户工厂 / 多态分发 / 简易事务 / 持久化调度
// ----------------------------------------------------------------------------
// 【设计思路】每个业务方法的固定套路：
//   采集输入 → findAccount 定位 → 空指针卫语句 → 多态调用子类方法 → 回显。
//   Bank 全程只拿 Account*（基类接口）干活，从不关心对面是哪种账户 ——
//   "新增第四种账户"时 Bank 代码几乎不用改，这就是多态换来的扩展性。
// ============================================================================

#include "Bank.h"
#include <iostream>

#include "FileManager.h"       // 持久化层：save/load 委托给它
#include "NormalAccount.h"     // 开户工厂需要 new 三种子类（需要完整定义）
#include "SavingAccount.h"
#include "VipAccount.h"
using namespace std;

// 构造：容器自动初始化为空，dataFile 走 NSDMI 默认值 —— 函数体只打印横幅
Bank::Bank() {
    cout << "[Bank] 银行系统启动\n";
}

// 析构：释放所有账户对象 —— 裸指针所有权的"兜底回收站"
// 【技术原理】delete a 通过基类指针触发"虚析构"：Account 析构是 virtual，
//   实际执行的是 NormalAccount/VipAccount/SavingAccount 各自的析构链
//   （子类 → 基类）。若基类析构非虚，这里只会析构基类部分 —— 泄漏子类资源。
Bank::~Bank() {
    for (Account* a : accounts) delete a;    // 逐个释放堆对象（delete 空指针是安全的，但容器里不应有空指针）
    accounts.clear();                       // 清空指针本身（析构后 vector 也会自动清，这里显式表达意图）
    cout << "[Bank] 银行系统关闭\n";
}

// 开户：按用户选择的类型 new 对应子类 —— 简单工厂模式
void Bank::openAccount() {
    cout << "\n--- 开户 ---\n";
    cout << "选择账户类型: 1=普通 2=VIP 3=储蓄: ";
    int type;
    cin >> type;
    string id, name;
    double initBalance;
    cout << "账号: "; cin >> id;             // 采集共性字段（三种子类都要）
    cout << "户主姓名: "; cin >> name;
    cout << "初始余额: "; cin >> initBalance;
    Account* account = nullptr;              // 先置空：任何分支没赋值它就是 nullptr，可判别
    // 【技术细节】case 内声明变量（rate/term）不带花括号：因为它们是"平凡类型
    //   且无初始化器"，跳转初始化是合法的；若换成 string 等非平凡类型就必须加 {}。
    switch (type) {
        case 1:                              // 普通：无附加字段，三件套直接构造
            account = new NormalAccount(id, name, initBalance);
            break;
        case 2:                              // VIP：附加"年利率"参数
            double rate;
            cout << "年利率（如 0.05）: "; cin >> rate;
            account = new VipAccount(id, name, initBalance, rate);
            break;
        case 3:                              // 储蓄：附加"定期月数"参数
            int term;
            cout << "定期月数（如 12）: "; cin >> term;
            account = new SavingAccount(id, name, initBalance, term);
            break;
        default:
            cout << "[Bank] 无效的账户类型\n";
            break;
    }
    // 【修复说明】原代码无条件 push_back：选了无效类型时 account 仍是 nullptr，
    //   空指针入容器后，showAll/findAccount 遍历解引用直接崩溃。
    //   卫语句兜底：没有成功构造出账户就放弃本次开户。
    if (account == nullptr) {
        return;
    }
    accounts.push_back(account);             // 堆对象的指针交容器保管（所有权归 Bank）
    cout << "[Bank] 开户成功，当前账户总数: " << accounts.size() << "\n";
}

// 存款：定位 → 多态 deposit（VIP 会自动多入账利息，Bank 无感知）
void Bank::deposit() {
    cout << "\n--- 存款 ---\n";
    string id;
    double amount;
    // ⚠ 怎么从 vector 里找到这个账号？？？
    // 【解答】这是开发中自然冒出的问题 → 抽出 findAccount 辅助函数（见文件底部）：
    //   后面取款/查询/转账还要再用三次，一次抽好处处受益（DRY 原则）。
    cout << "账号: "; cin >> id;
    cout << "金额: "; cin >> amount;
    Account* account = findAccount(id);      // 线性查找，返回目标指针或 nullptr
    if (!account) {                          // 空指针卫语句：查无此人就提前返回
        cout << "[Bank] 账号不存在\n";
        return;
    }
    account->deposit(amount);                 // 虚调用：VIP→带利息版，其余→基类默认版
}

// 取款：与存款完全同构（定位 + 卫语句 + 多态调用）
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
    account->withdraw(amount);                // 虚调用：普通禁透支 / VIP 可透支 / 储蓄扣违约金
}

// 查询：一行多态调用，每个子类展示自己的"自画像"
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

// 转账：两个定位 + 自转校验 + "先扣后加"的简易事务
void Bank::transfer() {
    cout << "\n--- 转账 ---\n";
    string fromId, toId;
    double amount;
    cout << "转出账号: "; cin >> fromId;
    cout << "转入账号: "; cin >> toId;
    cout << "金额: "; cin >> amount;
    Account* from = findAccount(fromId);      // 定位转出方
    Account* to   = findAccount(toId);        // 定位转入方
    if (!from || !to) {                       // 两个都检查：缺一不可（|| 短路求值）
        cout << "[Bank] 账号不存在\n";
        return;
    }
    if (fromId == toId) {                     // 业务规则：自己转自己毫无意义还可能被钻空子
        cout << "[Bank] 不能转账给自己\n";
        return;
    }
    // 简易事务：先扣后加，扣失败则取消
    // 【设计思路·事务的雏形】用 withdraw 的 bool 返回值当"提交信号"：
    //   转出成功才执行转入；转出失败（余额/额度不足）立即取消 —— 保证不会
    //   出现"钱扣了却没到账"的中间态。真正的银行事务还需要回滚（转入失败时
    //   恢复转出方余额），教学版受益于"deposit 只会因金额非法失败"的约定。
    if (!from->withdraw(amount)) {            // 步骤①：多态扣款（VIP 转出允许透支）
        cout << "[Bank] 转出失败，转账取消\n";
        return;
    }
    to->deposit(amount);                      // 步骤②：多态入账（VIP 转入还有利息）
    cout << "[Bank] 转账成功 " << fromId << " -> " << toId
         << " ¥" << amount << "\n";
}

// 展示所有账户：多态的"高光时刻"
void Bank::showAll() const {
    cout << "\n--- 所有账户 (共 " << accounts.size() << " 个) ---\n";
    if (accounts.empty()) {                  // 空容器卫语句：避免打印空表头
        cout << "（空）\n";
        return;
    }
    // const Account* 遍历：读取指针数组时连"指针本身"都不改（const 正确性）；
    // a->showInfo() 的 const 性由 Account::showInfo() const 签名保证
    for (const Account* a : accounts) {
        a->showInfo();        // ⭐ 多态遍历：每个账户用自己的格式打印
        // 同一行代码，三次不同表现：Normal/VIP/Saving 各自的 showInfo
        // 被虚表（vtable）分发执行 —— 这就是"面向对象的灵魂"。
    }
}

// 保存：一行委托给持久化层（Bank 决定"何时存"，FileManager 决定"怎么存"）
void Bank::saveAll() {
    FileManager::save(dataFile,accounts);
}

// 加载：先释放旧数据再装载新数据（防泄漏 + 防重复）
void Bank::loadAll() {
    // 先释放原有数据
    // 【一致性要点】重复加载时若不先释放，旧对象将永远失联（指针被新 vector
    //   覆盖）→ 经典内存泄漏；这里复刻析构的 delete 循环，保证"先回收再换新"。
    for (Account* a : accounts) delete a;
    accounts.clear();
    // 加载新数据
    // 【所有权交接】load 返回的 vector<Account*> 中，每个堆对象的所有权
    //   移交给 Bank —— 从此由 Bank 的析构负责释放。
    accounts = FileManager::load(dataFile);
}


// 私有辅助：按账号线性查找，返回对象指针；找不到返回 nullptr
// 【设计对比】案例01 的 isExist 返回 int 下标（还要用下标二次访问）；
//   这里直接返回 Account* —— 拿到指针就能调用一切方法，一步到位
//   （nullptr 表不存在，语义与 std::map::find 返回 end() 同源）。
Account *Bank::findAccount(const std::string &id) {
    for (Account* a : accounts) {            // 线性扫描 O(n)（百级账户无压力；十万级该换 map/哈希）
        if (a->getId() == id) {              // const 成员函数调用 + string 内容比较
            return a;                       // 命中即返回该对象的指针
        }
    }
    return nullptr;                          // 扫完未命中：空指针即"不存在"的哨兵
}
