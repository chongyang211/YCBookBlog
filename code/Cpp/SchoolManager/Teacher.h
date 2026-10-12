//
// Created by 杨充 on 2026/5/28.
//
// ============================================================================
// 【文件定位】Teacher.h —— 教师角色类（User 的子类）
// ----------------------------------------------------------------------------
// 【设计思路】与学生对称：教师是"审批/评审角色"，负责审核机房预约、给演讲评分、
//             查看排名。同样不持有业务数据，只做菜单交互与请求转发。
//             三兄弟（Student/Teacher/Admin）结构几乎一致 —— 这是刻意为之的
//             "平行继承"结构，让新增角色变得模板化。
// ============================================================================

#ifndef TEACHER_H     // 头文件保护宏：防止重复包含
#define TEACHER_H
#include "User.h"     // 包含基类完整定义（继承的前提）
class CampusSystem;         // 前向声明，避免循环 include
// 【技术原理】同 Student.h：CampusSystem 与 User 相互引用形成环，
//             用前向声明打破环。指针成员只需类名，不需要完整类型。

class Teacher : public User{    // public 继承 User：教师 is-a 用户
private:
    CampusSystem* sys;      // 反向引用，用来调用业务方法（阶段 ④ 才会用到）
    // 【设计思路】与 Student 的 sys 完全同构：非拥有型裸指针，
    //             生命周期由 main 中的栈对象 sys 保证不悬空。
public:
    // 构造函数：默认参数 t = nullptr 允许"先创建、后装配"
    // 【技术原理】初始化列表 ": User(...), sys(t)"：
    //             基类子对象先构造 → 派生类成员后构造（C++ 标准规定的固定顺序），
    //             与列表中书写的顺序无关（乱写会有编译器警告）。
    Teacher(const std::string& id, const std::string& name, const std::string& pwd,
        CampusSystem* t = nullptr)
    : User(id, name, pwd), sys(t) {}

    // 重写基类纯虚函数：教师的专属菜单（审核/评分/排名）
    // 【技术原理】override 做编译期签名校验，防止"以为重写了其实没有"的隐性 bug。
    void mainMenu() override;
    // 教师身份标签 'T'（Teacher 首字母），用于 CSV 序列化/反序列化时识别类型
    char roleTag() const override { return 'T'; }
    // 运行期注入系统引用：登录成功后由 main 调用
    void setSystem(CampusSystem* t) { sys = t; }
};




#endif //TEACHER_H   // 头文件保护宏结束
