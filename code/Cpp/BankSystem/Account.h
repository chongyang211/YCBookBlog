//
// Created by 杨充 on 2026/5/25.
//
// ============================================================================
// 【文件定位】Account.h —— 实体层·抽象基类 Account（整个继承体系的根）
// ----------------------------------------------------------------------------
// 【设计思路】三种子类（普通/VIP/储蓄）"都是账户"：账号、户主、余额是共性，
//   存取款/展示/序列化是"同名行为、不同规则" —— 共性抽到基类，差异声明为
//   纯虚函数强制度子类实现，这就是"同一接口，不同实现"的运行时多态。
// 【三层架构】本项目分三层，依赖方向单向向下：
//   实体层（Account 体系，本文件）← 管理层（Bank）← 持久化层（FileManager）。
// ============================================================================

#ifndef ACCOUNT_H   // 头文件保护宏：防止重复包含导致类重定义
#define ACCOUNT_H
#include <string>   // std::string 成员依赖
#include <iostream>  // 流头文件（Account.cpp 用到 cout，头文件一并引入）

// Account 是抽象基类，不能直接实例化
// 【技术原理】含至少一个纯虚函数（= 0）的类即抽象类：
//   编译器禁止 new Account(...)，只能通过 Account* 指针引用子类对象 ——
//   这正是多态的载体（Bank 的 vector<Account*> 装三种子类）。
class Account {
protected:  // protected：子类可见、外部不可见
    // 【设计权衡】余额若设为 private，子类实现 withdraw/deposit 就得走 public
    //   接口绕一圈；设为 protected 让子类规则（透支/违约金）能直接读写，
    //   而外部代码依然只能通过 getter 拿到只读视图 —— 封装的边界放在
    //   "家族内部 vs 外部世界"之间。
    std::string accountId;   // 账号
    std::string ownerName;   // 户主姓名
    double balance;          // 余额
    // 【教学提示】double 存金额存在二进制精度问题（0.1 无法精确表示），
    //   生产系统应使用整数"分"或 decimal 库 —— 教学项目从简。

public:
    // 构造函数，子类通过基类构造列表调用
    // 【技术原理】protected 成员对子类"可见"但子类构造函数仍不能在函数体内
    //   直接初始化它们之前的成员？——不，子类必须通过初始化列表显式调用
    //   基类构造：基类子对象总是先于派生类部分构造（C++ 规定的固定顺序）。
    Account(const std::string& id, const std::string& name, double initBalance);
    // 析构函数，为什么要设计成 virtual 虚函数？如果不写成虚函数，子类delete时候，那么就只调基类析构，无法释放子类自己
    // 【技术原理】"通过基类指针 delete 派生类对象"是本项目的日常操作
    //   （vector<Account*> 的析构循环 delete a）—— 若析构非虚，delete 只执行
    //   Account 的析构体，子类新增部分（如 VipAccount 的字符串成员）不释放，
    //   属于未定义行为（资源泄漏）。= default 让编译器生成高效的空实现。
    virtual ~Account() = default;
    // 纯虚函数，子类必须实现
    // 有纯虚函数的类，不可以直接new
    // 【设计动机】四件事每种子类规则都不同：
    //   showInfo  —— 展示字段不同（VIP 多利率、储蓄多到期日）；
    //   withdraw  —— 取款规则不同（普通禁透支 / VIP 透支 1000 / 储蓄扣违约金）；
    //   typeTag   —— 序列化身份标签（CSV 首列），反序列化据此决定 new 谁；
    //   toCsv     —— 字段序列化格式不同（列数与含义随子类变化）。
    virtual void showInfo() const = 0;
    virtual bool withdraw(double amount) = 0;
    // 【接口设计】withdraw 返回 bool 而非 void：失败（余额不足/超额）必须
    //   告知调用方 —— 转账的"先扣后加"事务正是靠这个返回值决定提交或取消。
    virtual char typeTag() const = 0;
    virtual std::string toCsv() const = 0;

    // 普通虚函数，基类有默认实现，自己可以选择性重写
    // 【设计对比·纯虚 vs 普通虚】存钱三种账户规则几乎相同（金额>0 就入账），
    //   没必要逼每个子类都写一遍 —— 基类给默认实现；只有 VIP 有差异
    //   （存款带利息），由 VipAccount 选择性重写。
    //   "纯虚 = 强制契约，普通虚 = 可覆盖的默认" —— 这是接口设计的常用分寸。
    virtual void deposit(double amount);

    // 普通函数，子类共享
    // 【技术原理】返回 const 引用而非值：零拷贝暴露只读视图；
    //   非 virtual：纯查询没有多态需求，还能避免虚表开销（也意味着子类
    //   无法改写"取账号"这种无差异行为——本来就不该有差异）。
    const std::string& getId()      const { return accountId; }
    const std::string& getOwner()   const { return ownerName; }
    double             getBalance() const { return balance; }
};



#endif //ACCOUNT_H  // 头文件保护宏结束
