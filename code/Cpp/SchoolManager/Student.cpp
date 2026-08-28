//
// Created by 杨充 on 2026/5/28.
//

#include "Student.h"
#include <iostream>

#include "CampusSystem.h"
using namespace std;

void Student::mainMenu() {
    while (true) {
        cout << "\n--- 学生 " << userName << " ---\n";
        cout << "1. 浏览机房  2. 预约机房  3. 取消预约  4. 报名演讲  0. 退出登录\n";
        int op; cin >> op;
        switch (op) {
            case 1: {
                cout << "[Student] 进入了 listRooms \n";
                sys->listRooms();
                break;
            }
            case 2: {
                cout << "[Student] 进入了 reserveRoom \n";
                // int roomId; std::string date;
                // cout << "机房编号: "; cin >> roomId;
                // cout << "日期(YYYY-MM-DD): "; cin >> date;
                // sys->reserveRoom(userId, roomId, date);
                break;
            }

            case 3: {
                cout << "[Student] 进入了 cancelReservation \n";
                int resId;
                cout << "预约号: "; cin >> resId;
                sys->cancelReservation(userId, resId);
                break;
            }
            case 4: {
                cout << "[Student] 进入了 signupSpeech \n";
                string topic; int round;
                cout << "演讲主题: "; cin >> topic;
                cout << "轮次(1=初赛, 2=复赛): "; cin >> round;
                sys->signupSpeech(userId, topic, round);
                break;
            }
            case 0: return;
            default: cout << "无效选择\n";
        }
    }
}


