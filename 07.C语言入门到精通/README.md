# 专栏笔记总结大全


**01.基础语法**

- 1.1.5 C语⾔特点：C 语言的语法都是基础语法，不提供高级的数据结构，比如 C 语言没有“类”class，复杂结构需要自己构造。
- 1.1.6 C语言版本：
- 1.3.3 编译源文件：使用gcc编译器，将源文件hello.c编译成二进制代码。
- 1.4.2 语句块：使用一对大括号{}，组成一个块，也称为复合语句
- 1.4.5 标准库头文件：“标准库”，因为它们是写入标准的，到底包括哪些功能，应该怎么使用的，都是规定好的，这样才能保证代码的规范和可移植。
- 1.5.1 修饰符：修饰符是用来修改基本数据类型的含义的关键字，通常用于声明变量或函数的类型。
- 1.5.2 标识符：
- 1.5.3 常见关键字：

**02.数据类型**

- 2.1.1 基本数据类型：



**03.运算符**

- 3.1 自增运算符：这个 ++：是自增运算符，这个 --：是自减运算符。
- 3.6 位运算符：

**07.指针**

- 7.1 


在 C 语言中，**没有直接支持类（Class）** 的概念，因为 C 语言是一种面向过程的编程语言，而不是面向对象的编程语言（如 C++ 或 Java）。然而，可以通过结构体（`struct`）和函数指针来模拟类的行为，实现封装、继承和多态等面向对象的特性。

---

### **1. 使用结构体模拟类**

---

### **2. 使用函数指针模拟成员方法**

---

### **3. 模拟继承**

---

### **4. 模拟多态**

---

### **5. 总结**
- C 语言没有类的概念，但可以通过结构体和函数指针模拟面向对象的特性。
- 使用结构体封装数据，函数指针模拟成员方法。
- 通过嵌套结构体模拟继承，通过函数指针和类型转换模拟多态。
- 虽然 C 语言不是面向对象的语言，但通过这些技巧可以实现类似的功能。

这些方法可以帮助你在 C 语言中实现面向对象的设计模式。



在 C 语言中，**内存管理** 是一个非常重要的主题。C 语言提供了直接操作内存的能力，这使得它非常强大，但也容易引发错误（如内存泄漏、野指针等）。理解 C 语言中的内存模型和内存管理机制是编写高效、安全程序的关键。

---

### **1. 内存模型**
在 C 语言中，程序的内存通常分为以下几个区域：

#### **1.1 栈（Stack）**
- 用于存储局部变量、函数参数和函数调用的上下文。
- 内存由编译器自动分配和释放。
- 大小有限，通常较小（几 MB）。
- 访问速度快。

**示例：**
```c
void func() {
    int x = 10; // x 存储在栈中
}
```

#### **1.2 堆（Heap）**
- 用于动态内存分配。
- 内存由程序员手动管理（使用 `malloc`、`calloc`、`realloc` 和 `free`）。
- 大小较大，受系统内存限制。
- 访问速度较慢。

**示例：**
```c
int *p = (int *)malloc(sizeof(int)); // 在堆中分配内存
free(p); // 释放内存
```

#### **1.3 全局/静态区（Global/Static Area）**
- 用于存储全局变量和静态变量。
- 内存由编译器在程序启动时分配，在程序结束时释放。
- 分为初始化和未初始化两部分。

**示例：**
```c
int global_var = 10; // 全局变量，存储在全局区
static int static_var = 20; // 静态变量，存储在全局区
```

#### **1.4 常量区（Constant Area）**
- 用于存储字符串常量和 `const` 变量。
- 内存由编译器分配，程序结束时释放。
- 通常是只读的。

**示例：**
```c
const char *str = "Hello, World!"; // 字符串常量，存储在常量区
```

#### **1.5 代码区（Code Area）**
- 用于存储程序的二进制代码（指令）。
- 通常是只读的。

---

### **2. 动态内存管理**
C 语言提供了以下函数来动态管理堆内存：

#### **2.1 `malloc`**
- 分配指定大小的内存块。
- 返回指向分配内存的指针。
- 分配的内存未初始化。

**示例：**
```c
int *p = (int *)malloc(sizeof(int)); // 分配 4 字节内存
if (p == NULL) {
    perror("Failed to allocate memory");
    return 1;
}
*p = 10;
free(p); // 释放内存
```

#### **2.2 `calloc`**
- 分配指定数量和大小的内存块。
- 返回指向分配内存的指针。
- 分配的内存初始化为 0。

**示例：**
```c
int *p = (int *)calloc(5, sizeof(int)); // 分配 5 个 int 大小的内存
if (p == NULL) {
    perror("Failed to allocate memory");
    return 1;
}
free(p); // 释放内存
```

