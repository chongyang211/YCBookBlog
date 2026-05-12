//
// 银行管理系统 - 合并版本实现
// Created by 杨充 on 2025/10/13
//

#include "BankManagement.h"

// ==================== Account类实现 ====================

Account::Account(const std::string& accNumber, const std::string& accName, double initialBalance) 
    : accountNumber(accNumber), name(accName), balance(initialBalance) {
}

std::string Account::getAccountNumber() const {
    return accountNumber;
}

std::string Account::getName() const {
    return name;
}

double Account::getBalance() const {
    return balance;
}

void Account::deposit(double amount) {
    if (amount > 0) {
        balance += amount;
        std::cout << "存款成功！当前余额: " << balance << std::endl;
    } else {
        std::cout << "存款金额必须大于 0！" << std::endl;
    }
}

void Account::withdraw(double amount) {
    if (amount > 0 && amount <= balance) {
        balance -= amount;
        std::cout << "取款成功！当前余额: " << balance << std::endl;
    } else {
        std::cout << "取款金额无效或余额不足！" << std::endl;
    }
}

bool Account::transfer(Account& targetAccount, double amount) {
    if (amount > 0 && amount <= balance) {
        balance -= amount;
        targetAccount.deposit(amount);
        std::cout << "转账成功！当前余额: " << balance << std::endl;
        return true;
    } else {
        std::cout << "转账金额无效或余额不足！" << std::endl;
        return false;
    }
}

void Account::display() const {
    std::cout << "账户号: " << accountNumber 
              << ", 姓名: " << name 
              << ", 余额: " << balance << std::endl;
}

// ==================== BankManagementSystem类实现 ====================

BankManagementSystem::BankManagementSystem(const std::string& filename) 
    : dataFileName(filename) {
    std::cout << "银行管理系统初始化完成！" << std::endl;
}

void BankManagementSystem::displayMenu() {
    std::cout << "\n===============================\n";
    std::cout << "        银行账户管理系统        \n";
    std::cout << "===============================\n";
    std::cout << "1. 开户\n";
    std::cout << "2. 存款\n";
    std::cout << "3. 取款\n";
    std::cout << "4. 查询余额\n";
    std::cout << "5. 转账\n";
    std::cout << "6. 显示所有账户\n";
    std::cout << "7. 保存数据到文件\n";
    std::cout << "8. 从文件加载数据\n";
    std::cout << "9. 清空所有账户\n";
    std::cout << "0. 退出系统\n";
    std::cout << "===============================\n";
    std::cout << "当前账户数量: " << accounts.size() << "\n";
    std::cout << "请选择操作: ";
}

Account* BankManagementSystem::findAccount(const std::string& accNumber) {
    for (auto& account : accounts) {
        if (account.getAccountNumber() == accNumber) {
            return &account;
        }
    }
    return nullptr;
}

void BankManagementSystem::pauseScreen() {
    std::cout << "\n按回车键继续...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void BankManagementSystem::run() {
    int choice;
    
    while (true) {
        displayMenu();
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                printf("=== 开户服务 ===\n");
                openAccount();
                break;
            case 2:
                printf("=== 存款服务 ===\n");
                deposit();
                break;
            case 3:
                printf("=== 取款服务 ===\n");
                withdraw();
                break;
            case 4:
                printf("=== 余额查询 ===\n");
                checkBalance();
                break;
            case 5:
                printf("=== 转账服务 ===\n");
                transfer();
                break;
            case 6:
                printf("=== 账户列表 ===\n");
                displayAllAccounts();
                break;
            case 7:
                printf("=== 保存数据 ===\n");
                saveToFile();
                break;
            case 8:
                printf("=== 加载数据 ===\n");
                loadFromFile();
                break;
            case 9:
                printf("=== 清空账户 ===\n");
                clearAllAccounts();
                break;
            case 0:
                printf("=== 退出系统 ===\n");
                std::cout << "感谢使用银行管理系统，再见！\n";
                return;
            default:
                std::cout << "无效选择，请重试！\n";
                break;
        }
        pauseScreen();
    }
}

void BankManagementSystem::openAccount() {
    std::string accNumber, name;
    double initialBalance;
    
    std::cout << "请输入账户号: ";
    std::getline(std::cin, accNumber);
    
    // 检查账户号是否已存在
    if (findAccount(accNumber) != nullptr) {
        std::cout << "账户号已存在，请使用其他账户号！\n";
        return;
    }
    
    std::cout << "请输入姓名: ";
    std::getline(std::cin, name);
    
    std::cout << "请输入初始余额: ";
    std::cin >> initialBalance;
    
    if (initialBalance < 0) {
        std::cout << "初始余额不能为负数！\n";
        return;
    }
    
    createAccount(accNumber, name, initialBalance);
}

void BankManagementSystem::deposit() {
    std::string accNumber;
    double amount;
    
    std::cout << "请输入账户号: ";
    std::getline(std::cin, accNumber);
    
    std::cout << "请输入存款金额: ";
    std::cin >> amount;
    
    depositToAccount(accNumber, amount);
}

