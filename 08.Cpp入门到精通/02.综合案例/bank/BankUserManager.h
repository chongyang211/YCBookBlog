//
// Created by 杨充 on 2025/7/16.
//
#include "string"
#include "iostream"

//最多用户不超过100
#define MAX_ACCOUNTS = 100;
//用户名最多是50位
#define MAX_LENGTH_NAME = 50;
//用户文件名称
#define FILE_NAME = "accounts.txt";


//展示菜单显示
void displayMenu();

//main中核心函数
int showMain();

//菜单选择
void showMainChoice();

// 开户
void openAccount();

// 存款
void deposit();

// 取款
void withdraw();

// 查询余额
void checkBalance();

// 用户转账
void transfer();
