#pragma once

#include <string>

// 联系人结构体：姓名、性别、年龄、电话、地址
struct Person {
    std::string name;       // 姓名
    int sex;                // 性别：1=男 2=女
    int age;                // 年龄
    std::string phone;      // 电话
    std::string address;    // 家庭住址
};

// 通讯录结构体：联系人数组 + 当前人数
struct AddressBooks {
    static const int MAX = 100;         // 最大联系人数量
    Person personArray[MAX];            // 联系人数组
    int size = 0;                       // 当前联系人数量
};

// ===== 菜单 =====
void showMenu();                        // 显示菜单
void showMainSelect(AddressBooks* abs); // 主流程分发

// ===== 业务函数 =====
void addPerson(AddressBooks* abs);      // 添加联系人
void showPerson(AddressBooks* abs);     // 显示联系人
void deletePerson(AddressBooks* abs);   // 删除联系人
void findPerson(AddressBooks* abs);     // 查找联系人
void modifyPerson(AddressBooks* abs);   // 修改联系人
void cleanPerson(AddressBooks* abs);    // 清空联系人

// ===== 辅助函数 =====
int  isExist(AddressBooks* abs, const std::string& name); // 按姓名查找，返回下标，-1 表示不存在
void clearScreen();                    // 清屏（ANSI 转义，跨平台）
void pauseAndCls();                    // 暂停 + 清屏
