//
// Created by 杨充 on 2026/5/25.
//
// ============================================================================
// 【文件定位】NormalAccount.h —— 实体层·普通账户子类（规则最朴素的一种）
// 【设计思路】普通账户 = "基类能干什么我就干什么"：
//   存款直接用基类默认实现（不重写 deposit），取款只加"禁透支"规则，
//   序列化字段 = 基类三件套。它是理解另外两个花哨子类的"参照系"。
// ============================================================================

#ifndef NORMALACCOUNT_H   // 头文件保护宏
#define NORMALACCOUNT_H
#include "Account.h"      // 必须包含基类完整定义：public 继承的前提


class NormalAccount : public Account {   // public 继承：普通账户 is-a 账户
                                         // （is-a 才用 public 继承；is-a 关系是多态的前提）
public:
    // 子类构造函数：自己无新增成员，全部工作就是转发参数给基类构造
    NormalAccount(const std::string& id, const std::string& name, double initBalance);

    // override：显式告知编译器"我在重写基类纯虚函数"，签名不匹配直接编译报错
    // （不加 override 的手误会变成"静默定义了一个新函数"，多态失效难排查）
    void showInfo() const override;
    bool withdraw(double amount) override;
    char typeTag() const override { return 'N'; }   // 类型标签 'N'：CSV 首列，反序列化的"路标"
    std::string toCsv() const override;
    // 【注意】没有重写 deposit —— 直接复用 Account::deposit 的默认实现
    // （普通账户存款无利息），这正是基类"普通虚函数给默认实现"的意义。
};



#endif //NORMALACCOUNT_H  // 头文件保护宏结束
