#include "SchoolManager.h"

User  users[MAX_USERS];
int   userCount = 0;

Order orders[MAX_ORDERS];
int   orderCount = 0;
int   orderIdCounter = 1;

Room rooms[3] = { {1, 20}, {2, 50}, {3, 100} };
int  roomCount = 3;

// ===== 辅助：状态/身份转文本 =====

const char *statusText(OrderStatus s) {
    switch (s) {
        case STATUS_PENDING:  return "待审核";
        case STATUS_APPROVED: return "已通过";
        case STATUS_REJECTED: return "已拒绝";
        case STATUS_CANCELED: return "已取消";
        default:              return "未知";
    }
}

const char *roleText(UserRole r) {
    switch (r) {
        case ROLE_STUDENT: return "学生";
        case ROLE_TEACHER: return "教师";
        case ROLE_ADMIN:   return "管理员";
        default:           return "未知";
    }
}

// ===== 菜单与登录 =====

void showMainMenu() {
    printf("\n===== 校园机房预约系统 =====\n");
    printf("1. 学生登录\n");
    printf("2. 教师登录\n");
    printf("3. 管理员登录\n");
    printf("0. 退出\n");
    printf("=============================\n");
    printf("请选择: ");
}

int findUser(int id, const char *password) {
    for (int i = 0; i < userCount; i++) {
        if (users[i].id == id &&
            strcmp(users[i].password, password) == 0) {
            return i;
        }
    }
    return -1;
}

// 登录：返回 1 成功、0 失败，通过指针输出角色/姓名/ID
int login(UserRole selectedRole, UserRole *outRole, char *outName, int *outId) {
    int id;
    char password[20];
    printf("请输入账号: "); scanf("%d", &id);
    printf("请输入密码: "); scanf("%19s", password);

    int idx = findUser(id, password);
    if (idx == -1) {
        printf(">>> 账号或密码错误！\n");
        return 0;
    }
    // ⭐ 身份匹配校验：防止教师用学生入口登录（越权）
    if (users[idx].role != selectedRole) {
        printf(">>> 身份不匹配！请选择正确的登录类型。\n");
        return 0;
    }
    *outRole = users[idx].role;
    strcpy(outName, users[idx].name);
    *outId   = users[idx].id;
    printf(">>> 登录成功！欢迎 %s\n", users[idx].name);
    return 1;
}

// ===== 学生功能 =====

void studentMenu(int userId, const char *name) {
    int choice;
    while (1) {
        printf("\n--- 学生菜单 (%s) ---\n", name);
        printf("1. 申请预约  2. 查看我的预约\n");
        printf("3. 取消预约  0. 注销登录\n");
        printf("请选择: ");
        scanf("%d", &choice);
        while (getchar() != '\n');
        switch (choice) {
            case 1: studentApply(userId, name); break;
            case 2: studentView(userId);        break;
            case 3: studentCancel(userId);      break;
            case 0: return;
        }
    }
}

void studentApply(int userId, const char *name) {
    if (orderCount >= MAX_ORDERS) {
        printf("预约已满！\n"); return;
    }

    printf("\n--- 可用机房 ---\n");
    for (int i = 0; i < roomCount; i++) {
        printf("机房%d: 容量%d人\n", rooms[i].roomId, rooms[i].capacity);
    }

    Order *o = &orders[orderCount];
    o->orderId = orderIdCounter++;
    o->studentId = userId;
    strcpy(o->studentName, name);

    printf("选择机房 (1/2/3): "); scanf("%d", &o->roomId);
    printf("日期 (YYYY-MM-DD): ");  scanf("%10s", o->date);
    printf("时段 (上午/下午): ");    scanf("%9s", o->timeSlot);
    printf("人数: ");              scanf("%d", &o->peopleCount);

    o->status = STATUS_PENDING;
    orderCount++;
    printf(">>> 预约提交成功！订单号: %d（待教师审核）\n", o->orderId);
}

void studentView(int userId) {
    int found = 0;
    printf("\n--- 我的预约 ---\n");
    printf("订单号 | 日期       | 机房 | 时段 | 人数 | 状态\n");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < orderCount; i++) {
        if (orders[i].studentId == userId) {   // 只显示自己的预约
            printf("%-7d | %-10s | %-4d | %-4s | %-4d | %s\n",
                   orders[i].orderId, orders[i].date,
                   orders[i].roomId, orders[i].timeSlot,
                   orders[i].peopleCount, statusText(orders[i].status));
            found = 1;
        }
    }
    if (!found) printf("暂无预约记录。\n");
}

