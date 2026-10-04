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
    char roleTag() const override { return 'T'; }
    void setSystem(CampusSystem* t) { sys = t; }
};

class Admin: public User {
private:
    CampusSystem* sys;
public:
    Admin(std::string id, std::string name, std::string pwd, CampusSystem* s = nullptr)
        : User(id, name, pwd), sys(s) {}
    void mainMenu() override {
        std::cout << "\n--- 管理元 " << userName << " 已登录（占位菜单，阶段 ④ 实现）---\n";
    }
    char roleTag() const override { return 'A'; }
    void setSystem(CampusSystem* t) { sys = t; }
};

class Computer {
public:
    int id;
    int capacity;
    std::string spec;
    Computer() = default;
    Computer(int id, int capacity, std::string spec):
        id(id), capacity(capacity), spec(spec) {}
    std::string toCsv() const {
        return std::to_string(id) + "," + std::to_string(capacity) + "," + spec;
    }
    static Computer fromCsv(const std::string & line) {
        return Computer{};
    }
};

class Speech {
public:
    std::string studentId;
    std::string topic;
    int round = 1;
    double score;
    Speech() = default;
    Speech(const std::string& sid, const std::string& t, int r)
          : studentId(sid), topic(t), round(r) {}
};

// 枚举
enum class ResStatus { Pending, Approved, Rejected, Cancelled };

class Reservation {
public:
    int resId;
    std::string studentId;
    int computerId;
    std::string date;
    ResStatus status = ResStatus::Pending;
    Reservation() = default;
    Reservation(int rid, const std::string& sid, int cid, const std::string& d)
          : resId(rid), studentId(sid), computerId(cid), date(d) {}
    std::string statusText() const {
        switch (status) {
            case ResStatus::Pending:   return "待审核";
            case ResStatus::Approved:  return "已批准";
            case ResStatus::Rejected:  return "已拒绝";
            case ResStatus::Cancelled: return "已取消";
        }
        return "未知";
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
    case 2: return make_shared<Teacher>(id, "教师" + id, pwd);
    case 3: return make_shared<Admin>(id, "管理员" + id, pwd);
    default: cout << "[Login] 占位：模拟登录成功 - role=" << role << " id=" << id << "\n";
        // case 2: return make_shared<Teacher>(...);  ← 下一步再加
        // case 3: return make_shared<Admin>(...);
    }
    return nullptr;   // 阶段 ① 暂时返回空，循环会自然结束
}

void test() {
    Computer c(101, 50, "i7+RTX4060");
    cout << "[Test] 机房 " << c.id << " 容量 " << c.capacity << " 配置 " << c.spec << "\n";
    cout << "[Test] toCsv: " << c.toCsv() << "\n";
    Speech s("S001", "AI伦理", 1);
    Reservation r(1, "S001", 101, "2026-06-01");

    cout << "机房: " << c.toCsv() << "\n";
    cout << "演讲: " << s.studentId << " " << s.topic << " round=" << s.round << "\n";
    cout << "预约: #" << r.resId << " 学生 " << r.studentId
         << " 机房 " << r.computerId << " 状态 " << r.statusText() << "\n";
}

// g++ -std==c++17 Test.cpp
// g++ Test.cpp Student.cpp
// g++ -std=c++17 Test.cpp CampusSystem.cpp Computer.cpp
// 身份 + 账号 + 密码
int main() {
    test();
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