//
// 【文件定位】main.cpp —— 程序入口（组装层 / App 层）
// ----------------------------------------------------------------------------
// 【设计思路】main 只做三件事：
//   1. 构造唯一的 CampusSystem（业务中枢）并播种测试数据；
//   2. 死循环扮演"登录网关"：身份选择 → 账号密码 → sys.login 校验；
//   3. 登录成功后给用户对象注入 sys 引用（setSystem），再通过
//      user->mainMenu() 触发多态分发 —— 三种角色进入各自的菜单世界。
// ============================================================================

#include <iostream>     // cin / cout 标准输入输出
#include <string>       // std::string（账号、密码）
#include <memory>       // std::shared_ptr / make_shared（多态对象的智能指针管理）

#include "Admin.h"      // 管理员角色（构造种子用户时用到）
#include "CampusSystem.h"   // 业务中枢（必须完整定义，因为要构造其栈对象）
#include "Computer.h"       // 预约/测试时构造机房实体
#include "Reservation.h"    // test() 中构造预约单
#include "Speech.h"         // test() 中构造演讲报名
#include "Student.h"        // 学生角色
#include "Teacher.h"        // 教师角色
using namespace std;    // 教学简化：省略 std:: 前缀（生产代码不推荐）


// 【历史遗留】阶段 ② 的旧版登录函数（占位）：已被 main 中的 sys.login 流程取代，不再被调用
shared_ptr<User> login() {
    cout << "\n=== 校园系统登录 ===\n";          // 打印身份选择菜单
    cout << "1. 学生  2. 教师  3. 管理员  0. 退出\n";
    cout << "选择身份: ";
    int role; cin >> role;       // 读取身份编号
    if (role == 0) return nullptr;   // 0 = 用户主动退出，返回空指针交由调用方结束程序

    string id, pwd;              // 账号与密码
    cout << "账号: "; cin >> id;     // 读取账号（遇空白截断）
    cout << "密码: "; cin >> pwd;    // 读取密码（控制台回显，教学项目未做遮蔽）

    // TODO（阶段 ⑦）: 接 FileStore 校验账号密码
    // （阶段 ②）: 根据 role 返回 Student/Teacher/Admin 对象
    // switch (role) {
    //     case 1: return make_shared<Student>(id, "学生" + id, pwd);
    //     case 2: return make_shared<Teacher>(id, "教师" + id, pwd);
    //     case 3: return make_shared<Admin>(id, "管理员" + id, pwd);
    // }
    cout << "[Login] 占位：模拟登录成功 - role=" << role << " id=" << id << "\n";
    return nullptr;   // 阶段 ① 暂时返回空，循环会自然结束
}

// 【历史遗留】阶段 ① 的冒烟测试：直接构造三个实体并打印，验证数据模型能跑通
void test() {
    Computer c(101, 50, "i7+RTX4060");           // 机房：编号 101、容量 50、配置描述
    Speech s("S001", "AI伦理", 1);               // 演讲：S001 报名第 1 轮，主题"AI伦理"
    Reservation r(1, "S001", 101, "2026-06-01"); // 预约：1 号单，S001 订 101 机房

    cout << "机房: " << c.toCsv() << "\n";       // 验证 Computer::toCsv 序列化
    cout << "演讲: " << s.studentId << " " << s.topic << " round=" << s.round << "\n";    // 验证 Speech 字段
    cout << "预约: #" << r.resId << " 学生 " << r.studentId       // 验证 Reservation 字段与状态翻译
         << " 机房 " << r.computerId << " 状态 " << r.statusText() << "\n";
}

// g++ -std=c++17 main.cpp User.cpp Student.cpp Admin.cpp Teacher.cpp Computer.cpp Speech.cpp Reservation.cpp CampusSystem.cpp
// （等价的命令行编译方式，与 build.sh 内容一致；使用 CMake 时无需手敲）