void studentCancel(int userId) {
    int orderId;
    printf("请输入要取消的订单号: "); scanf("%d", &orderId);
    for (int i = 0; i < orderCount; i++) {
        if (orders[i].orderId == orderId &&
            orders[i].studentId == userId) {   // 只能取消自己的
            if (orders[i].status == STATUS_CANCELED) {
                printf("该预约已取消！\n"); return;
            }
            if (orders[i].status != STATUS_PENDING) {
                printf("该预约已被审核，无法取消！\n"); return;
            }
            orders[i].status = STATUS_CANCELED;
            printf(">>> 取消成功！\n"); return;
        }
    }
    printf("未找到该订单或无权限！\n");
}

// ===== 教师功能 =====

void teacherMenu(int userId, const char *name) {
    (void)userId;   // 教师菜单暂不需要按 userId 过滤（保留签名一致性）
    int choice;
    while (1) {
        printf("\n--- 教师菜单 (%s) ---\n", name);
        printf("1. 查看所有预约  2. 审核预约  0. 注销\n");
        printf("请选择: ");
        scanf("%d", &choice);
        while (getchar() != '\n');
        switch (choice) {
            case 1: teacherViewAll(); break;
            case 2: teacherReview();  break;
            case 0: return;
        }
    }
}

void teacherViewAll() {
    printf("\n--- 所有预约 ---\n");
    if (orderCount == 0) { printf("暂无预约记录。\n"); return; }
    printf("订单号 | 学生   | 日期       | 机房 | 时段 | 人数 | 状态\n");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < orderCount; i++) {
        printf("%-7d | %-5s | %-10s | %-4d | %-4s | %-4d | %s\n",
               orders[i].orderId, orders[i].studentName, orders[i].date,
               orders[i].roomId, orders[i].timeSlot,
               orders[i].peopleCount, statusText(orders[i].status));
    }
}

void teacherReview() {
    int orderId, decision;
    printf("请输入订单号: "); scanf("%d", &orderId);

    for (int i = 0; i < orderCount; i++) {
        if (orders[i].orderId == orderId) {
            if (orders[i].status != STATUS_PENDING) {
                printf("该预约已被处理！当前状态不可审核。\n"); return;
            }
            printf("\n当前预约: %s | 机房%d | %s | %d人\n",
                   orders[i].studentName, orders[i].roomId,
                   orders[i].timeSlot, orders[i].peopleCount);
            printf("1.通过  2.拒绝: "); scanf("%d", &decision);

            // ⭐ 容量校验：只有"通过"才需要检查（防超订）
            if (decision == 1) {
                int totalPeople = orders[i].peopleCount;
                for (int j = 0; j < orderCount; j++) {
                    if (orders[j].roomId == orders[i].roomId &&
                        orders[j].status == STATUS_APPROVED &&
                        orders[j].orderId != orders[i].orderId) {
                        totalPeople += orders[j].peopleCount;
                    }
                }
                int capacity = 0;
                for (int r = 0; r < roomCount; r++) {
                    if (rooms[r].roomId == orders[i].roomId) {
                        capacity = rooms[r].capacity;
                        break;
                    }
                }
                if (totalPeople > capacity) {
                    printf(">>> 审核拒绝：机房%d 容量%d人，已审批+当前申请=%d人，超限！\n",
                           orders[i].roomId, capacity, totalPeople);
                    return;   // 维持 Pending 状态，让教师手动拒绝
                }
            }

            orders[i].status = (decision == 1) ? STATUS_APPROVED : STATUS_REJECTED;
            printf(">>> 审核完成！\n");
            return;
        }
    }
    printf("未找到订单！\n");
}

// ===== 管理员功能 =====

void adminMenu(int userId, const char *name) {
    (void)userId;   // 管理员菜单暂不需要按 userId 过滤（保留签名一致性）
    int choice;
    while (1) {
        printf("\n--- 管理员菜单 (%s) ---\n", name);
        printf("1. 添加账号  2. 查看所有账号\n");
        printf("3. 清空预约  0. 注销\n");
        printf("请选择: ");
        scanf("%d", &choice);
        while (getchar() != '\n');
        switch (choice) {
            case 1: adminAddUser();     break;
            case 2: adminViewAll();     break;
            case 3: adminClearOrders(); break;
            case 0: return;
        }
    }
}

