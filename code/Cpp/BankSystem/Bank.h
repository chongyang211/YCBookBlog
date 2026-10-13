//
// Created by 杨充 on 2026/5/25.
//
// ============================================================================
// 【文件定位】Bank.h —— 管理层：账户容器 + 全部业务逻辑（增存取查转 + 持久化调度）
// ----------------------------------------------------------------------------
// 【设计思路】Bank 是三层架构的中枢：
//   - 向下持有"唯一的数据库" vector<Account*>，对实体层做增查与多态调用；
//   - 再向下把保存/加载委托给 FileManager（Bank 只管"什么时候存"，不管"怎么存"）；
//   - 向上（main）只暴露无参的业务方法 —— 交互采集（cin）与业务执行合一。
// 【已知局限·教学取舍】故意用裸指针 Account* + 手动 delete（RAII 章节未学），
//   析构函数负责兜底释放；综合案例 04 将升级为 std::unique_ptr<Account> 自动回收。
// ============================================================================

#pragma once        // 头文件保护（编译器内置指令，等价于 #ifndef 三件套）

#include "Account.h"    // 容器元素类型：Account 完整定义（vector 存其指针、调用其方法）
#include <vector>       // std::vector：动态数组（对比案例01的定长数组，容量不再写死100）
#include <string>       // std::string


class Bank {
private:
    // 唯一的"数据库"
    // 【技术原理】vector<Account*>：基类指针数组 —— 装三种子类对象的关键。
    //   装指针而非对象值：① Account 是抽象类不能按值存储；
    //   ② 指针 + 虚函数才能在遍历时触发动态分发（多态）；
    //   ③ 但指针意味着"所有权"要自己管（见析构函数的 delete 循环）。
    // 【已修复的进阶方向】vector<unique_ptr<Account>> 可自动释放（案例04）。
    std::vector<Account*> accounts;
    // file文件
    // 【设计思路】数据文件名是 Bank 的"配置"：NSDMI 类内初始化给默认值 "bank"，
    //             保存/加载统一走它，改文件名只改这一处（单一数据源）。
    std::string dataFile = "bank";
    // 查找
    // 【设计动机·findAccount 的诞生】写第二个功能（存款）时发现"按账号找账户"
    //   在取款/查询/转账全都要用 —— 抽成私有辅助函数（同案例01 isExist 的故事）：
    //   返回 Account* 直接可用（比返回下标更方便：拿到指针就能调一切方法）。
    Account* findAccount(const std::string& id);
public:
    // 构造函数
    Bank();
    // 析构函数
    // 【为何必须写】容器里是裸指针 —— vector 析构只释放"指针本身"，
    //   不释放"指针指向的对象"，必须手写析构循环 delete（所有权语义）。
    ~Bank();
    // ===== 业务方法（交互 + 执行一体，方法名即菜单项）=====
    // 【命名说明】Bank::deposit() 无参（自己 cin 采集账号金额），
    //   与 Account::deposit(double) 同名但类域不同、签名不同 —— 不是重载关系也不冲突。
    // 开户
    void openAccount();
    // 存款
    void deposit();
    // 取款
    void withdraw();
    // 查询余额
    void queryBalance();
    // 转账
    void transfer();
    // 展示所有
    void showAll() const;   // const：纯只读遍历，编译器帮我们检查"不改成员"
    // 保存
    void saveAll();
    // 加载
    void loadAll();
};



