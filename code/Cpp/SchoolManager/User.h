//
// Created by 杨充 on 2026/5/28.
//

#ifndef USER_H
#define USER_H
#include <string>
#include <iostream>

// User 类是一个抽象基类，用于定义用户的基本接口。
// 1.支持多态性，允许派生类实现不同的行为。2.拓展性，确保所有用户类型都具有一致的行为。
class User {
protected:
    std::string userId;
    std::string userName;
    std::string password;
public:
    User(const std::string& id, const std::string& name, const std::string& pwd)
        : userId(id), userName(name), password(pwd) {}
    // 虚析构函数，使用 = default 表示使用编译器生成的默认实现。
    // 确保在通过基类指针删除派生类对象时，能够正确调用派生类的析构函数。
    virtual ~User() = default;
    // 纯虚函数，表示 User 类是一个抽象基类，不能直接实例化。派生类必须实现 mainMenu 方法。
    virtual void mainMenu() = 0;
    // 纯虚：返回身份标签 'S'/'T'/'A'，用于 CSV 反序列化（阶段 ⑦ 用）
    virtual char roleTag() const = 0;
    // 校验密码
    bool verify(const std::string& pwd) const { return password == pwd; }

    const std::string& getId()   const { return userId; }
    const std::string& getName() const { return userName; }

    // 序列化为 CSV 行（阶段 ⑦ 才会真正用上）
    virtual std::string toCsv() const {
        return std::string(1, roleTag()) + "," + userId + "," + userName + "," + password;
    }
};



#endif //USER_H
