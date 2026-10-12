//
// Created by 杨充 on 2026/5/28.
//
// ============================================================================
// 【文件定位】Student.h —— 学生角色类（User 的子类）
// ----------------------------------------------------------------------------
// 【设计思路】
//   1. 学生是"被动角色"：浏览机房、预约机房、取消预约、报名演讲，
//      所有业务数据都存在 CampusSystem 里，Student 只负责"菜单交互 + 转发请求"。
//   2. 采用"瘦客户端"模式：子类本身不持有业务数据（无成员变量），
//      避免数据分散在多处导致不一致。
// ============================================================================

#ifndef STUDENT_H     // 头文件保护宏：防止重复包含导致 Student 类重定义
#define STUDENT_H

#include "User.h"     // 必须包含基类头文件：public 继承要求编译器知道 User 的完整定义
class CampusSystem;         // 前向声明，避免循环 include
// 【技术原理】CampusSystem.h 里要用到 User（存在 map 中），
//             而 Student.h 又需要引用 CampusSystem —— 如果两边都互相 #include 对方，
//             就会形成"循环包含"，头文件保护宏会导致其中一方"先到先得"，
//             另一方拿到的是不完整类型（incomplete type）而编译失败。
//             前向声明只告诉编译器"存在这样一个类名"，不暴露其内部定义，
//             足够声明"CampusSystem* 指针"成员（指针大小与类型定义无关）。
// 【代价】前向声明的类型不能被解引用或创建对象（定义不完整），
//         所以只能存放指针/引用，真正的调用要留到 .cpp 中 #include 完整定义之后。

class Student : public User {   // public 继承：实现"is-a"关系（学生是一种用户）
                                // 基类的 public 成员在子类中仍是 public，protected 仍是 protected
private:
    CampusSystem* sys;      // 反向引用，用来调用业务方法（阶段 ④ 才会用到）
    // 【设计思路】这是"控制器回指"模式：UI 层（Student）持有业务层（CampusSystem）的
    //             裸指针，把用户的菜单选择转发给系统对象处理。
    // 【内存安全】此处用裸指针是"非拥有型引用"（CampusSystem 的生命周期由 main 管理），
    //             不构成所有权，因此无需 shared_ptr；sys 悬空风险由程序结构保证
    //             （main 中 CampusSystem 栈对象活得比所有用户会话都久）。
public:
    // 构造函数：参数带默认值 a = nullptr，允许先构造、之后再通过 setSystem 注入系统引用
    // 【技术原理】": User(id, name, pwd), sys(s)" 是初始化列表：
    //             - 先显式调用基类 User 的构造函数初始化继承来的成员；
    //             - 再初始化自己的 sys 指针。列表顺序必须与"基类 → 成员声明顺序"一致。
    //             C++11 起可用 "CampusSystem* s = nullptr"（而非 NULL），类型更安全。
    Student(const std::string& id, const std::string& name, const std::string& pwd,
        CampusSystem* s = nullptr)
    : User(id, name, pwd), sys(s) {}

    // override：显式告诉编译器"我要重写基类的纯虚函数 mainMenu"
    // 【技术原理】编译器会检查基类中确实存在签名一致的虚函数，
    //             拼写错误/签名不匹配会直接报编译错误（不用 override 则只是"定义了新函数"，
    //             多态静默失效，很难排查的 bug）。
    // mainMenu 是纯虚函数的实现，至此 Student 成为可实例化的具体类。
    void mainMenu() override;
    // 实现基类的另一个纯虚函数：学生身份标签 'S'（Student 首字母）
    // 【技术原理】const override：既承诺不修改成员，又显式标记为虚函数重写。
    //             函数体单行 return，内联成本低。
    char roleTag() const override { return 'S'; }
    // 运行期注入系统引用：main.cpp 登录成功后调用它把 &sys 塞进来
    // 【设计思路】构造与装配分离：对象可以在"尚未连接系统"时先创建
    //             （如从 CSV 加载用户时 CampusSystem 可能还在构造中）。
    void setSystem(CampusSystem* s) { sys = s; }
};




#endif //STUDENT_H  // 头文件保护宏结束
