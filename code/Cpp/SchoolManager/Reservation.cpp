//
// Created by 杨充 on 2026/5/28.
//
// ============================================================================
// 【文件定位】Reservation.cpp —— Reservation 实体的实现文件
// ----------------------------------------------------------------------------
// 【设计思路】单行小函数（statusText/构造/toCsv/状态映射）已在 Reservation.h
//             类内定义（隐式 inline）；本文件只放逻辑较长的 fromCsv 反序列化。
//             序列化/反序列化成对放在实体类内，字段格式知识不外泄（信息隐藏）。
// ============================================================================

#include "Reservation.h"    // 引入 Reservation 类的完整定义
#include <sstream>          // std::istringstream（按逗号切分字段）

// 静态工厂：CSV 一行 → Reservation 对象
// 【技术原理】字段顺序与 toCsv 严格对应：resId,studentId,roomId,date,status
// 【安全边界】字段缺失或 resId 非数字时返回"全空默认对象"（resId=0），
//             由调用方（loadReservations 的外键/有效性校验）统一丢弃，
//             解析层只负责"尽力解析"，不做业务判断。
Reservation Reservation::fromCsv(const std::string &line) {
    std::istringstream iss(line);   // 整行文本包装成输入流
    std::string idStr, sid, cidStr, date, statusText;
    if (!std::getline(iss, idStr,     ',')) return Reservation{};   // resId
    if (!std::getline(iss, sid,       ',')) return Reservation{};   // studentId
    if (!std::getline(iss, cidStr,    ',')) return Reservation{};   // roomId
    if (!std::getline(iss, date,      ',')) return Reservation{};   // date
    if (!std::getline(iss, statusText))     return Reservation{};   // status（读到行尾）

    Reservation r{};
    try {
        r.resId = std::stoi(idStr);
        r.computerId = std::stoi(cidStr);
    } catch (const std::exception&) {   // 编号非数字：返回默认对象交由调用方丢弃
        return Reservation{};
    }
    r.studentId = sid;
    r.date      = date;
    r.status    = statusFromText(statusText);   // 未知文本按 Pending 兜底
    return r;
}
