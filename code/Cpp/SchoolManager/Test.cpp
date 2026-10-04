//
// Created by 杨充 on 2026/10/4.
//

#include <iostream>
#include <string>
#include <memory>
using namespace std;

class User {
protected:
    std::string userId;
    std::string userName;
    std::string password;
public:
    User(std::string id, std::string name, std::string pwd)
        : userId(id), userName(name), password(pwd) {}
    virtual ~User() = default;
    virtual void mainMenu() = 0;
    virtual char roleTag() const = 0;
    bool verify(const std::string & pwd) const{
        return password == pwd;
    }
    const std::string& getId() {
        return userId;
    }
    const std::string& getName() {
        return userName;
    }
    virtual std::string toCsv() const {
        return std::string(1, roleTag()) + "," + userId + "," + userName + "," + password;
    }
};

class CampusSystem;
class Student : public User {
private:
    CampusSystem* sys;
public:
    Student(std::string id, std::string name, std::string pwd, CampusSystem* s = nullptr)
        : User(id, name, pwd), sys(s) {}

    void mainMenu() override{
        cout << "\n--- 学生 " << userName << " 已登录（占位菜单，阶段 ④ 实现）---\n";
    }
    char roleTag() const override{
        return 'S';
    }
    void setSystem(CampusSystem* s) {
        sys = s;
    }
};

class Teacher: public User {
private:
    CampusSystem* sys;
public:
    Teacher(std::string id, std::string name, std::string pwd, CampusSystem* s = nullptr)
        : User(id, name, pwd), sys(s) {}
    void mainMenu() override {
        std::cout << "\n--- 老师 " << userName << " 已登录（占位菜单，阶段 ④ 实现）---\n";
    }
};

shared_ptr<User> login() {
    cout << "\n=== 校园系统登录 ===\n";
    cout << "1. 学生  2. 教师  3. 管理员  0. 退出\n";
    cout << "选择身份: ";
    int role; cin >> role;
    if (role == 0) return nullptr;
    string id,pwd;
    cout << "账号: "; cin >> id;
    cout << "密码: "; cin >> pwd;
    switch (role) {
    case 1: return make_shared<Student>(id, "学生" + id, pwd);
    default: cout << "[Login] 占位：模拟登录成功 - role=" << role << " id=" << id << "\n";
        // case 2: return make_shared<Teacher>(...);  ← 下一步再加
        // case 3: return make_shared<Admin>(...);
    }
    return nullptr;   // 阶段 ① 暂时返回空，循环会自然结束
}

// g++ -std==c++17 Test.cpp
// g++ Test.cpp Student.cpp
// g++ -std=c++17 Test.cpp CampusSystem.cpp
// 身份 + 账号 + 密码
int main() {
    while (true) {
        auto user = login();
        if (!user) {
            cout << "再见！\n";
            return 0;
        }
        user->mainMenu();
    }
    return 0;
}