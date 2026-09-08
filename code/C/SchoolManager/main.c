#include "SchoolManager.h"

int main() {
    loadAllData();   // 启动时加载数据

    int choice;
    while (1) {
        showMainMenu();
        scanf("%d", &choice);
        while (getchar() != '\n');   // 清空残留换行

        if (choice == 0) {
            printf("再见！\n");
            saveAllData();   // 退出前保存
            return 0;
        }
        if (choice < 1 || choice > 3) {
            printf("无效选择！\n");
            continue;
        }

        UserRole role;
        char name[NAME_LEN];
        int userId;
        // 传入用户选择的身份，login 内部做身份匹配校验（防越权）
        if (!login((UserRole)choice, &role, name, &userId)) continue;

        // ⭐ 根据角色进入不同子菜单 — C 语言的"多态分发"
        switch (role) {
            case ROLE_STUDENT: studentMenu(userId, name); break;
            case ROLE_TEACHER: teacherMenu(userId, name); break;
            case ROLE_ADMIN:   adminMenu(userId, name);   break;
            default: break;
        }
    }
    return 0;
}