#### **2.3 `realloc`**
- 调整已分配内存块的大小。
- 返回指向新内存块的指针。
- 如果新大小大于原大小，新增部分未初始化。

**示例：**
```c
int *p = (int *)malloc(5 * sizeof(int)); // 分配 5 个 int 大小的内存
p = (int *)realloc(p, 10 * sizeof(int)); // 调整为 10 个 int 大小
if (p == NULL) {
    perror("Failed to reallocate memory");
    return 1;
}
free(p); // 释放内存
```

#### **2.4 `free`**
- 释放动态分配的内存。
- 只能释放由 `malloc`、`calloc` 或 `realloc` 分配的内存。

**示例：**
```c
int *p = (int *)malloc(sizeof(int));
free(p); // 释放内存
```

---

### **3. 常见内存问题**

#### **3.1 内存泄漏**
- 动态分配的内存未释放。
- 导致程序占用内存不断增加。

**示例：**
```c
void func() {
    int *p = (int *)malloc(sizeof(int));
    // 忘记调用 free(p);
}
```

#### **3.2 野指针**
- 指针指向已释放的内存。
- 访问野指针会导致未定义行为。

**示例：**
```c
int *p = (int *)malloc(sizeof(int));
free(p);
*p = 10; // 野指针访问
```

#### **3.3 双重释放**
- 对同一块内存多次调用 `free`。
- 导致程序崩溃。

**示例：**
```c
int *p = (int *)malloc(sizeof(int));
free(p);
free(p); // 双重释放
```

#### **3.4 越界访问**
- 访问超出分配内存范围的数据。
- 导致程序崩溃或数据损坏。

**示例：**
```c
int *p = (int *)malloc(5 * sizeof(int));
p[5] = 10; // 越界访问
free(p);
```

---

### **4. 内存管理的最佳实践**
1. **初始化指针**：在声明指针时初始化为 `NULL`。
2. **检查返回值**：在使用 `malloc`、`calloc` 或 `realloc` 后检查返回值是否为 `NULL`。
3. **及时释放内存**：动态分配的内存使用完毕后及时调用 `free`。
4. **避免野指针**：释放内存后将指针置为 `NULL`。
5. **使用工具检测内存问题**：如 Valgrind、AddressSanitizer 等。

---

### **5. 示例代码**

#### **5.1 动态数组**
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int n = 5;
    int *arr = (int *)malloc(n * sizeof(int));
    if (arr == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        arr[i] = i + 1;
    }

    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }

    free(arr);
    return 0;
}
```

#### **5.2 二维动态数组**
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    int rows = 3, cols = 4;
    int **arr = (int **)malloc(rows * sizeof(int *));
    if (arr == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }

    for (int i = 0; i < rows; i++) {
        arr[i] = (int *)malloc(cols * sizeof(int));
        if (arr[i] == NULL) {
            perror("Failed to allocate memory");
            return 1;
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            arr[i][j] = i * cols + j + 1;
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < rows; i++) {
        free(arr[i]);
    }
    free(arr);

    return 0;
}
```

---

### **6. 总结**
- C 语言的内存分为栈、堆、全局区、常量区和代码区。
- 动态内存管理使用 `malloc`、`calloc`、`realloc` 和 `free`。
- 常见内存问题包括内存泄漏、野指针、双重释放和越界访问。
- 遵循最佳实践可以避免内存问题，编写高效、安全的程序。

通过掌握这些知识，你可以在 C 语言中更好地管理内存，避免常见的错误。


在 C 语言中，**字符串** 是由字符组成的数组，以空字符 `\0` 结尾。字符串是 C 语言中处理文本数据的基本方式。由于 C 语言没有内置的字符串类型，字符串通常通过字符数组或字符指针来表示。

---

### **1. 字符串的表示**
#### **1.1 字符数组**
字符串可以通过字符数组来存储，数组的最后一个元素必须是 `\0`，表示字符串的结束。

**示例：**
```c
char str[] = "Hello, World!";
```
等价于：
```c
char str[] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\0'};
```

#### **1.2 字符指针**
字符串也可以通过字符指针来指向。

**示例：**
```c
const char *str = "Hello, World!";
```

---

### **2. 字符串的输入输出**
#### **2.1 输出字符串**
使用 `printf` 函数输出字符串：
```c
printf("%s\n", str);
```

#### **2.2 输入字符串**
使用 `scanf` 或 `gets` 函数输入字符串：
```c
char str[100];
scanf("%s", str); // 输入字符串（遇到空格停止）
gets(str);        // 输入一行字符串（不推荐使用，存在安全隐患）
```

**注意：**
- `scanf` 遇到空格会停止输入。
- `gets` 不检查缓冲区大小，可能导致缓冲区溢出，建议使用 `fgets`。

**推荐使用 `fgets`：**
```c
char str[100];
fgets(str, sizeof(str), stdin); // 安全地输入一行字符串
```