void BankManagementSystem::withdraw() {
    std::string accNumber;
    double amount;
    
    std::cout << "请输入账户号: ";
    std::getline(std::cin, accNumber);
    
    std::cout << "请输入取款金额: ";
    std::cin >> amount;
    
    withdrawFromAccount(accNumber, amount);
}

void BankManagementSystem::checkBalance() {
    std::string accNumber;
    
    std::cout << "请输入账户号: ";
    std::getline(std::cin, accNumber);
    
    queryBalance(accNumber);
}

void BankManagementSystem::transfer() {
    std::string fromAccNumber, toAccNumber;
    double amount;
    
    std::cout << "请输入转出账户号: ";
    std::getline(std::cin, fromAccNumber);
    
    std::cout << "请输入转入账户号: ";
    std::getline(std::cin, toAccNumber);
    
    std::cout << "请输入转账金额: ";
    std::cin >> amount;
    
    transferBetweenAccounts(fromAccNumber, toAccNumber, amount);
}

void BankManagementSystem::displayAllAccounts() {
    if (accounts.empty()) {
        std::cout << "暂无账户信息！\n";
        return;
    }
    
    std::cout << "所有账户信息：\n";
    std::cout << "----------------------------------------\n";
    for (const auto& account : accounts) {
        account.display();
    }
    std::cout << "----------------------------------------\n";
    std::cout << "总账户数: " << accounts.size() << "\n";
}

void BankManagementSystem::clearAllAccounts() {
    char confirm;
    std::cout << "确定要清空所有账户吗？(y/n): ";
    std::cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        accounts.clear();
        std::cout << "所有账户已清空！\n";
    } else {
        std::cout << "操作已取消。\n";
    }
}

void BankManagementSystem::saveToFile() {
    std::ofstream ofs(dataFileName);
    
    if (!ofs.is_open()) {
        std::cout << "无法打开文件进行保存！\n";
        return;
    }
    
    for (const auto& account : accounts) {
        ofs << account.getAccountNumber() << ","
            << account.getName() << ","
            << account.getBalance() << "\n";
    }
    
    ofs.close();
    std::cout << "数据保存成功到文件: " << dataFileName << "\n";
}

void BankManagementSystem::loadFromFile() {
    std::ifstream ifs(dataFileName);
    
    if (!ifs.is_open()) {
        std::cout << "无法打开文件进行加载！\n";
        return;
    }
    
    accounts.clear(); // 清空现有账户
    std::string line;
    int loadCount = 0;
    
    while (std::getline(ifs, line)) {
        std::istringstream iss(line);
        std::string accNumber, name, balanceStr;
        
        if (std::getline(iss, accNumber, ',') &&
            std::getline(iss, name, ',') &&
            std::getline(iss, balanceStr)) {
            
            try {
                double balance = std::stod(balanceStr);
                accounts.emplace_back(accNumber, name, balance);
                loadCount++;
            } catch (const std::exception& e) {
                std::cout << "解析账户数据失败: " << line << "\n";
            }
        }
    }
    
    ifs.close();
    std::cout << "数据加载成功！共加载 " << loadCount << " 个账户。\n";
}

// 银行业务核心方法实现
void BankManagementSystem::createAccount(const std::string& accNumber, const std::string& accName, double initialBalance) {
    accounts.emplace_back(accNumber, accName, initialBalance);
    std::cout << "开户成功！账户号: " << accNumber << ", 姓名: " << accName << "\n";
}

void BankManagementSystem::depositToAccount(const std::string& accNumber, double amount) {
    Account* account = findAccount(accNumber);
    if (account != nullptr) {
        account->deposit(amount);
    } else {
        std::cout << "账户不存在！\n";
    }
}

void BankManagementSystem::withdrawFromAccount(const std::string& accNumber, double amount) {
    Account* account = findAccount(accNumber);
    if (account != nullptr) {
        account->withdraw(amount);
    } else {
        std::cout << "账户不存在！\n";
    }
}

void BankManagementSystem::queryBalance(const std::string& accNumber) {
    Account* account = findAccount(accNumber);
    if (account != nullptr) {
        std::cout << "账户余额: " << account->getBalance() << std::endl;
    } else {
        std::cout << "账户不存在！\n";
    }
}

void BankManagementSystem::transferBetweenAccounts(const std::string& fromAccNumber, const std::string& toAccNumber, double amount) {
    Account* fromAccount = findAccount(fromAccNumber);
    Account* toAccount = findAccount(toAccNumber);
    
    if (fromAccount && toAccount) {
        if (fromAccNumber == toAccNumber) {
            std::cout << "不能向自己的账户转账！\n";
            return;
        }
        fromAccount->transfer(*toAccount, amount);
    } else {
        std::cout << "转出或转入账户不存在！\n";
    }
}

size_t BankManagementSystem::getAccountCount() const {
    return accounts.size();
}

// g++ BankManagement.cpp
// ==================== 主函数 ====================
int main() {
    std::cout << "欢迎使用银行管理系统！\n";
    
    // 创建银行管理系统实例
    BankManagementSystem bankSystem("accounts.txt");
    
    // 运行系统
    bankSystem.run();
    
    return 0;
}