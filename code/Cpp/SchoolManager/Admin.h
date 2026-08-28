//
// Created by 杨充 on 2026/5/28.
//

#ifndef ADMIN_H
#define ADMIN_H
#include "Computer.h"
#include "User.h"
class CampusSystem;         // 前向声明，避免循环 include

class Admin : public User {
private:
    CampusSystem* sys;      // 反向引用，用来调用业务方法（阶段 ④ 才会用到）
public:
    Admin(const std::string& id, const std::string& name, const std::string& pwd,
        CampusSystem* a = nullptr)
    : User(id, name, pwd), sys(a) {}

    void mainMenu() override;
    char roleTag() const override { return 'A'; }
    void setSystem(CampusSystem* a) { sys = a; }
};



#endif //ADMIN_H
