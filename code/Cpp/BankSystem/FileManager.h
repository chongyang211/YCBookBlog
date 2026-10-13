//
// Created by 杨充 on 2026/5/27.
//
// ============================================================================
// 【文件定位】FileManager.h —— 持久化层：CSV 文件读写（静态工具类）
// ----------------------------------------------------------------------------
// 【设计思路】
//   1. 全 static 方法：FileManager 没有任何成员状态（文件名、账户列表都是
//      参数传进来的），不需要实例 —— "无状态的工具"就做成静态函数集合，
//      调用形如 FileManager::save(...)，免构造、免对象。
//   2. 层间协作：Bank 决定"何时存/何时取"，FileManager 决定"怎么存/怎么取"，
//      文件格式知识（CSV 字段顺序、类型标签含义）全部收敛在这一层 ——
//      将来换成 JSON/SQLite 只改这个文件。
//   3. 所有权约定：load 返回的堆对象指针归调用方（Bank）所有并由其释放 ——
//      "谁接收，谁负责"的裸指针交接契约（教学项目，案例04升级 unique_ptr）。
// ============================================================================

#ifndef FILEMANAGER_H    // 头文件保护宏
#define FILEMANAGER_H
#include "Account.h"     // 操作 Account 体系（调 toCsv / new 子类），需要完整定义
#include <vector>        // std::vector：账户集合的载体
#include <string>        // std::string：文件名


class FileManager {
public:
    // 保存
    // 【技术细节】vector 按值传参（拷贝的是指针数组，开销小但非零）——
    //   更严谨的写法是 const std::vector<Account*>&（零拷贝），教学版从简；
    //   const 修饰的是"vector 与其中的指针"，指针对象本身 Bank 还在用。
    static bool save(const std::string& fileName, const std::vector<Account*> accounts);
    // 获取
    // 【返回值】返回新构造的 vector：每个元素都是 load 里 new 出来的堆对象，
    //   所有权随返回值移交给调用方（Bank::loadAll 接住后负责 delete）。
    static std::vector<Account*> load(const std::string& fileName);
};



#endif //FILEMANAGER_H  // 头文件保护宏结束
