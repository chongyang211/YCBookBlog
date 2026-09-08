# StudentManager — C语言 学生管理通讯录系统

一个用纯 C 语言（`printf/scanf/malloc/free/fopen/fwrite/fread`）实现的学生管理系统，是《C 语言入门精通》综合案例的**第一关**。与 C++ 版同一案例的核心差异在于：**链表替代数组**、**手动内存管理**、**文件二进制持久化**——让你直观体会 C++ 的封装、RAII、STL 容器替 C 语言省掉了多少手动劳动。

## 功能

| 菜单项 | 功能 |
|--------|------|
| 1 录入学生信息 | `malloc` 一个节点，链表尾插 |
| 2 打印学生信息 | 遍历链表，格式化输出 |
| 3 保存学生信息 | `fwrite` 二进制写文件 |
| 4 读取学生信息 | `fread` 从文件加载 |
| 5 统计学生人数 | 遍历链表计数 |
| 6 查找学生信息 | 按学号查找 |
| 7 修改学生信息 | 按学号修改 |
| 8 删除学生信息 | 链表删除 + `free` |
| 0 退出系统 | 释放整个链表 |

## 目录结构

```text
StudentManager/
├── StudentManager.h         # 结构体（Student/Node）+ 函数声明
├── StudentManager.c         # 函数实现
├── main.c                   # 入口：菜单循环 + 退出清理
├── CMakeLists.txt
└── README.md
```

## 数据结构

```c
typedef struct {
    char  name[20];    // 姓名
    char  sex[4];      // 性别
    int   age;         // 年龄
    int   stuNo;       // 学号
    int   score;       // 成绩
} Student;

typedef struct tagNode {
    Student         stu;      // 学生信息
    struct tagNode *pNext;    // 指向下一个节点
} Node;
```

## 编译运行

```bash
cd StudentManager
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/StudentManager
```

## 核心知识点

- **链表替代数组**：C 没有 `vector`，链表 O(1) 尾插、永不溢出，每次只 `malloc` 一个节点
- **手动内存管理**：`malloc` 必须配对 `free`，删除节点只改指针不 `free` 会内存泄漏
- **三种位置删除**：头节点改 `g_pHead`、中间/尾节点改 `prev->pNext`，逻辑不同
- **文件二进制读写**：`fwrite` 只存 `Student`（不含 `pNext` 指针），加载时重建链表

## 关键陷阱（C 语言经典坑）

1. **忘记 `pNext = NULL`**：野指针 → 遍历时 segfault
2. **删除只改指针不 `free`**：内存泄漏（Valgrind 报 `definitely lost`）
3. **`scanf("%d")` 残留 `\n`**：会污染下一次 `getchar()`，需清空缓冲区
4. **`fwrite(sizeof(Node))`**：会写入 `pNext` 指针字段，读回来是悬垂指针——必须只存 `Student`
