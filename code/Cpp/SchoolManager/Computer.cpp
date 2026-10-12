//
// Created by 杨充 on 2026/5/28.
//
// ============================================================================
// 【文件定位】Computer.cpp —— Computer 实体 fromCsv 反序列化的实现
// ----------------------------------------------------------------------------
// 【设计思路】toCsv（对象→CSV 字符串）已在 Computer.h 内联实现；
//             fromCsv（CSV 字符串→对象）逻辑较长，放 .cpp 中实现。
//             序列化/反序列化成对放在实体类内，字段格式知识不外泄（信息隐藏）。
// ============================================================================

#include "Computer.h"  // 本类声明
#include <sstream>     // std::istringstream（字符串流：把一行文本当流读取，便于按逗号切分）

// 类外定义静态工厂方法：字符串 → Computer 对象
// 【技术原理】静态成员函数没有 this 指针，通过 "Computer::fromCsv(line)" 调用。
//             典型实现思路（阶段 ⑦）：
//               1. istringstream iss(line);    把整行文本包装成输入流；
//               2. 用 std::getline(iss, field, ',') 逐段按逗号切出字段；
//               3. 字符串转数值可用 std::stoi / iss >> id 的方式解析；
//               4. 按字段顺序（id, capacity, spec）构造并返回 Computer。
Computer Computer::fromCsv(const std::string &line) {
    // TODO（阶段 ⑦）: 真正解析 CSV
    return Computer{};      // 暂时返回默认构造的空机房（id=0, capacity=0, spec=""）
    // 【技术原理】"Computer{}" 是 C++11 值初始化语法：
    //             int 成员被零初始化，string 走默认构造为空串，
    //             保证返回值处于确定状态（不会是未初始化的随机内存）。
}


