#ifndef STUDENT_MANAGER_H
#define STUDENT_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 学生结构体：姓名、性别、年龄、学号、成绩
typedef struct {
    char  name[20];    // 姓名
    char  sex[4];      // 性别（男/女）
    int   age;         // 年龄
    int   stuNo;       // 学号
    int   score;       // 成绩
} Student;

// 链表节点：学生信息 + 指向下一个节点的指针
typedef struct tagNode {
    Student         stu;      // 学生信息
    struct tagNode *pNext;    // 指向下一个节点
} Node;

// 全局头指针——整个链表从这里开始（C 没有类成员变量，用全局指针当容器）
extern Node *g_pHead;

// ===== 菜单与辅助 =====
void showMenu();        // 显示菜单
void pauseProgram();    // 暂停，等待回车
void clearScreen();     // 清屏（ANSI 转义）

// ===== 业务函数 =====
void InputStudent();    // 录入学生信息（malloc + 尾插）
void PrintStudent();    // 打印所有学生
void SaveToFile();      // 保存到文件（二进制）
void LoadFromFile();    // 从文件加载
void CountStudent();    // 统计人数
void FindStudent();     // 按学号查找
void ModifyStudent();   // 按学号修改
void DeleteStudent();   // 按学号删除（free）

// ===== 链表辅助 =====
void addNodeToList(Node *pNewNode);   // 尾插
void freeList();                       // 释放整个链表

#endif // STUDENT_MANAGER_H
