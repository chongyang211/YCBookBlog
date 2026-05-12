#include "BankUserManager.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <limits>
#include <cstdio>
#include <stdio.h>

#include "Bank.h"
#include "FileManager.h"

//执行指令：g++ Account.cpp Bank.cpp FileManager.cpp BankUserManager.cpp -o BankUserManager

Bank bank;

int main() {
    showMain();
    return 0;
}

void displayMenu() {
    std::cout << "\n银行账户管理系统\n";
    std::cout << "1. 开户\n";
    std::cout << "2. 存款\n";
    std::cout << "3. 取款\n";
    std::cout << "4. 查询余额\n";
    std::cout << "5. 转账\n";
    std::cout << "6. 显示所有账户\n";
    std::cout << "7. 保存数据到文件\n";
    std::cout << "8. 从文件加载数据\n";
    std::cout << "0. 退出\n";
    std::cout << "请选择操作: ";
}

int showMain() {
    std::string filename = "accounts.txt";
    while (true) {
        displayMenu();
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice) {
            case 1: {
                printf("1. 开户\n");
                openAccount();
                break;
            }
            case 2: {
                printf("2. 存款\n");
                deposit();
                break;
            }
            case 3: {
                printf("3. 取款\n");
                withdraw();
                break;
            }
            case 4: {
                printf("4. 查询余额\n");
                checkBalance();
                break;
            }
            case 5: {
                printf("5. 转账\n");
                transfer();
                break;
            }
            case 6: {
                printf("6. 显示所有账户\n");
                bank.displayAllAccounts();
                break;
            }
            case 7: {
                printf("7. 保存数据到文件\n");
                // FileManager::saveToFile(bank, FILE_NAME);
                FileManager::saveToFile(bank, filename);
                break;
            }
            case 8: {
                printf("8. 从文件加载数据\n");
                FileManager::loadFromFile(bank, filename);
                bank.displayAllAccounts();
                break;
            }
            case 0: {
                printf("0. 退出\n");
                std::cout << "退出系统。\n";
                return 0;
            }
            default: {
                std::cout << "无效选择，请重试！\n";
                break;
            }
        }
    }
    return 0;
}

// 开户
// 数据流向：用户输入 → 局部变量 → Bank对象 → Account对象创建
void openAccount() {
    // 1. 声明局部变量存储用户输入
    std::string accNumber, name;  // 账户号和姓名（字符串类型）
    double initialBalance;        // 初始余额（浮点数类型）
    
    // 2. 获取账户号
    std::cout << "请输入账户号: ";
    std::getline(std::cin, accNumber);  // 使用getline读取整行，支持空格
    if (bank.findAccount(accNumber) != nullptr) {
        std::cout << "账户号已存在！" << std::endl;
        return;
    }
    
    // 3. 获取用户姓名  
    std::cout << "请输入姓名: ";
    std::getline(std::cin, name);       // 同样使用getline，支持中文姓名

    // 4. 获取初始余额
    std::cout << "请输入初始余额: ";
    std::cin >> initialBalance;         // 使用>>操作符读取数字
    while (!(std::cin >> initialBalance) || initialBalance < 0) {
        std::cout << "请输入有效的非负数: ";
        std::cin.clear();  // 清除错误标志
        std::cin.ignore(10000, '\n');  // 清除错误输入
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // 5. 调用银行系统创建账户
    bank.createAccount(accNumber, name, initialBalance);
}

// 存款
// 用户输入 → deposit()方法 → Bank::deposit() → Account::deposit() → 更新余额
void deposit() {
    // 1. 声明局部变量
    std::string accNumber;  // 存储账户号（字符串类型）
    double amount;          // 存储存款金额（双精度浮点数）
    // 2. 获取账户号
    std::cout << "请输入账户号:";
    std::getline(std::cin , accNumber);  // 读取整行，支持包含空格的账户号
    // 3. 获取存款金额
    std::cout << "请输入存款金额:";
    std::cin >> amount;                  // 读取数值类型
    // 4. 委托给银行系统处理
    bank.deposit(accNumber, amount);     // 调用Bank类的deposit方法
}

// 取款
void withdraw() {
    std::string accNumber;
    double amount;
    std::cout << "请输入账户号: ";
    std::getline(std::cin, accNumber);
    std::cout << "请输入取款金额: ";
    std::cin >> amount;
    bank.withdraw(accNumber, amount);
}

// 查询余额
void checkBalance() {
    std::string accNumber;
    std::cout << "请输入账户号: ";
    std::getline(std::cin, accNumber);
    bank.queryBalance(accNumber);
}

// 用户转账
void transfer() {
    // 定义转入和转出账号
    std::string fromAccNumber, toAccNumber;
    //转出金额
    double amount;
    std::cout << "请输入转出账户号: ";
    std::getline(std::cin, fromAccNumber);
    std::cout << "请输入转入账户号: ";
    std::getline(std::cin, toAccNumber);
    std::cout << "请输入转账金额: ";
    std::cin >> amount;
    bank.transfer(fromAccNumber, toAccNumber, amount);
}