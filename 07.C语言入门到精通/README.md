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




以下是关于 C 语言线程的详细说明和示例。

---

### **1. 线程的基本概念**
- **线程**：线程是程序执行的最小单元，一个进程可以包含多个线程，线程共享进程的内存空间。
- **多线程**：多线程允许程序同时执行多个任务，提高程序的并发性和效率。

---

### **2. 使用 pthread 创建线程**
#### **2.1 头文件**
```c
#include <pthread.h>
```

#### **2.2 创建线程**
使用 `pthread_create` 函数创建线程：
```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
```
- **参数**：
    - `thread`：指向线程标识符的指针。
    - `attr`：线程属性，通常为 `NULL`（使用默认属性）。
    - `start_routine`：线程执行的函数。
    - `arg`：传递给线程函数的参数。
- **返回值**：成功返回 `0`，失败返回错误码。

#### **2.3 示例代码**
```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// 线程函数
void *thread_function(void *arg) {
    int *value = (int *)arg;
    printf("Thread is running, value = %d\n", *value);
    sleep(2); // 模拟线程执行
    printf("Thread is done\n");
    return NULL;
}

int main() {
    pthread_t thread;
    int value = 42;

    // 创建线程
    if (pthread_create(&thread, NULL, thread_function, &value) != 0) {
        perror("Failed to create thread");
        return 1;
    }

    // 等待线程结束
    if (pthread_join(thread, NULL) != 0) {
        perror("Failed to join thread");
        return 1;
    }

    printf("Main thread is done\n");
    return 0;
}
```

---

### **3. 线程同步**
多线程程序中，线程之间可能会竞争共享资源，导致数据不一致。常用的同步机制包括 **互斥锁** 和 **条件变量**。

#### **3.1 互斥锁（Mutex）**
互斥锁用于保护共享资源，确保同一时间只有一个线程访问资源。

##### **示例代码**
```c
#include <stdio.h>
#include <pthread.h>

int shared_value = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_function(void *arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex); // 加锁
        shared_value++;
        pthread_mutex_unlock(&mutex); // 解锁
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, thread_function, NULL);
    pthread_create(&thread2, NULL, thread_function, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Shared value = %d\n", shared_value);
    return 0;
}
```

#### **3.2 条件变量（Condition Variable）**
条件变量用于线程之间的通信，通常与互斥锁一起使用。

##### **示例代码**
```c
#include <stdio.h>
#include <pthread.h>

int ready = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    pthread_mutex_lock(&mutex);
    ready = 1;
    printf("Producer: Data is ready\n");
    pthread_cond_signal(&cond); // 通知消费者
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *consumer(void *arg) {
    pthread_mutex_lock(&mutex);
    while (!ready) {
        pthread_cond_wait(&cond, &mutex); // 等待条件变量
    }
    printf("Consumer: Data is consumed\n");
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, consumer, NULL);
    pthread_create(&thread2, NULL, producer, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
```

---

### **4. 线程的终止**
#### **4.1 线程正常终止**
线程函数执行完毕后，线程会自动终止。

#### **4.2 线程强制终止**
使用 `pthread_cancel` 强制终止线程：
```c
pthread_cancel(thread);
```

#### **4.3 线程分离**
使用 `pthread_detach` 将线程设置为分离状态，线程结束后自动释放资源：
```c
pthread_detach(thread);
```

---

### **5. 线程属性**
可以通过 `pthread_attr_t` 设置线程属性，如栈大小、调度策略等。

##### **示例代码**
```c
pthread_attr_t attr;
pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // 设置线程为分离状态
pthread_create(&thread, &attr, thread_function, NULL);
pthread_attr_destroy(&attr);
```

---

### **6. 线程的返回值**
线程函数可以通过 `return` 或 `pthread_exit` 返回结果，主线程使用 `pthread_join` 获取返回值。

##### **示例代码**
```c
void *thread_function(void *arg) {
    int *result = malloc(sizeof(int));
    *result = 42;
    pthread_exit(result); // 返回结果
}

int main() {
    pthread_t thread;
    void *retval;

    pthread_create(&thread, NULL, thread_function, NULL);
    pthread_join(thread, &retval);

    printf("Thread returned: %d\n", *(int *)retval);
    free(retval);
    return 0;
}
```

---

### **7. 总结**
- 使用 `pthread_create` 创建线程。
- 使用 `pthread_join` 等待线程结束。
- 使用互斥锁和条件变量实现线程同步。
- 通过 `pthread_attr_t` 设置线程属性。
- 线程函数可以通过 `return` 或 `pthread_exit` 返回结果。

通过合理使用线程，可以提高程序的并发性和效率，但需要注意线程安全和同步问题。


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
