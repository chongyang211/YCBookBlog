//
// Created by 杨充 on 2026/5/28.
//
// ============================================================================
// 【文件定位】Admin.cpp —— 管理员菜单的实现（交互层 / View 层）
// ----------------------------------------------------------------------------
// 【设计思路】管理员是系统运营者，菜单提供三类能力：
//   1. 添加机房（构造 Computer 实体交给业务层入库）；
//   2. 添加用户（根据身份动态创建 Student/Teacher 子类对象 —— 简单工厂模式）；
//   3. 数据统计（只读报表）。
// 【安全设计】身份选择只允许 1=学生/2=教师，选其他一律拒绝创建
//             （防止再创建管理员导致权限扩散 —— 最小权限原则）。
// ============================================================================

#include "Admin.h"     // 本类声明（其中已包含 User.h、Computer.h）
#include "Student.h"   // 添加用户时需要 new Student（完整定义）
#include "Teacher.h"   // 添加用户时需要 new Teacher（完整定义）
#include "CampusSystem.h" // 调用 sys->addRoom/addUser/statistics 需要完整定义



// 管理员主菜单：覆写基类纯虚函数，由虚表分发调用
void Admin::mainMenu() {
    while (true) {      // 事件循环：直到选 0 退出
        std::cout << "\n--- 管理员 " << userName << " ---\n";
        std::cout << "1. 添加机房  2. 添加用户  3. 数据统计  0. 退出登录\n";
        int op;                 // 菜单选项
        std::cin >> op;         // 读入选项
        switch (op) {
            // 【修复说明】case 1 原先没有花括号，直接声明变量会导致
            //             "jump bypasses variable initialization"编译错误：
            //             switch 的 case 标签只是跳转目标，跨过变量初始化直接
            //             跳到后面的 case 是未定义行为，编译器强制禁止；
            //             加花括号把作用域限制在本 case 内即可合法。
            case 1: {           // 添加机房：就地采集三个字段，构造临时 Computer 转交业务层
                std::cout << "[Admin] addRoom \n";
                int id, cap;    // 机房编号、容量
                std::string spec;   // 配置描述
                std::cout << "机房编号: ";
                std::cin >> id;
                std::cout << "容量: ";
                std::cin >> cap;
                std::cout << "配置: ";
                std::cin >> spec;
                // 用聚合参数临时构造 Computer(id, cap, spec)：
                // "栈上构造 → 拷贝入 map → 立即析构"，简洁高效；
                // 查重（编号是否已存在）由业务层 addRoom 内部负责
                sys->addRoom(Computer(id, cap, spec));
                break;
            }
            case 2: {           // 添加用户：简单工厂 —— 按身份选择创建哪个子类
                std::cout << "[Admin] addUser 占位\n";
                int role;               // 身份选择（1=学生 2=教师）
                std::string id, name, pwd;  // 账号、姓名、密码
                std::cout << "身份(1=学生 2=教师): ";
                std::cin >> role;
                std::cout << "账号: ";
                std::cin >> id;
                std::cout << "姓名: ";
                std::cin >> name;
                std::cout << "密码: ";
                std::cin >> pwd;

                // 【技术原理】多态容器的构造方式：基类智能指针指向子类对象
                //   - make_shared<T>(...) 一次堆分配同时容纳对象与控制块（比
                //     shared_ptr<T>(new T) 少一次分配，且异常安全）；
                //   - u 声明为 shared_ptr<User>（基类指针），指向 Student/Teacher
                //     均可 —— 这是"运行时多态"的前提（虚函数通过 vtable 分发）；
                //   - 注意必须写 std::make_shared：不加命名空间前缀会被部分编译器
                //     视为 C++20 扩展（ADL 仅对已声明模板生效），产生告警甚至报错。
                std::shared_ptr<User> u;
                if      (role == 1) u = std::make_shared<Student>(id, name, pwd);   // 创建学生
                else if (role == 2) u = std::make_shared<Teacher>(id, name, pwd);   // 创建教师
                // 【安全设计】管理员不能再创建管理员（权限不可自我复制）；
                //             break 直接放弃本次操作（u 为空也不会误入库）
                else { std::cout << "[Admin] 不允许添加管理员\n"; break; }
                sys->addUser(u);    // 所有权随 shared_ptr 移交给 users map（引用计数共享）
                break;
            }
            case 3:             // 数据统计：只读报表，直接转发
                std::cout << "[Admin] statistics \n";
                sys->statistics();       // 业务层用 accumulate/count_if 聚合各类数据
                break;
            case 0: return;     // 退出登录：结束菜单循环，回到 main 的登录界面
            default: std::cout << "无效选择\n";    // 非法输入防兜底
        }
    }
}


