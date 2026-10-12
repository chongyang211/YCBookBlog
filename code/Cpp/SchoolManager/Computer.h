//
// Created by 杨充 on 2026/5/28.
//
// ============================================================================
// 【文件定位】Computer.h —— 机房实体类（纯数据载体 / POJO）
// ----------------------------------------------------------------------------
// 【设计思路】
//   1. "实体类"只描述数据（编号/容量/配置），不带业务行为 —— 业务规则
//      （如"机房不能重复添加"）统一收敛在 CampusSystem 中，职责分离。
//   2. 成员全部 public 且无不变量约束（id 唯一性由 map 的 key 保证），
//      属于教学项目常用的简化；更严格的写法是 private + getter，
//      并在构造时校验 capacity > 0 等约束。
//   3. 自带 toCsv/fromCsv 一对方法：序列化(对象→字符串)与反序列化(字符串→对象)，
//      为阶段 ⑦ 的文件持久化做准备。
// ============================================================================

#ifndef COMPUTER_H   // 头文件保护宏：防止重复包含
#define COMPUTER_H
#include <string>    // std::string 成员 spec 依赖


// 【设计权衡】聚合类型用 struct 还是 class？
//             C++ 中两者唯一区别是默认访问权限（struct 默认 public，class 默认 private）。
//             纯数据聚合习惯用 struct；本项目用 class + 全 public 成员，效果等价。
class Computer {
public:
    int     id;            // 机房编号（主键）
    // 【技术原理】"主键"语义：id 作为 CampusSystem::rooms 这个 map<int, Computer> 的 key，
    //             由 map 天然保证"不重复"（addRoom 前先 count 检查）。
    int     capacity;      // 容量（人数）
    std::string spec;      // 配置（如 "i7+RTX4060"）

    // 使用编译器生成的默认构造函数。
    // 【技术原理】一旦定义了任何构造函数，编译器就不再自动生成默认构造函数；
    //             "= default" 显式要求编译器补回无参版本，
    //             这样 map<int, Computer>::operator[] 等需要"可默认构造"的场景才能工作。
    Computer() = default;
    // 带参构造：成员全部用初始化列表直接初始化，比"函数体内赋值"更高效
    // 【技术原理】初始化列表是"直接初始化"（成员一出生就有正确值），
    //             函数体赋值则是"先默认构造再赋值"（多一步开销，string 尤其明显）。
    //             参数名加下划线后缀（id_/cap/s）避免与成员名遮蔽（shadowing）。
    Computer(int id_, int cap, const std::string& s)
     : id(id_), capacity(cap), spec(s) {}

    // 序列化为 CSV 一行："编号,容量,配置"
    // 【技术原理】std::to_string(int) 把整数转字符串（C++11），
    //             多个 string 用 "+" 拼接会产生临时对象（教学写法）；
    //             高频场景建议用 std::ostringstream 或 string::append 减少临时量。
    // const 成员函数：承诺只读，不修改任何成员。
    std::string toCsv() const {
        return std::to_string(id) + "," + std::to_string(capacity) + "," + spec;
    }

    // 从 CSV 格式的字符串中解析并创建一个 Computer 对象。
    // 阶段 ⑦ 才会真正实现，先放声明
    // 【技术原理】static 工厂方法：不依赖任何实例，直接 "Computer::fromCsv(line)"
    //             调用即可得到对象 —— 把"解析"责任内聚到实体类自身，
    //             外部（FileStore）无需了解字段顺序等细节。
    static Computer fromCsv(const std::string& line);
};



#endif //COMPUTER_H // 头文件保护宏结束
