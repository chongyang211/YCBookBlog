#include <iostream>
#include <string>
#include <memory>

#include "Admin.h"
#include "CampusSystem.h"
#include "Computer.h"
#include "Reservation.h"
#include "Speech.h"
#include "Student.h"
#include "Teacher.h"
using namespace std;


shared_ptr<User> login() {
    cout << "\n=== 校园系统登录 ===\n";
    cout << "1. 学生  2. 教师  3. 管理员  0. 退出\n";
    cout << "选择身份: ";
    int role; cin >> role;
    if (role == 0) return nullptr;

    string id, pwd;
    cout << "账号: "; cin >> id;
    cout << "密码: "; cin >> pwd;

    // TODO（阶段 ⑦）: 接 FileStore 校验账号密码
    // （阶段 ②）: 根据 role 返回 Student/Teacher/Admin 对象
    switch (role) {
        case 1: return make_shared<Student>(id, "学生" + id, pwd);
        case 2: return make_shared<Teacher>(id, "教师" + id, pwd);
        case 3: return make_shared<Admin>(id, "管理员" + id, pwd);
    }
    cout << "[Login] 占位：模拟登录成功 - role=" << role << " id=" << id << "\n";
    return nullptr;   // 阶段 ① 暂时返回空，循环会自然结束
}

void test() {

    Computer c(101, 50, "i7+RTX4060");
    Speech s("S001", "AI伦理", 1);
    Reservation r(1, "S001", 101, "2026-06-01");

    cout << "机房: " << c.toCsv() << "\n";
    cout << "演讲: " << s.studentId << " " << s.topic << " round=" << s.round << "\n";
    cout << "预约: #" << r.resId << " 学生 " << r.studentId
         << " 机房 " << r.computerId << " 状态 " << r.statusText() << "\n";
}

// g++ -std=c++17 main.cpp User.cpp Student.cpp Admin.cpp Teacher.cpp Computer.cpp Speech.cpp Reservation.cpp CampusSystem.cpp
int main() {
    CampusSystem sys;
    // ⭐ 临时：手动 add 三个测试用户（阶段 ⑦ 接 FileStore 后会从 CSV 加载）
    sys.addUser(make_shared<Student>("S001", "张三", "123"));
    sys.addUser(make_shared<Teacher>("T001", "李老师", "456"));
    sys.addUser(make_shared<Admin>("A001", "王管理员", "789"));
    while (true) {
        cout << "\n=== 校园系统登录 ===\n";
        cout << "1. 学生  2. 教师  3. 管理员  0. 退出\n";
        cout << "选择身份: ";
        int role; cin >> role;
        if (role == 0) { cout << "再见！\n"; return 0; }
        string id, pwd;
        cout << "账号: "; cin >> id;
        cout << "密码: "; cin >> pwd;

        auto user = sys.login(id, pwd);
        if (!user) {
            continue;
        }
        // ⭐ 关键：让子类拿到 sys 引用（阶段 ⑤ 起 mainMenu 真业务时要用）
        if (auto p = dynamic_pointer_cast<Student>(user)) p->setSystem(&sys);
        else if (auto p = dynamic_pointer_cast<Teacher>(user)) p->setSystem(&sys);
        else if (auto p = dynamic_pointer_cast<Admin>(user))   p->setSystem(&sys);

        user->mainMenu();   // ⭐ 多态分发
    }
    return 0;
}
