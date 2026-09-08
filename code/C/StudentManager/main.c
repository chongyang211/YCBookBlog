#include "StudentManager.h"

int main() {
    LoadFromFile();   // 启动时自动加载已有数据

    while (1) {
        showMenu();
        int choice;
        scanf("%d", &choice);
        // ⚠️ 消费掉 scanf 残留的换行符，否则下次 getchar() 会读到它
        while (getchar() != '\n');

        switch (choice) {
            case 1:  // 录入学生信息
                InputStudent();
                break;
            case 2:  // 打印学生信息
                PrintStudent();
                break;
            case 3:  // 保存学生信息
                SaveToFile();
                break;
            case 4:  // 读取学生信息
                LoadFromFile();
                break;
            case 5:  // 统计学生人数
                CountStudent();
                break;
            case 6:  // 查找学生信息
                FindStudent();
                break;
            case 7:  // 修改学生信息
                ModifyStudent();
                break;
            case 8:  // 删除学生信息
                DeleteStudent();
                break;
            case 0:  // 退出系统
                printf("退出系统，再见！\n");
                freeList();   // 释放整个链表，避免内存泄漏
                return 0;
            default:
                printf("无效选择，请重新输入！\n");
                pauseProgram();
                clearScreen();
                break;
        }
    }
    return 0;
}
