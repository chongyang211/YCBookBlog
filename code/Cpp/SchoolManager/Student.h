//
// Created by 杨充 on 2026/5/28.
//

#ifndef STUDENT_H
#define STUDENT_H

#include "User.h"
class CampusSystem;         // 前向声明，避免循环 include

class Student : public User {
private:
    CampusSystem* sys;      // 反向引用，用来调用业务方法（阶段 ④ 才会用到）
public:
    Student(const std::string& id, const std::string& name, const std::string& pwd,
        CampusSystem* s = nullptr)
    : User(id, name, pwd), sys(s) {}

    void mainMenu() override;
    char roleTag() const override { return 'S'; }
    void setSystem(CampusSystem* s) { sys = s; }
};



#endif //STUDENT_H
