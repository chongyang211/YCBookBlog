
### 代码实现：
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNTS 100
#define NAME_LENGTH 50
#define FILENAME "bank_accounts.txt"

typedef struct {
    int accountNumber;
    char name[NAME_LENGTH];
    double balance;
} BankAccount;

BankAccount accounts[MAX_ACCOUNTS];
int account_count = 0;

// 开户
void openAccount() {
    if (account_count >= MAX_ACCOUNTS) {
        printf("账户数量已达到上限，无法继续开户！\n");
        return;
    }

    BankAccount newAccount;
    newAccount.accountNumber = account_count + 1;

    printf("请输入账户姓名: ");
    scanf("%s", newAccount.name);

    printf("请输入初始余额: ");
    scanf("%lf", &newAccount.balance);

    accounts[account_count] = newAccount;
    account_count++;

    printf("账户开户成功！账户号: %d\n", newAccount.accountNumber);
}

// 存款
void deposit() {
    int accountNumber;
    double amount;

    printf("请输入账户号: ");
    scanf("%d", &accountNumber);

    for (int i = 0; i < account_count; i++) {
        if (accounts[i].accountNumber == accountNumber) {
            printf("请输入存款金额: ");
            scanf("%lf", &amount);

            if (amount <= 0) {
                printf("存款金额必须大于0！\n");
                return;
            }

            accounts[i].balance += amount;
            printf("存款成功！当前余额: %.2lf\n", accounts[i].balance);
            return;
        }
    }

    printf("未找到账户号为%d的账户！\n", accountNumber);
}

// 取款
void withdraw() {
    int accountNumber;
    double amount;

    printf("请输入账户号: ");
    scanf("%d", &accountNumber);

    for (int i = 0; i < account_count; i++) {
        if (accounts[i].accountNumber == accountNumber) {
            printf("请输入取款金额: ");
            scanf("%lf", &amount);

            if (amount <= 0) {
                printf("取款金额必须大于0！\n");
                return;
            }

            if (amount > accounts[i].balance) {
                printf("余额不足，无法取款！\n");
                return;
            }

            accounts[i].balance -= amount;
            printf("取款成功！当前余额: %.2lf\n", accounts[i].balance);
            return;
        }
    }

    printf("未找到账户号为%d的账户！\n", accountNumber);
}

// 查询余额
void checkBalance() {
    int accountNumber;

    printf("请输入账户号: ");
    scanf("%d", &accountNumber);

    for (int i = 0; i < account_count; i++) {
        if (accounts[i].accountNumber == accountNumber) {
            printf("账户号: %d, 姓名: %s, 当前余额: %.2lf\n",
                   accounts[i].accountNumber,
                   accounts[i].name,
                   accounts[i].balance);
            return;
        }
    }

    printf("未找到账户号为%d的账户！\n", accountNumber);
}

// 转账
void transfer() {
    int fromAccount, toAccount;
    double amount;

    printf("请输入转出账户号: ");
    scanf("%d", &fromAccount);

    printf("请输入转入账户号: ");
    scanf("%d", &toAccount);

    if (fromAccount == toAccount) {
        printf("转出账户和转入账户不能相同！\n");
        return;
    }

    int fromIndex = -1, toIndex = -1;
    for (int i = 0; i < account_count; i++) {
        if (accounts[i].accountNumber == fromAccount) {
            fromIndex = i;
        }
        if (accounts[i].accountNumber == toAccount) {
            toIndex = i;
        }
    }

    if (fromIndex == -1 || toIndex == -1) {
        printf("转出账户或转入账户不存在！\n");
        return;
    }

    printf("请输入转账金额: ");
    scanf("%lf", &amount);

    if (amount <= 0) {
        printf("转账金额必须大于0！\n");
        return;
    }

    if (amount > accounts[fromIndex].balance) {
        printf("转出账户余额不足，无法转账！\n");
        return;
    }

    accounts[fromIndex].balance -= amount;
    accounts[toIndex].balance += amount;
    printf("转账成功！\n");
    printf("转出账户余额: %.2lf, 转入账户余额: %.2lf\n",
           accounts[fromIndex].balance,
           accounts[toIndex].balance);
}

// 保存数据到文件
void saveToFile() {
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf("无法打开文件！\n");
        return;
    }

    for (int i = 0; i < account_count; i++) {
        fprintf(file, "%d %s %.2lf\n", accounts[i].accountNumber, accounts[i].name, accounts[i].balance);
    }

    fclose(file);
    printf("账户数据已保存到文件！\n");
}

// 从文件读取数据
void loadFromFile() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("文件不存在或无法打开！\n");
        return;
    }

    account_count = 0;
    while (fscanf(file, "%d %s %lf", &accounts[account_count].accountNumber, accounts[account_count].name, &accounts[account_count].balance) != EOF) {
        account_count++;
    }

    fclose(file);
    printf("账户数据已从文件加载！\n");
}

int main() {
    int choice;

    loadFromFile(); // 启动时加载文件中的账户数据

    while (1) {
        printf("\n银行账户管理系统\n");
        printf("1. 开户\n");
        printf("2. 存款\n");
        printf("3. 取款\n");
        printf("4. 查询余额\n");
        printf("5. 转账\n");
        printf("6. 保存数据到文件\n");
        printf("7. 退出\n");
        printf("请输入您的选择: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                openAccount();
                break;
            case 2:
                deposit();
                break;
            case 3:
                withdraw();
                break;
            case 4:
                checkBalance();
                break;
            case 5:
                transfer();
                break;
            case 6:
                saveToFile();
                break;
            case 7:
                printf("退出系统，再见！\n");
                saveToFile(); // 退出前保存数据
                exit(0);
            default:
                printf("无效的选择，请重新输入！\n");
        }
    }

    return 0;
}
```

### 功能说明：
1. **开户**：创建新账户，输入账户号、姓名和初始余额。
2. **存款**：根据账户号存款，更新账户余额。
3. **取款**：根据账户号取款，更新账户余额。
4. **查询余额**：根据账户号查询当前余额。
5. **转账**：支持从一个账户转账到另一个账户。
6. **保存数据到文件**：将账户数据保存到文件中。
7. **从文件读取数据**：启动时从文件加载账户数据。

### 使用方法：
1. 编译并运行程序。
2. 根据提示选择相应的操作。
3. 按照提示输入相关信息。

### 注意事项：
- 账户数据会保存到文件 `bank_accounts.txt` 中，程序启动时会自动加载。
- 转账时需确保转出账户和转入账户都存在，且转出账户余额充足。

通过这个系统，可以方便地管理银行账户信息，并实现数据的持久化存储。