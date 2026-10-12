//
// Created by 杨充 on 2026/5/28.
//
// ============================================================================
// 【文件定位】Teacher.cpp —— 教师菜单的实现（交互层 / View 层）
// ----------------------------------------------------------------------------
// 【设计思路】与学生菜单同构：事件循环 + switch 分发 + 转发到 CampusSystem。
//   差异在于教师是"审批/评审"视角：
//   - 审核的对象是别人（学生）提交的预约 → 输入预约号而非 userId；
//   - 评分需要"学生账号 + 轮次 + 分数"三个定位参数。
// ============================================================================

#include "Teacher.h"    // 本类声明

#include "CampusSystem.h"   // 解引用 sys 指针需要完整定义（头文件中仅前向声明）

// 教师主菜单：覆写基类纯虚函数，由虚表分发调用
void Teacher::mainMenu() {
    while (true) {      // 事件循环：直到选 0 退出
        std::cout << "\n--- 教师 " << userName << " ---\n";    // userName 为继承的 protected 成员
        std::cout << "1. 待审预约  2. 审核预约  3. 演讲评分  4. 查看排名  0. 退出登录\n";
        int op;                     // 菜单选项
        std::cin >> op;             // 读入选项（未做非法输入校验，教学简化）
        switch (op) {
            case 1: {               // 查看待审核列表：纯只读查询，直接转发
                    std::cout << "[Teacher] listPending \n";
                    sys->listPendingReservations();   // 业务层内部用 count_if 过滤 Pending 状态
                    break;
                }
            case 2: {               // 审核预约：采集预约号 + 审核结论，转发给业务层
                    std::cout << "[Teacher] reviewReservation \n";
                    int resId; int approved;          // approved 用 int 接收（1=通过，0=拒绝）
                    std::cout << "预约号: ";
                    std::cin >> resId;                // 读预约单编号
                    std::cout << "通过(1)还是拒绝(0): ";
                    std::cin >> approved;             // 读审核结论
                    // "approved == 1" 把 int 转换为 bool 再传参：
                    // 接口用 bool 表意更清晰，交互层承担"数字→布尔"的翻译职责
                    sys->reviewReservation(resId, approved == 1);
                    break;
                }
            case 3: {               // 演讲评分：定位"某学生某轮"的报名记录并写分
                    std::cout << "[Teacher] scoreSpeech \n";
                    std::string sid; int round; double score;    // 三元组精确定位一条报名
                    std::cout << "学生账号: ";
                    std::cin >> sid;
                    std::cout << "轮次(1/2): ";
                    std::cin >> round;
                    std::cout << "分数: ";
                    std::cin >> score;                // double：支持小数分（如 8.5）
                    sys->scoreSpeech(sid, round, score);
                    break;
                }
            case 4: {               // 查看排名：按轮次对评分做降序排序展示
                    std::cout << "[Teacher] rankSpeeches \n";
                    int round;
                    std::cout << "查看哪一轮(1/2): ";
                    std::cin >> round;
                    sys->rankSpeechesByScore(round);  // 业务层内部：equal_range 抽取 + sort 排序
                    break;
                }
            case 0: return;         // 退出登录：结束本函数，回到 main 的登录循环
            default: std::cout << "无效选择\n";        // 范围外输入的防兜底提示
        }
    }
}

