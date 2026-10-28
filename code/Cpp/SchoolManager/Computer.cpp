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
//             字段顺序与 toCsv 严格对应：id,capacity,spec
// 【安全边界】解析失败（字段不足/编号非数字）返回默认对象由调用方丢弃，
//             不抛异常中断主流程 —— 教学项目保持 IO 路径"永不中断"。
Computer Computer::fromCsv(const std::string &line) {
    std::istringstream iss(line);   // 把整行文本包装成输入流
    std::string idStr, capStr, spec;
    // 按逗号逐段切出字段：getline 的第三个参数是分隔符（读到分隔符为止，分隔符本身丢弃）
    if (!std::getline(iss, idStr, ',')) return Computer{};
    if (!std::getline(iss, capStr, ',')) return Computer{};
    if (!std::getline(iss, spec))       return Computer{};  // 最后一个字段读到行尾
    try {
        return Computer{std::stoi(idStr), std::stoi(capStr), spec};
    } catch (const std::exception&) {   // stoi 遇到非数字会抛异常：兜底成空对象
        return Computer{};
    }
}