void adminAddUser() {
    if (userCount >= MAX_USERS) {
        printf("用户数已达上限！\n"); return;
    }

    int newId, roleChoice;
    printf("身份 (1.学生 2.教师): "); scanf("%d", &roleChoice);
    if (roleChoice != 1 && roleChoice != 2) {
        printf("无效身份！只能添加学生或教师。\n"); return;
    }
    printf("账号ID: "); scanf("%d", &newId);

    // ID 唯一性检查
    for (int i = 0; i < userCount; i++) {
        if (users[i].id == newId) {
            printf("该ID已被占用！\n"); return;
        }
    }

    User *u = &users[userCount];
    u->id = newId;
    u->role = (roleChoice == 1) ? ROLE_STUDENT : ROLE_TEACHER;
    printf("姓名: "); scanf("%49s", u->name);
    printf("密码: "); scanf("%19s", u->password);
    userCount++;
    printf(">>> 账号添加成功！当前用户总数: %d\n", userCount);
}

void adminViewAll() {
    printf("\n--- 所有账号 ---\n");
    printf("ID   | 姓名     | 身份\n");
    printf("----------------------\n");
    for (int i = 0; i < userCount; i++) {
        printf("%-4d | %-8s | %s\n",
               users[i].id, users[i].name, roleText(users[i].role));
    }
    printf("----------------------\n");
    printf("共 %d 个用户\n", userCount);
}

void adminClearOrders() {
    char confirm;
    printf("确认清空所有预约记录？(y/n): ");
    scanf(" %c", &confirm);
    if (confirm == 'y' || confirm == 'Y') {
        orderCount = 0;
        orderIdCounter = 1;
        printf(">>> 已清空所有预约记录！\n");
    } else {
        printf("操作已取消。\n");
    }
}

// ===== 文件持久化 =====

void saveUsers() {
    FILE *f = fopen(USERS_FILE, "w");
    if (!f) { printf("无法打开 %s！\n", USERS_FILE); return; }
    for (int i = 0; i < userCount; i++) {
        fprintf(f, "%d %s %s %d\n",
                users[i].id, users[i].name,
                users[i].password, (int)users[i].role);
    }
    fclose(f);
    printf(">>> 已保存 %d 个用户到 %s\n", userCount, USERS_FILE);
}

void loadUsers() {
    FILE *f = fopen(USERS_FILE, "r");
    if (!f) {
        printf("未找到 %s，将使用默认管理员账号。\n", USERS_FILE);
        // 首次启动：自动创建默认管理员
        users[0].id = 0;
        strcpy(users[0].name, "admin");
        strcpy(users[0].password, "admin123");
        users[0].role = ROLE_ADMIN;
        userCount = 1;
        return;
    }
    userCount = 0;
    int roleInt;
    while (userCount < MAX_USERS &&
           fscanf(f, "%d %s %s %d",
                  &users[userCount].id,
                  users[userCount].name,
                  users[userCount].password,
                  &roleInt) == 4) {
        users[userCount].role = (UserRole)roleInt;
        userCount++;
    }
    fclose(f);
    printf(">>> 已加载 %d 个用户\n", userCount);
}

void saveOrders() {
    FILE *f = fopen(ORDERS_FILE, "w");
    if (!f) { printf("无法打开 %s！\n", ORDERS_FILE); return; }
    for (int i = 0; i < orderCount; i++) {
        fprintf(f, "%d %d %s %d %s %s %d %d\n",
                orders[i].orderId, orders[i].studentId,
                orders[i].studentName, orders[i].roomId,
                orders[i].date, orders[i].timeSlot,
                orders[i].peopleCount, (int)orders[i].status);
    }
    fclose(f);
    printf(">>> 已保存 %d 条预约到 %s\n", orderCount, ORDERS_FILE);
}

void loadOrders() {
    FILE *f = fopen(ORDERS_FILE, "r");
    if (!f) { orderCount = 0; return; }
    orderCount = 0;
    int statusInt, maxOrderId = 0;
    while (orderCount < MAX_ORDERS &&
           fscanf(f, "%d %d %s %d %s %s %d %d",
                  &orders[orderCount].orderId,
                  &orders[orderCount].studentId,
                  orders[orderCount].studentName,
                  &orders[orderCount].roomId,
                  orders[orderCount].date,
                  orders[orderCount].timeSlot,
                  &orders[orderCount].peopleCount,
                  &statusInt) == 8) {
        orders[orderCount].status = (OrderStatus)statusInt;
        if (orders[orderCount].orderId > maxOrderId) {
            maxOrderId = orders[orderCount].orderId;
        }
        orderCount++;
    }
    fclose(f);
    orderIdCounter = maxOrderId + 1;   // 恢复自增计数器
    printf(">>> 已加载 %d 条预约\n", orderCount);
}

// 统一入口：⭐ 先加载被引用方(users)，再加载引用方(orders)
void loadAllData() {
    loadUsers();
    loadOrders();
}

void saveAllData() {
    saveUsers();
    saveOrders();
}
