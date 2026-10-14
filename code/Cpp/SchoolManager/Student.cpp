//
// Created by 杨充 on 2026/5/28.
//
// ============================================================================
// 【文件定位】Student.cpp —— 学生菜单的实现（交互层 / View 层）
// ----------------------------------------------------------------------------
// 【设计思路】
//   1. mainMenu 是一个"事件循环"（event loop）：打印菜单 → 读选项 →
//      switch 分发 → 回到菜单，直到用户选 0 退出（return 结束函数）。
//   2. 学生自己不实现业务，全部转发给 sys->（CampusSystem）——
//      "UI 只做交互与转发，规则收敛在业务层"，便于将来把控制台换成 GUI/Web。
// ============================================================================

#include "Student.h"    // 本类的声明
#include <iostream>     // cin/cout 标准输入输出

#include "CampusSystem.h"   // 调用 sys->listRooms() 等方法需要 CampusSystem 的完整定义
                            // （头文件里只有前向声明，真正解引用指针必须在这里拿到定义）
using namespace std;    // 教学代码简化写法：省略 std:: 前缀（生产代码不推荐，易命名冲突）

// 覆写基类纯虚函数：学生的专属菜单循环
// 【技术原理】此函数通过 User* 指针被虚表（vtable）动态分发调用，
//             同一句 user->mainMenu() 会根据对象真实类型执行不同菜单。
void Student::mainMenu() {
    while (true) {          // 无限循环：菜单反复显示，直到用户主动退出
        cout << "\n--- 学生 " << userName << " ---\n";    // userName 是 protected 继承来的成员，可直接访问
        cout << "1. 浏览机房  2. 预约机房  3. 取消预约  4. 报名演讲  0. 退出登录\n";
        int op; cin >> op;  // 读取用户输入的菜单编号
        // 【技术原理】cin >> op：>> 是流提取运算符重载，按 int 格式解析；
        //             若输入非数字会置失败位导致 op 未更新（教学代码未做输入校验，
        //             健壮写法应检查 cin.fail() 并 clear()+ignore() 清理缓冲区）。
        switch (op) {       // 按选项分发到对应业务
            case 1: {       // 浏览机房：纯查询，直接转发
                cout << "[Student] 进入了 listRooms \n";   // 执行轨迹日志，便于调试
                sys->listRooms();      // 转发给 CampusSystem（sys 指针由 main 登录后注入）
                break;                 // break 跳出 switch，回到 while 重新显示菜单
            }
            case 2: {       // 预约机房：功能占位（参数采集代码被注释掉，待后续阶段启用）
                cout << "[Student] 进入了 reserveRoom \n";
                int roomId; std::string date;
                cout << "机房编号: "; cin >> roomId;
                cout << "日期(YYYY-MM-DD): "; cin >> date;
                sys->reserveRoom(userId, roomId, date);
                break;
            }
            case 3: {       // 取消预约：先采集预约号，再转发
                cout << "[Student] 进入了 cancelReservation \n";
                int resId;
                cout << "预约号: "; cin >> resId;       // 读取要取消的预约单编号
                sys->cancelReservation(userId, resId);  // 传 userId：让业务层校验"只能取消自己的预约"
                break;
            }
            case 4: {       // 报名演讲：采集主题与轮次后转发
                cout << "[Student] 进入了 signupSpeech \n";
                string topic; int round;    // 主题（单词输入，cin 遇空格截断）与轮次
                cout << "演讲主题: "; cin >> topic;
                cout << "轮次(1=初赛, 2=复赛): "; cin >> round;
                sys->signupSpeech(userId, topic, round);   // 轮次合法性由业务层再校验（1/2）
                break;
            }
            case 0: return;     // 退出登录：return 直接结束 mainMenu，控制流回到 main 的登录循环
            default: cout << "无效选择\n";    // 兜底分支：输入不在 0-4 范围时提示（防呆设计）
        }
    }
}


