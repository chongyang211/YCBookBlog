#include "BankSystem.h"

Account accounts[MAX_ACCOUNTS];
int     accountCount = 0;

void showMenu() {
    printf("\n========== 银行账户管理系统 ==========\n");
    printf("1. 开户        2. 存款        3. 取款\n");
    printf("4. 查询余额    5. 转账        6. 保存数据\n");
    printf("7. 退出\n");
    printf("=======================================\n");
    printf("请选择: ");
}

// 按卡号查找账户索引，找不到返回 -1
int findAccount(int accountNo) {
    for (int i = 0; i < accountCount; i++) {
        if (accounts[i].accountNo == accountNo) return i;
    }
    return -1;
}

void openAccount() {
    if (accountCount >= MAX_ACCOUNTS) {
        printf("账户数量已达上限！\n");
        return;
    }
    Account newAcc;
    newAcc.accountNo = accountCount + 1;   // 自动分配卡号，永远不重复
    printf("请输入姓名: ");
    scanf("%49s", newAcc.name);
    printf("请输入初始余额: ");
    scanf("%lf", &newAcc.balance);
    if (newAcc.balance < 0) {
        printf("余额不能为负！\n");
        return;
    }
    accounts[accountCount++] = newAcc;
    printf(">>> 开户成功！卡号: %d\n", newAcc.accountNo);
}

void deposit() {
    int accountNo;
    double amount;
    printf("请输入卡号: "); scanf("%d", &accountNo);
    int idx = findAccount(accountNo);
    if (idx == -1) { printf("未找到该账户！\n"); return; }
    printf("请输入存款金额: "); scanf("%lf", &amount);
    if (amount <= 0) { printf("金额必须 >0！\n"); return; }
    accounts[idx].balance += amount;
    printf(">>> 存款成功！余额: %.2f\n", accounts[idx].balance);
}

void withdraw() {
    int accountNo;
    double amount;
    printf("请输入卡号: "); scanf("%d", &accountNo);
    int idx = findAccount(accountNo);
    if (idx == -1) { printf("未找到该账户！\n"); return; }
    printf("请输入取款金额: "); scanf("%lf", &amount);
    if (amount <= 0)     { printf("金额必须 >0！\n"); return; }
    if (amount > accounts[idx].balance) {
        printf("余额不足！当前余额: %.2f\n", accounts[idx].balance);
        return;
    }
    accounts[idx].balance -= amount;
    printf(">>> 取款成功！余额: %.2f\n", accounts[idx].balance);
}

void checkBalance() {
    int accountNo;
    printf("请输入卡号: "); scanf("%d", &accountNo);
    int idx = findAccount(accountNo);
    if (idx == -1) { printf("未找到！\n"); return; }
    printf("卡号: %d | 姓名: %s | 余额: %.2f\n",
           accounts[idx].accountNo,
           accounts[idx].name,
           accounts[idx].balance);
}

void transfer() {
    int fromNo, toNo;
    double amount;
    printf("转出卡号: "); scanf("%d", &fromNo);
    printf("转入卡号: "); scanf("%d", &toNo);
    if (fromNo == toNo) { printf("不能转给自己！\n"); return; }

    int fromIdx = findAccount(fromNo);
    int toIdx   = findAccount(toNo);
    if (fromIdx == -1 || toIdx == -1) {
        printf("账户不存在！\n"); return;
    }
    printf("转账金额: "); scanf("%lf", &amount);
    if (amount <= 0) { printf("金额必须 >0！\n"); return; }

    // ⭐ 事务一致性核心：先全部校验，再全部修改
    // 如果先扣钱再加钱、中间不校验，会导致 A 透支(-1000)+B 多钱(+2500)=钱凭空多出
    if (amount > accounts[fromIdx].balance) {
        printf("余额不足！当前余额: %.2f\n", accounts[fromIdx].balance);
        return;   // 校验不通过 → 双方都不动，资金原封不动
    }

    // 校验通过后，两行修改连续执行（单线程下是原子的）
    accounts[fromIdx].balance -= amount;
    accounts[toIdx].balance   += amount;
    printf(">>> 转账成功！\n");
    printf("A(%s)余额: %.2f → B(%s)余额: %.2f\n",
           accounts[fromIdx].name, accounts[fromIdx].balance,
           accounts[toIdx].name,   accounts[toIdx].balance);
}

void saveToFile() {
    FILE *file = fopen(FILENAME, "w");
    if (!file) { printf("无法打开文件！\n"); return; }
    for (int i = 0; i < accountCount; i++) {
        fprintf(file, "%d %s %.2f\n",
                accounts[i].accountNo,
                accounts[i].name,
                accounts[i].balance);
    }
    fclose(file);
    printf(">>> 数据保存成功！\n");
}

void loadFromFile() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("未找到数据文件，将创建新账户系统。\n");
        return;
    }
    accountCount = 0;   // 清空
    while (accountCount < MAX_ACCOUNTS &&
           fscanf(file, "%d %s %lf",
                  &accounts[accountCount].accountNo,
                  accounts[accountCount].name,
                  &accounts[accountCount].balance) == 3) {
        accountCount++;
    }
    fclose(file);
    printf(">>> 已加载 %d 个账户\n", accountCount);
}
