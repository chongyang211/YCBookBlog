//
// Created by 杨充 on 2026/5/28.
//
// ============================================================================
// 【文件定位】Reservation.cpp —— Reservation 实体的实现文件
// ----------------------------------------------------------------------------
// 【设计思路】Reservation 的全部成员函数（statusText）与构造函数均为
//             单行小函数，已在 Reservation.h 中以类内定义（隐式 inline）实现；
//             因此本文件当前仅包含头文件，为后续阶段（⑦ 持久化）
//             预留 toCsv/fromCsv 等"类外定义"的实现位置。
// ============================================================================

#include "Reservation.h"    // 引入 Reservation 类的完整定义
// 【预留】阶段 ⑦ 可在此实现：
//   std::string Reservation::toCsv() const { ... }        // 序列化为 CSV 一行
//   Reservation Reservation::fromCsv(const std::string&); // 从 CSV 行解析
