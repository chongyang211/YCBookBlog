#include <iostream>

#include "Bank.h"
#include "NormalAccount.h"
#include "SavingAccount.h"
#include "VipAccount.h"

using namespace std;

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
int main() {
    Bank bank;
    // 启动时自动加载（如果文件存在）
    bank.loadAll();
    int select = 0;
    while (true) {
        showMenu();
        cin >> select;
        switch (select) {
            case 1:
                cout << "[开户] \n";
                bank.openAccount();
                break;
            case 2:
                cout << "[存款] \n";
                bank.deposit();
                break;
            case 3:
                cout << "[取款] \n";
                bank.withdraw();
                break;
            case 4:
                cout << "[查询余额]\n";
                bank.queryBalance();
                break;
            case 5:
                cout << "[转账]\n";
                bank.transfer();
                break;
            case 6:
                cout << "[显示所有账户]\n";
                bank.showAll();
                break;
            case 7:
                cout << "[保存数据]\n";
                bank.saveAll();
                break;
            case 8:
                cout << "[加载数据]\n";
                bank.loadAll();
                break;
            case 0:
                cout << "感谢使用，再见！\n";
                return 0;
            default:
                cout << "输入有误，请重新选择。\n";
                break;
        }
    }
    return 0;
}
