//
// Created by 杨充 on 2026/5/27.
//
// ============================================================================
// 【文件定位】FileManager.cpp —— 持久化层实现：保存（多态 toCsv）+ 加载（工厂 new）
// ----------------------------------------------------------------------------
// 【设计思路】持久化的"两端"：
//   保存端 = 多态序列化：循环调 a->toCsv()，每个子类写自己的字段格式；
//   加载端 = 工厂反序列化：读首列类型标签决定 new 哪个子类 —— "类型标签"
//   就是为此诞生的（对象恢复后需要记住"自己原来是哪种账户"）。
// ============================================================================

#include "FileManager.h"
#include <iostream>
#include <ostream>     // 流基类（iostream 已传递包含，教学惯例显式写出）
#include <fstream>     // ofstream / ifstream：文件流（RAII 对象，析构自动关闭文件）
#include <fstream>     // 【冗余包含】重复写了两遍 —— 无害（头文件有保护）但应删一处
#include <sstream>     // stringstream：字符串流，用于按逗号切分 CSV 行

#include "NormalAccount.h"    // 工厂需要 new 三种子类（完整定义）
#include "SavingAccount.h"
#include "VipAccount.h"
using namespace std;


// 保存：把整个容器序列化成 CSV 文本
bool FileManager::save(const std::string &fileName, const std::vector<Account *> accounts) {
    std::ofstream ofs(fileName);        // 构造即打开文件（覆盖写）；析构时自动 close（RAII）
    if (!ofs.is_open()) {               // 失败守卫：目录不存在/权限不足等
        std::cout << "[FileManager] 打开文件失败: " << fileName << "\n";
        return false;                   // 把失败"说出来"，调用方可以决定后续（如提示用户）
    }
    for (const Account* a : accounts) {
        ofs << a->toCsv() << "\n";          // ⭐ 多态：每个子类生成自己格式的 CSV
        // 每个对象一行文本：Normal 4 列、VIP 5 列、Saving 6 列 —— 行与行之间
        // 列数可以不同，全靠首列类型标签区分（这就是"标签制"持久化的灵活性）
    }
    ofs.close();       // 显式关闭（其实 RAII 下可省 —— 提前落盘、释放句柄，教学写法）
    std::cout << "[FileManager] 已保存 " << accounts.size() << " 个账户到 " << fileName << "\n";
    return true;
}

// 加载：逐行读取 CSV → 按类型标签工厂创建子类对象
std::vector<Account *> FileManager::load(const std::string &fileName) {
    std::vector<Account*> result;      // 本函数构造的结果容器（返回时移交给调用方）
    ifstream ifs(fileName);            // 构造即打开文件（只读）
    if (!ifs.is_open()) {              // 首次运行没有数据文件是常态 → 只提示不报错
        cout << "[FileManager] 文件不存在或打开失败: " << fileName << "\n";
        return result;                 // 返回空 vector（调用方拿到空列表继续运行）
    }

    string line;
    int lineNum = 0;                   // 行号计数：出错时能精确定位到"第几行"
    while (getline(ifs, line)) {       // 逐行读取（自动丢弃行尾换行符）
        lineNum++;
        if (line.empty()) continue;    // 跳过空行（手工编辑过的文件常有空行）

        // 解析 CSV：用 stringstream + getline(',')
        // 【技术原理】把一行文本包成 stringstream 后，getline(ss, token, ',')
        //   复用"流读取"的能力按自定义分隔符切分 —— 比手写 find/substr 循环
        //   简洁且不易越界。注意：字段内若含逗号会切错（教学版不支持转义）。
        stringstream ss(line);
        string token;
        vector<string> fields;        // 本行切出的所有字段
        while (getline(ss, token, ',')) {
            fields.push_back(token);  // 每段塞进字段数组
        }

        if (fields.empty()) continue; // 全空行防御（理论上上面已拦，双保险）

        try {
            char tag = fields[0][0];  // 首列首字符 = 类型标签（'N'/'V'/'S'）
            // 【边界风险】fields[0] 为空串时 [0] 越界 —— 实践中上一行 empty 判断
            //   拦掉了全空行，但 ",x,y" 这种首字段为空的行仍可能触发，属教学盲点。
            Account* acc = nullptr;

            // ⭐ 根据类型标签创建对应子类（这就是简易的"工厂模式"）
            // 【设计要点】"首列标签 + 最低字段数"双重校验：
            //   标签决定 new 谁，字段数防越界访问（fields[4]/fields[5] 必须存在）。
            if (tag == 'N' && fields.size() >= 4) {
                acc = new NormalAccount(
                    fields[1],                        // accountId
                    fields[2],                        // ownerName
                    stod(fields[3]));                 // balance
                // 【技术原理】std::stod：string→double；解析失败抛 invalid_argument
                //   异常（下面的 catch 兜住），比 atof 的"静默返回 0"安全得多。
            }
            else if (tag == 'V' && fields.size() >= 5) {   // VIP 行多一列利率
                acc = new VipAccount(
                    fields[1], fields[2],
                    stod(fields[3]),                  // balance
                    stod(fields[4]));                 // interestRate
            }
            else if (tag == 'S' && fields.size() >= 6) {   // 储蓄行多两列：定期月数+到期时间戳
                acc = new SavingAccount(
                    fields[1], fields[2],
                    stod(fields[3]),                  // balance
                    stoi(fields[4]),                  // term（string→int，失败抛异常）
                    stoll(fields[5]));                // maturityTimestamp（string→long long）
                // 【注意】这里走"构造 ②"（直接给到期时间戳）—— 恢复保存时的
                //   事实数据，而不是按 term 重新推算（见 SavingAccount.cpp 的设计动机）
            }
            else {
                // 行级容错 ①：标签或字段数不对 → 跳过这一行，不影响其他行
                cout << "[FileManager] 第 " << lineNum << " 行格式错误: " << line << "\n";
                continue;
            }

            result.push_back(acc);    // 构造成功的对象交给结果容器
        }
        catch (const exception& e) {
            // 行级容错 ②：stod/stoi 解析失败（如 "abc"）抛异常 → 本行放弃，循环继续
            // 【设计思想】"一行坏了不拖垮整个文件"：坏行报行号，好行照常加载
            cout << "[FileManager] 第 " << lineNum << " 行解析异常: " << e.what() << "\n";
        }
    }
    ifs.close();      // 显式关闭（RAII 下可省）
    cout << "[FileManager] 已从 " << fileName << " 加载 " << result.size() << " 个账户\n";
    return result;    // 返回值携带全部堆对象的所有权 → 移交给调用方 Bank
}

