//
// Created by 杨充 on 2026/5/28.
//
// ============================================================================
// 【文件定位】CampusSystem.h —— 校园系统核心类（所有业务规则的唯一归属地）
// ----------------------------------------------------------------------------
// 【设计思路】"MVC 雏形"：
//   - Model（数据）：users / rooms / reservations / speeches 四大容器 + 实体类；
//   - Controller（业务）：本类的方法（登录、预约、审核、评分、统计……）；
//   - View（交互）：Student/Teacher/Admin 的 mainMenu（菜单 + 转发）。
//   把"改数据"的权力全部集中到本类，角色类只调用不直改，保证规则一致性。
//
// 【容器选型理由】（STL 数据结构特性决定）
//   users        → map<string, shared_ptr<User>>  ：按账号 id 有序 + O(logN) 查找登录
//   rooms        → map<int, Computer>             ：机房编号有序 + 查重/查找方便
//   reservations → vector<Reservation>            ：预约单顺序追加、遍历为主，无需按 key 查
//   reservedRooms→ set<int>                       ：只关心"哪些机房被占用"，O(logN) 查重
//   speeches     → multimap<int, Speech>          ：同一轮次多人报名，key 允许重复
// ============================================================================

#ifndef CAMPUSSYSTEM_H // 头文件保护宏：防止重复包含
#define CAMPUSSYSTEM_H
#include "User.h"      // 需要 User 完整定义（map 中存储其指针并调用其方法）
#include <map>         // std::map / std::multimap（红黑树实现的有序关联容器）
#include <memory>      // std::shared_ptr（引用计数智能指针）
#include <set>         // std::set（红黑树实现的有序去重集合）
#include <string>      // std::string
#include <vector>      // std::vector（动态数组）

#include "Computer.h"      // 值语义存储 Computer，需要完整定义
#include "Reservation.h"   // 值语义存储 Reservation，需要完整定义


class Speech;  // 前向声明：CampusSystem.h 中只出现 Speech 的名字（multimap 声明处），
               // speech 对象的创建/访问都在 .cpp 中进行，故头文件里前向声明即可。
               // （注：multimap<int, Speech> 的成员声明其实需要完整类型才能实例化，
               //   这里能过编译是因为 <map> 只用到 Speech 的声明形式——
               //   严谨起见可把 #include "Speech.h" 直接放头文件。）

class CampusSystem {
private:
    // 【技术原理】map：红黑树（自平衡二叉搜索树）实现的有序键值容器
    //   - 插入/查找/删除均为 O(logN)；
    //   - 按 key（账号）字典序遍历；
    //   - value 是 shared_ptr<User> 而非 User 值：
    //     a) User 是抽象类，无法按值存储；
    //     b) 指针 + 虚函数才能实现"一个容器装三种角色"的多态；
    //     c) shared_ptr 自动引用计数，离开作用域自动 delete，防内存泄漏（RAII）。
    std::map<std::string, std::shared_ptr<User>> users;
    // 机房表：key=机房编号（天然查重），value=机房实体。
    // 与 users 不同，Computer 是具体类且体积小，直接值存储避免指针间接层。
    std::map<int, Computer> rooms;
    std::vector<Reservation> reservations;      // 容器
    // 【技术原理】vector：连续内存动态数组，push_back 均摊 O(1)，
    //             预约单按时间顺序追加，遍历统计/审核场景为主，适合 vector。
    std::set<int> reservedRooms;
    // 【技术原理】set：元素唯一且有序的集合（同为红黑树）。
    // 【设计思路】"占用表"冗余索引：vector 里也能算出谁被占用，但那是 O(N) 扫描；
    //             维护一个 set 把"机房是否空闲"查询降到 O(logN)，
    //             用少量内存换取查询性能（空间换时间的经典取舍）。
    //             【注意】冗余数据必须与主数据严格同步增删，否则出现不一致 bug
    //             （本代码在取消预约/拒绝审核时都会同步 erase）。
    int nextResId = 1;          // 自增 id
    // 【技术原理】NSDMI 类内初始化：预约号单调递增、永不重复；
    //             nextResId++ 后缀自增：先取旧值再 +1，保证第一条预约号是 1。
    // 存储键值对（key-value pairs）。与 std::map 不同，std::multimap 允许键（key）重复，即多个值可以关联到同一个键。
    // 【设计思路】一轮比赛(key=1) 可以有多条报名(value=多个 Speech) —— 正是多对一关系，
    //             用 multimap 免去"map<int, vector<Speech>>"的嵌套写法。
    std::multimap<int, Speech> speeches;
public:
    CampusSystem();  // 构造函数（.cpp 中打印启动提示）
    // 登录：账号不存在/密码错误返回 nullptr，成功返回多态的用户指针
    std::shared_ptr<User> login(const std::string& id, const std::string& pwd);

    // ============ 学生业务（由 Student::mainMenu 转发调用）============
    // student
    // 1.浏览机房
    void listRooms() const;    // const：纯只读查询，不改变任何成员
    // 2.预约机房
    bool reserveRoom(const std::string& sid, int roomId, const std::string& date);
    // 3.取消预约
    bool cancelReservation(const std::string& sid, int resId);
    // 4.报名演讲
    bool signupSpeech(const std::string& sid, const std::string& topic, int round);

    // ============ 教师业务（由 Teacher::mainMenu 转发调用）============
    // teacher
    // 1.待审预约
    void listPendingReservations() const;                       // ⭐ Teacher 业务方法 1
    // 2.审核预约
    bool reviewReservation(int resId, bool approved);           // ⭐ Teacher 业务方法 2
    // 3.演讲评分
    void scoreSpeech(const std::string& sid, int round, double score);   // ⭐ 业务方法 3
    // 4.查看排名
    void rankSpeechesByScore(int round) const;     // ⭐ 业务方法 4：lambda 排序高光

    // ============ 管理员业务（由 Admin::mainMenu 转发调用）============
    // 1.添加机房
    bool addRoom(const Computer& c);     // ⭐ Admin 业务方法
    // 2.添加用户：先查重再插入；参数是 shared_ptr（移交所有权给 users map）
    bool addUser(std::shared_ptr<User> u);
    void statistics() const;     // ⭐ 业务方法：lambda 三连 accumulate
    // 【技术原理】statistics() 加 const（其余写操作不加）：编译器会逐个检查
    //             const 方法内是否意外修改成员，相当于免费的正确性检查。
};



#endif //CAMPUSSYSTEM_H    // 头文件保护宏结束
