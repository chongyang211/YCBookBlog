//
// Created by 杨充 on 2026/5/27.
//

#include "FileManager.h"
#include <iostream>
#include <ostream>
#include <fstream>
#include <fstream>
#include <sstream>

#include "NormalAccount.h"
#include "SavingAccount.h"
#include "VipAccount.h"
using namespace std;


bool FileManager::save(const std::string &fileName, const std::vector<Account *> accounts) {
    std::ofstream ofs(fileName);
    if (!ofs.is_open()) {
        std::cout << "[FileManager] 打开文件失败: " << fileName << "\n";
        return false;
    }
    for (const Account* a : accounts) {
        ofs << a->toCsv() << "\n";          // ⭐ 多态：每个子类生成自己格式的 CSV
    }
    ofs.close();
    std::cout << "[FileManager] 已保存 " << accounts.size() << " 个账户到 " << fileName << "\n";
    return true;
}

std::vector<Account *> FileManager::load(const std::string &fileName) {
    std::vector<Account*> result;
    ifstream ifs(fileName);
    if (!ifs.is_open()) {
        cout << "[FileManager] 文件不存在或打开失败: " << fileName << "\n";
        return result;
    }

    string line;
    int lineNum = 0;
    while (getline(ifs, line)) {
        lineNum++;
        if (line.empty()) continue;

        // 解析 CSV：用 stringstream + getline(',')
        stringstream ss(line);
        string token;
        vector<string> fields;
        while (getline(ss, token, ',')) {
            fields.push_back(token);
        }

        if (fields.empty()) continue;

        try {
            char tag = fields[0][0];
            Account* acc = nullptr;

            // ⭐ 根据类型标签创建对应子类（这就是简易的"工厂模式"）
            if (tag == 'N' && fields.size() >= 4) {
                acc = new NormalAccount(
                    fields[1],                        // accountId
                    fields[2],                        // ownerName
                    stod(fields[3]));                 // balance
            }
            else if (tag == 'V' && fields.size() >= 5) {
                acc = new VipAccount(
                    fields[1], fields[2],
                    stod(fields[3]),                  // balance
                    stod(fields[4]));                 // interestRate
            }
            else if (tag == 'S' && fields.size() >= 6) {
                acc = new SavingAccount(
                    fields[1], fields[2],
                    stod(fields[3]),                  // balance
                    stoi(fields[4]),                  // term
                    stoll(fields[5]));                // maturityTimestamp
            }
            else {
                cout << "[FileManager] 第 " << lineNum << " 行格式错误: " << line << "\n";
                continue;
            }

            result.push_back(acc);
        }
        catch (const exception& e) {
            cout << "[FileManager] 第 " << lineNum << " 行解析异常: " << e.what() << "\n";
        }
    }
    ifs.close();
    cout << "[FileManager] 已从 " << fileName << " 加载 " << result.size() << " 个账户\n";
    return result;
}


