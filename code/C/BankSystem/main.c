#include "BankSystem.h"

int main() {
    loadFromFile();   // 启动时自动加载已有数据

    int choice;
    while (1) {
        showMenu();
        scanf("%d", &choice);
        // ⚠️ 消费掉 scanf 残留的换行符，避免污染下一次输入
        while (getchar() != '\n');

        switch (choice) {
            case 1:  // 开户
                openAccount();
                break;
            case 2:  // 存款
                deposit();
                break;
            case 3:  // 取款
                withdraw();
                break;
            case 4:  // 查询余额
                checkBalance();
                break;
            case 5:  // 转账
                transfer();
                break;
            case 6:  // 保存数据
                saveToFile();
                break;
            case 7:  // 退出
                printf("再见！\n");
                return 0;
            default:
                printf("无效选择！\n");
                break;
        }
    }
    return 0;
}
