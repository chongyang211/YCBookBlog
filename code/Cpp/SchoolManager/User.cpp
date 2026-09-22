//
// Created by 杨充 on 2026/5/28.
//

#include "User.h"

// TODO：为什么这样写构造函数？
User::User(const std::string& id, const std::string& name, const std::string& pwd)
    : userId(id), userName(name), password(pwd) {
    cout << "[User] 创建账户 " << id << " - " << name << endl;
}


// TODO：把其他实现都给写下