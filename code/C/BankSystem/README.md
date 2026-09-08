# BankSystem — C语言 银行账户管理系统

《C 语言入门精通》综合案例的**第二关**。从第一关"学生通讯录"的单表链表，升级到"开户/存款/取款/转账"的事务性系统。核心挑战从"数据结构"转向**数据一致性**——转账时两个账户的余额必须同时成功或同时失败。

与第一关的区别：**结构体数组替代链表**（按卡号随机查询多）、**转账事务保护**、**文本格式持久化**（替代二进制）。

## 功能

| 菜单项 | 功能 | 核心 |
|--------|------|------|
| 1 开户 | 自动递增卡号 | `accountCount + 1` |
| 2 存款 | 余额增加 | 金额 > 0 检查 |
| 3 取款 | 余额减少（不能透支） | 余额足够检查 |
| 4 查询余额 | 按卡号查询 | 复用 `findAccount` |
| 5 转账 | A→B 双方同时变更 | **事务一致性** |
| 6 保存数据 | 写入 `bank_accounts.txt` | `fprintf` 文本 |
| 7 退出 | — | — |

## 目录结构

```text
BankSystem/
├── BankSystem.h       # Account 结构体 + 函数声明
├── BankSystem.c       # 函数实现
├── main.c             # 菜单循环入口
├── CMakeLists.txt
└── README.md
```

## 数据结构

```c
typedef struct {
    int    accountNo;       // 卡号
    char   name[NAME_LEN];  // 姓名
    double balance;         // 余额
} Account;

Account accounts[MAX_ACCOUNTS];  // 全局数组（定长 100）
int     accountCount = 0;         // 全局计数器
```

## 编译运行

```bash
cd BankSystem
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/BankSystem
```

## 核心知识点

- **数组 vs 链表**：银行账户"按卡号随机查"多、数量可预估 → 用数组（O(1) 随机访问）；第一关学生"顺序遍历"多、数量不可预估 → 用链表
- **自动递增卡号**：`accountNo = accountCount + 1`，避免用户手动输入导致的重复卡号
- **事务一致性**：转账"先全部校验 → 再全部修改"，避免"扣了 A 的钱、B 没收到"
- **文本持久化**：银行数据是可读的数字和姓名，`fprintf`/`fscanf` 比二进制更实用（可 diff、可手动编辑）

## 关键陷阱（C 语言经典坑）

1. **数组越界**：`accounts[accountCount]` 在 `accountCount >= MAX_ACCOUNTS` 时会越界
2. **重复卡号**：手动输入卡号会导致两个用户共用同一卡号
3. **转账"丢钱"**：先扣后加中间无校验 → A 透支(-1000) + B 多钱(+2500) = 钱凭空多出 500
4. **`fscanf` 返回值不检查**：文件格式被改坏时静默失败
