//
// Created by 杨充 on 2026/5/28.
//
// ============================================================================
// 【文件定位】Speech.h —— 演讲报名实体类（演讲比赛的参赛记录）
// ----------------------------------------------------------------------------
// 【设计思路】
//   1. 比赛分两轮（1=初赛，2=复赛），一轮内可有多人报名 → 天然适合
//      std::multimap<int, Speech>（key=轮次，可重复）来存储，见 CampusSystem。
//   2. score 默认 0.0，报名时未评分，等教师通过 scoreSpeech 打分。
//   3. 与 Reservation 一样是"瘦实体"：只存数据，规则收敛在 CampusSystem。
// ============================================================================

#pragma once          // 与 #ifndef 等价的另一种头文件保护写法（编译器内置，非标准但全平台支持）
#include <string>     // std::string 成员依赖


class Speech {
public:
    std::string studentId;     // 报名学生（外键关联 User）
    // 【技术原理】"外键"：只存账号字符串，不持有 Student 对象本身，
    //             避免循环依赖（Student.h 并不知道 Speech 的存在）。
    std::string topic;         // 主题
    int    round = 1;          // 轮次（1=初赛，2=复赛）
    // 【技术原理】类内默认初始化（NSDMI）：即使默认构造也保证 round 有值。
    double score = 0.0;        // 评分（默认 0，等教师评分）
    // 【技术原理】score = 0.0 与报名即未评分的语义绑定；
    //             排名时未打分的人会以 0 分参与排序（教学简化）。

    // = default 补回默认构造（定义带参构造后编译器不再隐式生成）
    Speech() = default;
    // 带参构造：初始化列表直接初始化三个成员；score 走 NSDMI 默认 0.0
    Speech(const std::string& sid, const std::string& t, int r)
        : studentId(sid), topic(t), round(r) {}
};