---

### **3. 字符串操作函数**
C 标准库提供了许多字符串操作函数，定义在 `<string.h>` 头文件中。

#### **3.1 字符串长度**
使用 `strlen` 函数获取字符串长度（不包括 `\0`）：
```c
size_t strlen(const char *str);
```

**示例：**
```c
char str[] = "Hello";
int len = strlen(str); // len = 5
```

#### **3.2 字符串复制**
使用 `strcpy` 或 `strncpy` 函数复制字符串：
```c
char *strcpy(char *dest, const char *src); // 复制整个字符串
char *strncpy(char *dest, const char *src, size_t n); // 复制前 n 个字符
```

**示例：**
```c
char dest[20];
strcpy(dest, "Hello"); // dest = "Hello"
strncpy(dest, "World", 3); // dest = "Worlo"
```

#### **3.3 字符串连接**
使用 `strcat` 或 `strncat` 函数连接字符串：
```c
char *strcat(char *dest, const char *src); // 连接整个字符串
char *strncat(char *dest, const char *src, size_t n); // 连接前 n 个字符
```

**示例：**
```c
char dest[20] = "Hello";
strcat(dest, " World"); // dest = "Hello World"
strncat(dest, "!!!", 2); // dest = "Hello World!!"
```

#### **3.4 字符串比较**
使用 `strcmp` 或 `strncmp` 函数比较字符串：
```c
int strcmp(const char *str1, const char *str2); // 比较整个字符串
int strncmp(const char *str1, const char *str2, size_t n); // 比较前 n 个字符
```

**返回值：**
- 如果 `str1` 小于 `str2`，返回负值。
- 如果 `str1` 等于 `str2`，返回 0。
- 如果 `str1` 大于 `str2`，返回正值。

**示例：**
```c
int result = strcmp("apple", "banana"); // result < 0
```

#### **3.5 字符串查找**
使用 `strchr` 或 `strstr` 函数查找字符或子字符串：
```c
char *strchr(const char *str, int c); // 查找字符 c 第一次出现的位置
char *strstr(const char *haystack, const char *needle); // 查找子字符串 needle
```

**示例：**
```c
char *p = strchr("Hello", 'e'); // p 指向 'e'
char *q = strstr("Hello, World!", "World"); // q 指向 "World"
```

---

### **4. 字符串与数值的转换**
C 标准库提供了将字符串转换为数值的函数，定义在 `<stdlib.h>` 头文件中。

#### **4.1 字符串转整数**
使用 `atoi` 或 `strtol` 函数：
```c
int atoi(const char *str); // 将字符串转换为整数
long strtol(const char *str, char **endptr, int base); // 将字符串转换为长整数
```

**示例：**
```c
int num = atoi("123"); // num = 123
```

#### **4.2 字符串转浮点数**
使用 `atof` 或 `strtod` 函数：
```c
double atof(const char *str); // 将字符串转换为浮点数
double strtod(const char *str, char **endptr); // 将字符串转换为双精度浮点数
```

**示例：**
```c
double num = atof("3.14"); // num = 3.14
```

---

### **5. 示例代码**

#### **5.1 字符串操作**
```c
#include <stdio.h>
#include <string.h>

int main() {
    char str1[20] = "Hello";
    char str2[20] = "World";

    // 字符串连接
    strcat(str1, " ");
    strcat(str1, str2);
    printf("%s\n", str1); // 输出 "Hello World"

    // 字符串比较
    if (strcmp(str1, "Hello World") == 0) {
        printf("Strings are equal.\n");
    }

    // 字符串查找
    char *p = strchr(str1, 'W');
    if (p != NULL) {
        printf("Found 'W' at position %ld.\n", p - str1);
    }

    return 0;
}
```

