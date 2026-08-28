//
// Created by 杨充 on 2026/5/28.
//

#include "Teacher.h"

#include "CampusSystem.h"

void Teacher::mainMenu() {
    while (true) {
        std::cout << "\n--- 教师 " << userName << " ---\n";
        std::cout << "1. 待审预约  2. 审核预约  3. 演讲评分  4. 查看排名  0. 退出登录\n";
        int op;
        std::cin >> op;
        switch (op) {
            case 1:
                std::cout << "[Teacher] listPending \n";
                sys->listPendingReservations();
                break;
            case 2:
                std::cout << "[Teacher] reviewReservation \n";
                int resId; int approved;
                std::cout << "预约号: ";
                std::cin >> resId;
                std::cout << "通过(1)还是拒绝(0): ";
                std::cin >> approved;
                sys->reviewReservation(resId, approved == 1);
                break;
            case 3:
                std::cout << "[Teacher] scoreSpeech \n";
                std::string sid; int round; double score;
                std::cout << "学生账号: ";
                std::cin >> sid;
                std::cout << "轮次(1/2): ";
                std::cin >> round;
                std::cout << "分数: ";
                std::cin >> score;
                sys->scoreSpeech(sid, round, score);
                break;
            case 4: {
                std::cout << "[Teacher] rankSpeeches \n";
                int round;
                std::cout << "查看哪一轮(1/2): ";
                std::cin >> round;
                sys->rankSpeechesByScore(round);
                break;
            }
            case 0: return;
            default: std::cout << "无效选择\n";
        }
    }
}

