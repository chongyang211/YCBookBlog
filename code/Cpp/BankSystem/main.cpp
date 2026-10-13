// ============================================================================
// 【文件定位】main.cpp —— 入口层（UI）：菜单展示 + 事件循环分发
// ----------------------------------------------------------------------------
// 【设计思路】main 与案例01同构：菜单骨架 + switch 分发 + 0 退出。
//   差别是业务全部委托给栈上的 Bank 对象 —— main 只做"接线"，
//   业务规则收敛在 Account 体系，流程编排收敛在 Bank。
// ============================================================================

#include <iostream>

#include "Bank.h"             // 管理层：构造 Bank、调用业务方法
#include "NormalAccount.h"    // test1 冒烟测试要直接构造三种子类
#include "SavingAccount.h"
#include "VipAccount.h"

using namespace std;

// 菜单界面：所有功能的统一入口（纯展示，无业务）
void showMenu() {
    cout << "\n*****************************\n";
    cout << "*****  1、开户             *****\n";
    cout << "*****  2、存款             *****\n";
    cout << "*****  3、取款             *****\n";
    cout << "*****  4、查询余额         *****\n";
    cout << "*****  5、转账             *****\n";
    cout << "*****  6、显示所有账户     *****\n";
    cout << "*****  7、保存数据         *****\n";
    cout << "*****  8、加载数据         *****\n";
    cout << "*****  0、退出系统         *****\n";
    cout << "*****************************\n";
    cout << "请选择: ";
}

// 【历史冒烟测试】阶段③/④开发子类与验证多态时的直调测试（现在无人调用，保留作教学对照）：
//   直接构造三种对象各自调 showInfo/deposit/withdraw，用输出肉眼比对规则差异 ——
//   这正是"先让类独立可测，再接入 Bank"的分层开发节奏。
void test1() {
    NormalAccount n("N001","张三",1000);
    VipAccount v("V001", "李四", 5000, 0.05);
    SavingAccount s("S001", "王五", 10000, 12);   // 12 个月定期

    // 直接调用各自的 showInfo()
    n.showInfo();
    v.showInfo();
    s.showInfo();

    // 测试 deposit
    cout << "\n--- 测试存款 ---\n";
    n.deposit(500);
    v.deposit(500);   // VIP 会带利息

    // 测试 withdraw
    cout << "\n--- 测试取款 ---\n";
    n.withdraw(2000);  // ❌ 普通账户不允许透支
    v.withdraw(7000);  // ✅ VIP 允许透支 1000
    s.withdraw(1000);  // ⚠️ 储蓄未到期，扣违约金
}


// g++ -std=c++17 main.cpp Account.cpp NormalAccount.cpp VipAccount.cpp FileManager.cpp SavingAccount.cpp Bank.cpp
// （等价的命令行编译方式；build.sh 用 *.cpp 通配符更省事，CMakeLists 则是工程化方案）

int main() {
    Bank bank;             // 栈上构造管理层：其成员 vector 自动为空，析构负责释放全部账户
    // 启动时自动加载（如果文件存在）
    // 【设计思路】"冷启动恢复"：不强迫用户手选"加载"，程序启动即还原上次现场；
    //   文件不存在时 FileManager::load 只提示并返回空表，系统照常从零开始。
    bank.loadAll();
    int select = 0;
    while (true) {         // 事件循环：每轮打印菜单 → 读选项 → 分发 → 回到菜单
        showMenu();
        cin >> select;
        switch (select) {
            case 1:
                cout << "[开户] \n";
                bank.openAccount();       // 交互 + 工厂开户
                break;
            case 2:
                cout << "[存款] \n";
                bank.deposit();            // 定位 + 多态 deposit（VIP 带利息）
                break;
            case 3:
                cout << "[取款] \n";
                bank.withdraw();           // 定位 + 多态 withdraw（各子类规则）
                break;
            case 4:
                cout << "[查询余额]\n";
                bank.queryBalance();       // 定位 + 多态 showInfo
                break;
            case 5:
                cout << "[转账]\n";
                bank.transfer();           // 双定位 + 先扣后加的简易事务
                break;
            case 6:
                cout << "[显示所有账户]\n";
                bank.showAll();            // 多态遍历（const 方法）
                break;
            case 7:
                cout << "[保存数据]\n";
                bank.saveAll();            // 序列化到 CSV（菜单手动触发，防意外退出丢数据）
                break;
            case 8:
                cout << "[加载数据]\n";
                bank.loadAll();            // 手动重载：先释放旧数据再从文件恢复
                break;
            case 0:
                cout << "感谢使用，再见！\n";
                return 0;                  // 唯一出口：return 触发 bank 析构 → 全部账户自动释放
            default:
                cout << "输入有误，请重新选择。\n";   // 防兜底：非法输入不退出程序
                break;
        }
    }
    return 0;   // 实际不可达（唯一出口在 case 0），写上仅为满足 main 返回惯例
}
