#ifndef SCHOOL_MANAGER_H
#define SCHOOL_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS    100
#define MAX_ORDERS   500
#define NAME_LEN      50
#define USERS_FILE   "users.txt"
#define ORDERS_FILE  "orders.txt"

// ===== 枚举定义 =====
typedef enum {
    ROLE_STUDENT = 1,
    ROLE_TEACHER = 2,
    ROLE_ADMIN   = 3
} UserRole;

typedef enum {
    STATUS_PENDING  = 0,   // 待审核
    STATUS_APPROVED = 1,   // 已通过
    STATUS_REJECTED = -1,  // 已拒绝
    STATUS_CANCELED = 2    // 已取消
} OrderStatus;

// ===== 结构体定义 =====
typedef struct {
    int      id;
    char     name[NAME_LEN];
    char     password[20];
    UserRole role;         // 一个字段区分三种身份
} User;

typedef struct {
    int         orderId;
    int         studentId;       // 外键：哪个学生提交的
    char        studentName[NAME_LEN];  // 冗余，减少一次遍历
    int         roomId;          // 外键：哪个机房
    char        date[11];        // YYYY-MM-DD
    char        timeSlot[10];    // 上午/下午
    int         peopleCount;
    OrderStatus status;
} Order;

typedef struct {
    int roomId;
    int capacity;
} Room;

// ===== 全局数据 =====
extern User  users[MAX_USERS];
extern int   userCount;

extern Order orders[MAX_ORDERS];
extern int   orderCount;
extern int   orderIdCounter;

extern Room rooms[3];
extern int  roomCount;

// ===== 菜单与登录 =====
void showMainMenu();
int  findUser(int id, const char *password);
int  login(UserRole selectedRole, UserRole *outRole, char *outName, int *outId);

// ===== 学生功能 =====
void studentMenu(int userId, const char *name);
void studentApply(int userId, const char *name);
void studentView(int userId);
void studentCancel(int userId);

// ===== 教师功能 =====
void teacherMenu(int userId, const char *name);
void teacherViewAll();
void teacherReview();

// ===== 管理员功能 =====
void adminMenu(int userId, const char *name);
void adminAddUser();
void adminViewAll();
void adminClearOrders();

// ===== 文件持久化 =====
void saveUsers();
void loadUsers();
void saveOrders();
void loadOrders();
void loadAllData();
void saveAllData();

// ===== 辅助 =====
const char *statusText(OrderStatus s);
const char *roleText(UserRole r);

#endif // SCHOOL_MANAGER_H
