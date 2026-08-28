//
// Created by 杨充 on 2026/5/28.
//

#ifndef TEACHER_H
#define TEACHER_H
#include "User.h"
class CampusSystem;         // 前向声明，避免循环 include

class Teacher : public User{
private:
    CampusSystem* sys;      // 反向引用，用来调用业务方法（阶段 ④ 才会用到）
public:
    Teacher(const std::string& id, const std::string& name, const std::string& pwd,
        CampusSystem* t = nullptr)
    : User(id, name, pwd), sys(t) {}

    void mainMenu() override;
    char roleTag() const override { return 'T'; }
    void setSystem(CampusSystem* t) { sys = t; }
};



#endif //TEACHER_H
