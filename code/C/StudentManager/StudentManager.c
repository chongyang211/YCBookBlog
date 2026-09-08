#include "StudentManager.h"

Node *g_pHead = NULL;   // 头指针初始为空

// ===== 菜单与辅助 =====

void showMenu() {
    printf("***************************************\n");
    printf("*   欢迎使用学生管理系统 v1.0         *\n");
    printf("***************************************\n");
    printf("*   1.录入学生信息                    *\n");
    printf("*   2.打印学生信息                    *\n");
    printf("*   3.保存学生信息                    *\n");
    printf("*   4.读取学生信息                    *\n");
    printf("*   5.统计学生人数                    *\n");
    printf("*   6.查找学生信息                    *\n");
    printf("*   7.修改学生信息                    *\n");
    printf("*   8.删除学生信息                    *\n");
    printf("*   0.退出系统                        *\n");
    printf("***************************************\n");
    printf("请选择: ");
}

void pauseProgram() {
    printf("按回车键继续...\n");
    int c;
    // 清空缓冲区残留，直到换行（用户回车）或 EOF
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void clearScreen() {
    printf("\033[2J\033[H");     // ANSI 清屏（Linux/macOS）
}

// ===== 链表辅助 =====

// 链表尾插：找到最后一个节点，把新节点接上去
void addNodeToList(Node *pNewNode) {
    if (g_pHead == NULL) {
        g_pHead = pNewNode;
        return;
    }
    Node *p = g_pHead;
    while (p->pNext != NULL) {
        p = p->pNext;
    }
    p->pNext = pNewNode;
}

// 释放整个链表（退出时调用，避免内存泄漏）
void freeList() {
    while (g_pHead != NULL) {
        Node *temp = g_pHead;
        g_pHead = g_pHead->pNext;
        free(temp);
    }
}

// ===== 业务函数 =====

void InputStudent() {
    // malloc：在堆上分配一个 Node
    Node *pNewNode = (Node *)malloc(sizeof(Node));
    if (pNewNode == NULL) {
        printf("内存分配失败！\n");
        return;
    }
    // ⚠️ 必须初始化 pNext = NULL，否则是野指针
    pNewNode->pNext = NULL;

    printf("请输入学生姓名: ");
    scanf("%19s", pNewNode->stu.name);
    printf("请输入性别(男/女): ");
    scanf("%3s", pNewNode->stu.sex);
    printf("请输入年龄: ");
    scanf("%d", &pNewNode->stu.age);
    printf("请输入学号: ");
    scanf("%d", &pNewNode->stu.stuNo);
    printf("请输入成绩: ");
    scanf("%d", &pNewNode->stu.score);

    addNodeToList(pNewNode);
    printf(">>> 学生信息录入成功！\n");
    pauseProgram();
    clearScreen();
}

void PrintStudent() {
    clearScreen();
    Node *p = g_pHead;
    if (p == NULL) {
        printf("系统中暂无学生信息。\n");
        pauseProgram();
        return;
    }
    printf("学号\t姓名\t性别\t年龄\t成绩\n");
    printf("-----------------------------------------\n");
    while (p != NULL) {
        printf("%d\t%s\t%s\t%d\t%d\n",
               p->stu.stuNo, p->stu.name,
               p->stu.sex, p->stu.age, p->stu.score);
        p = p->pNext;
    }
    printf("-----------------------------------------\n");
    pauseProgram();
}

void SaveToFile() {
    FILE *pFile = fopen("stuinfo.dat", "wb");   // wb = 二进制写
    if (pFile == NULL) {
        printf("无法打开文件！\n");
        return;
    }
    Node *p = g_pHead;
    while (p != NULL) {
        // ⭐ 只写 Student 数据，不写 pNext 指针（避免悬垂指针）
        fwrite(&p->stu, sizeof(Student), 1, pFile);
        p = p->pNext;
    }
    fclose(pFile);
    printf(">>> 数据保存成功！（stuinfo.dat）\n");
    pauseProgram();
}

void LoadFromFile() {
    FILE *pFile = fopen("stuinfo.dat", "rb");
    if (pFile == NULL) {
        printf("未找到数据文件，将创建新通讯录。\n");
        return;
    }
    // 先清空现有链表，避免重复加载
    freeList();

    Student tempStu;
    while (fread(&tempStu, sizeof(Student), 1, pFile) == 1) {
        Node *pNewNode = (Node *)malloc(sizeof(Node));
        if (pNewNode == NULL) {
            printf("内存分配失败！\n");
            fclose(pFile);
            return;
        }
        pNewNode->stu = tempStu;
        pNewNode->pNext = NULL;
        addNodeToList(pNewNode);
    }
    fclose(pFile);
    printf(">>> 数据加载成功！\n");
}

void CountStudent() {
    int count = 0;
    Node *p = g_pHead;
    while (p != NULL) {
        count++;
        p = p->pNext;
    }
    printf("当前学生总人数: %d\n", count);
    pauseProgram();
}

void FindStudent() {
    int stuNo;
    printf("请输入要查找的学号: ");
    scanf("%d", &stuNo);

    Node *p = g_pHead;
    while (p != NULL) {
        if (p->stu.stuNo == stuNo) {
            printf("学号\t姓名\t性别\t年龄\t成绩\n");
            printf("%d\t%s\t%s\t%d\t%d\n",
                   p->stu.stuNo, p->stu.name,
                   p->stu.sex, p->stu.age, p->stu.score);
            pauseProgram();
            return;
        }
        p = p->pNext;
    }
    printf("未找到该学号的学生！\n");
    pauseProgram();
}

void ModifyStudent() {
    int stuNo;
    printf("请输入要修改的学号: ");
    scanf("%d", &stuNo);

    Node *p = g_pHead;
    while (p != NULL) {
        if (p->stu.stuNo == stuNo) {
            printf("当前信息: %s %s %d %d\n",
                   p->stu.name, p->stu.sex,
                   p->stu.age, p->stu.score);
            printf("请输入新姓名: "); scanf("%19s", p->stu.name);
            printf("请输入新性别: "); scanf("%3s", p->stu.sex);
            printf("请输入新年龄: "); scanf("%d", &p->stu.age);
            printf("请输入新成绩: "); scanf("%d", &p->stu.score);
            printf(">>> 修改成功！\n");
            pauseProgram();
            return;
        }
        p = p->pNext;
    }
    printf("未找到该学号的学生！\n");
    pauseProgram();
}

void DeleteStudent() {
    int stuNo;
    printf("请输入要删除的学号: ");
    scanf("%d", &stuNo);

    Node *p = g_pHead;
    Node *prev = NULL;

    while (p != NULL) {
        if (p->stu.stuNo == stuNo) {
            if (prev == NULL) {
                g_pHead = p->pNext;      // 删除头节点
            } else {
                prev->pNext = p->pNext;  // 删除中间/尾节点
            }
            free(p);                     // ⭐ 必须 free，否则内存泄漏
            printf(">>> 删除成功！\n");
            pauseProgram();
            return;
        }
        prev = p;
        p = p->pNext;
    }
    printf("未找到该学号的学生！\n");
    pauseProgram();
}
