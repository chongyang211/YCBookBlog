//
// Created by 杨充 on 2026/5/28.
//

#include "Admin.h"
#include "Student.h"
#include "Teacher.h"
#include "CampusSystem.h"



void Admin::mainMenu() {
    while (true) {
        std::cout << "\n--- 管理员 " << userName << " ---\n";
        std::cout << "1. 添加机房  2. 添加用户  3. 数据统计  0. 退出登录\n";
        int op;
        std::cin >> op;
        switch (op) {
            case 1:
                std::cout << "[Admin] addRoom \n";
                int id, cap;
                std::string spec;
                std::cout << "机房编号: ";
                std::cin >> id;
                std::cout << "容量: ";
                std::cin >> cap;
                std::cout << "配置: ";
                std::cin >> spec;
                sys->addRoom(Computer(id, cap, spec));
                break;
            case 2: {
                std::cout << "[Admin] addUser 占位\n";
                int role;
                std::string id, name, pwd;
                std::cout << "身份(1=学生 2=教师): ";
                std::cin >> role;
                std::cout << "账号: ";
                std::cin >> id;
                std::cout << "姓名: ";
                std::cin >> name;
                std::cout << "密码: ";
                std::cin >> pwd;

                std::shared_ptr<User> u;
                if      (role == 1) u = make_shared<Student>(id, name, pwd);
                else if (role == 2) u = make_shared<Teacher>(id, name, pwd);
                else { std::cout << "[Admin] 不允许添加管理员\n"; break; }
                sys->addUser(u);
                break;
            }
            case 3:
                std::cout << "[Admin] statistics \n";
                sys->statistics();
                break;
            case 0: return;
            default: std::cout << "无效选择\n";
        }
    }
}


