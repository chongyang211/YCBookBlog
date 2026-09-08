# SchoolManager — C语言 校园机房预约系统

《C 语言入门精通》综合案例的**第三关·多态入门**。从"银行账户"的单一数据结构，升级到**学生/教师/管理员三种身份、各自不同的菜单**。核心挑战是：在 C 语言没有继承和多态的情况下，如何优雅地实现同一套框架支持三种不同行为。

答案：**`enum UserRole` + `switch` 分发（手动 vtable）**——这是 C 语言实现"多态"的标准手法，也是理解 C++ 虚函数表底层原理的最佳入口。

## 三种身份功能矩阵（RBAC）

| 功能 | 学生 | 教师 | 管理员 |
|------|------|------|--------|
| 登录 | ✅ | ✅ | ✅ |
| 申请预约机房 | ✅ | ❌ | ❌ |
| 查看自己的预约 | ✅ | ❌ | ❌ |
| 取消自己的预约 | ✅ | ❌ | ❌ |
| 查看所有预约 | ❌ | ✅ | ✅ |
| 审核预约 | ❌ | ✅ | ❌ |
| 添加学生/教师账号 | ❌ | ❌ | ✅ |
| 查看所有账号 | ❌ | ❌ | ✅ |
| 清空预约 | ❌ | ❌ | ✅ |

## 目录结构

```text
SchoolManager/
├── SchoolManager.h       # 枚举 + 结构体 + 函数声明
├── SchoolManager.c       # 函数实现
├── main.c                # 主菜单 + 身份分发
├── CMakeLists.txt
└── README.md
```

## 数据结构

```c
typedef enum { ROLE_STUDENT=1, ROLE_TEACHER=2, ROLE_ADMIN=3 } UserRole;

typedef enum {
    STATUS_PENDING=0, STATUS_APPROVED=1, STATUS_REJECTED=-1, STATUS_CANCELED=2
} OrderStatus;

typedef struct { int id; char name[50]; char password[20]; UserRole role; } User;
typedef struct { /* orderId/studentId/roomId/日期/时段/人数/status */ } Order;
typedef struct { int roomId; int capacity; } Room;

Room rooms[3] = { {1,20}, {2,50}, {3,100} };   // 机房容量
```

## 编译运行

```bash
cd SchoolManager
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/SchoolManager
```

首次启动自动创建默认管理员：账号 `0`，密码 `admin123`。

## 核心知识点

- **身份多态**：一个 `struct` + `role` 字段区分三种身份，`switch(role)` 分发到各自菜单（对比 C++ 虚函数表）
- **枚举状态机**：预约 `待审核 → 通过/拒绝`，`待审核 → 取消`
- **外键关联**：`orders[i].studentId` 关联用户表，加载时"被引用方先加载"
- **权限分离**：`if (studentId == userId)` 手动校验——学生只能操作自己的预约
- **容量校验**：审核通过时统计该机房已审批人数，超限拒绝（防超订）
- **多文件持久化**：`users.txt` + `orders.txt` 两个文本文件独立读写

## 关键陷阱（C 语言经典坑）

1. **登录越权**：不校验 `role` 与登录选择的身份匹配 → 教师用学生入口也能登（本实现已修复）
2. **机房超订**：不检查容量 → 20 人机房预约 45 人
3. **加载顺序错**：先加载预约再加载用户 → 预约的 `studentId` 找不到人（孤儿数据）
4. **重复卡号/ID**：添加账号不检查 ID 唯一性 → 两个用户共用 ID