#### **5.2 字符串与数值转换**
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    char str[] = "12345";
    int num = atoi(str);
    printf("Number: %d\n", num); // 输出 "Number: 12345"

    char str2[] = "3.14";
    double num2 = atof(str2);
    printf("Number: %.2f\n", num2); // 输出 "Number: 3.14"

    return 0;
}
```

---

### **6. 总结**
- 字符串是字符数组，以 `\0` 结尾。
- 使用字符数组或字符指针表示字符串。
- C 标准库提供了丰富的字符串操作函数，如 `strlen`、`strcpy`、`strcat`、`strcmp` 等。
- 字符串与数值之间可以通过 `atoi`、`atof` 等函数进行转换。
- 注意字符串操作的安全性，避免缓冲区溢出等问题。

通过掌握这些知识，你可以在 C 语言中高效地处理字符串。




2.1 C语言的一个简单实例
2.2 实例说明
2.3 一个简单程序的结构
2.4 使程序可读的技巧
2.5 更进一步
2.6 多个函数
2.7 调试
2.8 关键字和保留标识符
2.9 关键概念
2.10 总结
2.11 复习题
2.12 编程练习

3.1 示例程序
3.2 变量与常量数据
3.3 数据：数据类型关键字
3.4 C数据类型
3.5 使用数据类型
3.6 参数和易犯的错误
3.7 另一个例子：转义序列
3.8 关键概念
3.9 总结
3.10 复习题
3.11 编程练习


4.1 前导程序
4.2 字符串简介
4.3 常量和C预处理器
4.4 研究和利用printf()和scanf()
4.5 关键概念
4.6 总结
4.7 复习题
4.8 编程练习

5.1 循环简介
5.2 基本运算符
5.3 其他运算符
5.4 表达式和语句
5.5 类型转换
5.6 带有参数的函数
5.7 一个示例程序
5.8 关键概念
5.9 总结
5.10 复习题
5.11 编程练习

6.1 再探while循环
6.2 while语句
6.4 不确定循环与计数循环
6.5 for循环
6.6 更多赋值运算符：+=、-=、*=、/=和%=
6.7 逗号运算符
6.8 退出条件循环：do while
6.9 选择哪种循环
6.10 嵌套循环
6.11 数组
6.12 使用函数返回值的循环例子
6.13 关键概念
6.14 总结
6.15 复习题


7.1 if语句
7.2 在if语句中添加else关键字
7.3 获得逻辑性
7.4 一个统计字数的程序
7.5 条件运算符?:
7.6 循环辅助手段：continue和break
7.7 多重选择：switch和break
7.8 goto语句
7.9 关键概念
7.10 总结
7.11 复习题
7.12 编程练习


8.1 单字符I/O：getchar()和putchar()
8.2 缓冲区
8.3 终止键盘输入
8.5 创建一个更友好的用户界面
8.6 输入确认
8.7 菜单浏览
8.8 关键概念
8.9 总结
8.10 复习题
8.11 编程练习

9.1 函数概述
9.2 ANSI C的函数原型
9.3 递归
9.4 多源代码文件程序的编译
9.5 地址运算符：&
9.6 改变调用函数中的变量
9.7 指针简介
9.8 关键概念
9.9 总结
9.10 复习题
9.11 编程练习

10.1 数组
10.2 多维数组
10.3 指针和数组
10.4 函数、数组和指针
10.5 指针操作
10.6 保护数组内容
10.7 指针和多维数组
10.8 变长数组（VLA）
10.9 复合文字
10.10 关键概念
10.11 总结
10.12 复习题
10.13 编程练习

12.1 存储类
12.2 存储类说明符
12.3 存储类和函数
12.4 随机数函数和静态变量
12.5 掷骰子
12.6 分配内存：malloc()和free()
12.7 ANSI C的类型限定词
12.8 关键概念
12.9 总结
12.10 复习题
12.11 编程练习


13.1 和文件进行通信
13.2 标准I/O
13.3 一个简单的文件压缩程序
13.4 文件I/O：fprintf ( )、fscanf ( )、fgets ( )和fputs ( )函数
13.5 随机存取：fseek()和ftell()函数
13.6 标准I/O内幕
13.7 其他标准I/O函数
13.8 关键概念
13.9 总结
13.10 复习题
13.11 编程练习

14.1 示例问题：创建图书目录
14.2 建立结构声明
14.3 定义结构变量
14.4 结构数组
14.5 嵌套结构
14.6 指向结构的指针
14.7 向函数传递结构信息
14.8 把结构内容保存到文件中
14.9 结构：下一步是什么
14.10 联合简介
14.11 枚举类型
14.12 typedef简介
14.13 奇特的声明
14.14 函数和指针
14.15 关键概念
14.16 总结
14.17 复习题
14.18 编程练习

15.1 二进制数、位和字节
15.2 其他基数
15.3 C的位运算符
15.4 位字段
15.5 关键概念
15.6 总结
15.7 复习题
15.8 编程练习

第16章 C预处理器和C库
16.1 翻译程序的第一步
16.2 明显常量：#define
16.3 在#define中使用参数
16.4 宏，还是函数
16.5 文件包含：#include
16.6 其他指令
16.7 内联函数
16.8 C库
16.9 数学库
16.10 通用工具库
16.11 诊断库
16.12 string.h库中的memcpy()和memmove()
16.13 可变参数：stdarg.h
16.14 关键概念
16.15 总结
16.16 复习题
16.17 编程练习


第17章 高级数据表示
17.1 研究数据表示
17.2 从数组到链表
17.3 抽象数据类型（ADT）
17.4 队列ADT
17.5 用队列进行模拟
17.6 链表与数组
17.7 二叉搜索树
17.8 其他说明
17.9 关键概念
17.10 总结
17.11 复习题
17.12 编程练习
