//
// Created by 杨充 on 2026/5/28.
//
// ============================================================================
// 【文件定位】Admin.h —— 管理员角色类（User 的子类）
// ----------------------------------------------------------------------------
// 【设计思路】管理员是"运营角色"：添加机房、添加用户、查看全局统计。
//             权力最大（可以创建教师/学生账号），所以业务上"不允许再创建管理员"，
//             防止权限无限扩散 —— 体现最小权限原则。
// 【备注】Admin.h 同时 #include 了 Computer.h，因为 mainMenu 中要
//             构造 Computer 对象传给 addRoom（.cpp 中包含也可以，放头文件更直观）。
// ============================================================================

#ifndef ADMIN_H       // 头文件保护宏：防止重复包含导致类重定义
#define ADMIN_H
#include "Computer.h" // 管理员菜单需要构造 Computer 对象（添加机房），故引入其完整定义
#include "User.h"     // 基类完整定义（继承的前提）
class CampusSystem;         // 前向声明，避免循环 include
// 【技术原理】CampusSystem ↔ User/Student/Teacher/Admin 相互引用形成包含环，
//             头文件里只需要"CampusSystem*"指针，前向声明即可打破环。

class Admin : public User {     // public 继承 User：管理员 is-a 用户
private:
    CampusSystem* sys;      // 反向引用，用来调用业务方法（阶段 ④ 才会用到）
    // 【设计思路】与 Student/Teacher 同构的"控制器回指"：
    //             菜单交互在 Admin::mainMenu，业务逻辑在 CampusSystem。
public:
    // 构造函数：默认参数 a = nullptr，支持"先构造、后 setSystem 注入"
    // 【技术原理】初始化列表先构造基类子对象，再初始化 sys 指针成员。
    Admin(const std::string& id, const std::string& name, const std::string& pwd,
        CampusSystem* a = nullptr)
    : User(id, name, pwd), sys(a) {}

    // 重写基类纯虚函数：管理员的专属菜单（机房/用户/统计）
    // 【技术原理】override 关键字让编译器做签名匹配检查。
    void mainMenu() override;
    // 管理员身份标签 'A'（Admin 首字母），CSV 序列化/反序列化时识别身份
    char roleTag() const override { return 'A'; }
    // 运行期注入系统引用：登录成功后由 main 调用（与其他角色一致）
    void setSystem(CampusSystem* a) { sys = a; }
};



#endif //ADMIN_H     // 头文件保护宏结束