// 程序入口：操作系统加载可执行文件后从这里开始执行
int main() {
    CampusSystem sys;       // 栈上构造业务中枢：四大容器自动初始化为空，nextResId=1
    // ⭐ 临时：手动 add 三个测试用户（阶段 ⑦ 接 FileStore 后会从 CSV 加载）
    // 【技术原理】make_shared<Student>(...)：堆上创建 Student 并交给 shared_ptr<User>
    //             （基类指针指向派生类对象 → 多态的前提）；随后按值传给 addUser，
    //             map 内部再拷贝一份 shared_ptr，引用计数共享同一对象。
    sys.addUser(make_shared<Student>("S001", "张三", "123"));       // 种子学生账号
    sys.addUser(make_shared<Teacher>("T001", "李老师", "456"));     // 种子教师账号
    sys.addUser(make_shared<Admin>("A001", "王管理员", "789"));     // 种子管理员账号
    while (true) {          // 主循环 = 登录网关：退出唯一出口是身份选择 0
        cout << "\n=== 校园系统登录 ===\n";
        cout << "1. 学生  2. 教师  3. 管理员  0. 退出\n";
        cout << "选择身份: ";
        int role; cin >> role;              // 读取身份（身份本身不参与校验，账号密码才是凭证）
        if (role == 0) { cout << "再见！\n"; return 0; }    // 0 → 优雅退出：return 0 表示正常结束
        string id, pwd;                     // 登录凭证
        cout << "账号: "; cin >> id;        // 读账号
        cout << "密码: "; cin >> pwd;       // 读密码

        // 登录校验：账号存在且密码正确则返回多态句柄，否则返回 nullptr
        auto user = sys.login(id, pwd);
        if (!user) {
            continue;       // 登录失败：回到循环开头重新选择身份（不退出程序）
        }
        // ⭐ 关键：让子类拿到 sys 引用（阶段 ⑤ 起 mainMenu 真业务时要用）
        // 【技术原理】dynamic_pointer_cast<T>(ptr)（C++11）：
        //   - 对 shared_ptr 做安全的向下转型（等价于 dynamic_cast<T*>+包装）；
        //   - 运行时借助 RTTI 检查对象真实类型，匹配成功返回目标类型智能指针，
        //     失败返回空指针 —— 用 if 语句的模式同时完成"判型 + 取指针"。
        // 【设计说明】mainMenu 是纯虚函数，理论上可以直接调用；
        //             此处逐类型 cast 是为了把 &sys 注入给对应子类的 sys 成员，
        //             让菜单实现里能转发调用业务方法（控制器装配）。
        if (auto p = dynamic_pointer_cast<Student>(user)) p->setSystem(&sys);       // 是学生 → 注入系统引用
        else if (auto p = dynamic_pointer_cast<Teacher>(user)) p->setSystem(&sys);  // 是教师 → 注入系统引用
        else if (auto p = dynamic_pointer_cast<Admin>(user))   p->setSystem(&sys);  // 是管理员 → 注入系统引用

        user->mainMenu();   // ⭐ 多态分发
        // 【技术原理】同一行代码，实际执行的是 Student/Teacher/Admin 中
        //             某个具体版本的 mainMenu —— 由虚表（vtable）在运行期决定。
        //             mainMenu 内部自循环，直到用户选 0 退出登录，控制流回到此处。
    }
    return 0;   // 循环唯一出口在 role==0 分支已 return，此行仅为满足 main 有返回值的惯例
}

// ============================================================================
// 【以下为设计笔记（原稿手写示意代码，不可编译，故用 #if 0 屏蔽以保留思路）】
// ----------------------------------------------------------------------------
// 主题：为什么"不用多态、每个角色各写一个类"是坏味道？
// 笔记内容指出的三大问题：
//   1. 属性重复：id/name/pwd 在每个类里写了一遍，没有抽取到基类复用；
//   2. 违背开闭原则：每加一种角色都要改 login 的 if-else 分支
//      （对修改开放了，而正确姿势是"对修改关闭、对扩展开放"——新增子类即可）；
//   3. 返回具体类型导致调用方必须知道所有类型细节。
// 反例正解：本项目的 User 抽象基类 + 纯虚 mainMenu + roleTag 的设计正是这些
//           问题的答案 —— 增加角色只需"派生新类 + 登录处多一行 cast"，核心逻辑零改动。
// ============================================================================
#if 0    // 预处理器直接跳过本段（不参与编译），仅作为文档留存

class User {
private:
    std::string id,       // 反例：成员声明语法错误（逗号后不能重复类型），且缺少结尾分号
    std::string name,
    std::string pwd;

}                        // 反例：类定义后缺少分号

// 派生类
class Student : public User {
public:
    void studentMenu();  // 反例：各写各的菜单方法，没有统一接口，无法多态调用
}

class Teacher : public User {
public:
    void studentMenu();
}

class Speech : public User{      // 反例：Speech 与"用户"没有 is-a 关系，此处继承语义错误
public:
    void studentMenu();
}

// 返回多种类型
// 属性，写了多遍，没有进行抽取复用
// 每添加一种类型，都要改login方法逻辑，违背开闭原则【对修改关闭，对拓展开放】
User login() {           // 反例：返回抽象类型按值返回（抽象类不能实例化，无法编译）
    User* user;          // 裸指针 + 手工 new，缺少虚析构与所有权管理（内存泄漏风险）
    if (role == 1) {     // role 未定义；类型分支硬编码，违背开闭原则
        user = new Student();
    } else if (role == 2) {
        user = new Teacher();
    } else {

    }
    return user;         // 指针→值语义转换错误
}
#endif   // 设计笔记结束，以下恢复正常编译
