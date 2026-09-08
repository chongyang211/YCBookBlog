# StudentScore — Java 学生成绩管理系统

《Java 入门精通》综合案例的**第一关**。用入门第 1-6 章的知识（基础语法 / 数据类型 / 运算符 / 字符串和数组 / 流程语句 / 函数方法）完成一个菜单驱动的成绩管理系统。

代码刻意保留"**过程式 Java**"风格——`static` 方法 + 并行数组（不用 `class Student`），为的是在下一案例"银行账户管理"里对比 class 化 + 多态后的价值。

## 功能

| 菜单项 | 功能 | 核心 |
|--------|------|------|
| 1 添加学生 | 录入学号/姓名/三科成绩 | 学号查重 |
| 2 显示学生 | 表格化打印 | `String.format` 对齐 |
| 3 删除学生 | 按学号删除 | 末尾填洞法（O(1)） |
| 4 查找学生 | 按学号查找 | `-1` 哨兵约定 |
| 5 修改成绩 | 按学号 + 科目修改 | 覆盖原位置 |
| 6 统计计算 | 单人/单科平均、第一名、及格人数 | 打擂台算法 |
| 7 排序展示 | 按总分降序 | 冒泡 + 同步交换 |
| 0 退出 | — | — |

## 编译运行

```bash
cd StudentManager

# 方式一：直接编译运行
javac -encoding UTF-8 StudentScore.java
java StudentScore

# 方式二：用 Makefile（一键编译 + 运行 + 清理）
make run
make clean
```

## 数据结构（并行数组）

```java
static final int MAX_STUDENTS = 50;
static int[]      ids    = new int[MAX_STUDENTS];       // 学号 int
static String[]   names  = new String[MAX_STUDENTS];    // 姓名 String
static double[][] scores = new double[MAX_STUDENTS][3]; // 三科分数
static int        count  = 0;                           // 计数器
```

三个类型不同的字段用**独立数组 + 同一下标 `i` 关联**，模拟"对象数组"。这是 OOP 之前最经典的数据组织方式。

## 核心知识点

- **并行数组**：类型分开 + 下标关联（写一遍痛点，才能体会 class 封装的甜）
- **`String.equals` vs `==`**：`==` 比引用地址，`equals` 比内容（Java 新手 90% 的坑）
- **末尾填洞法**：删除 O(1)，不保序（配合 `names[last] = null` 帮助 GC）
- **`-1` 哨兵约定**：找不到返回 -1，调用方必须先判断
- **整数除法陷阱**：平均分用 `double`，否则 `253/3 = 84` 砍掉小数
- **冒泡 + swapAll**：并行数组排序必须三个数组同步交换
- **防御式输入**：`isInteger`/`isDouble` 提前拦截，`parseInt` 永不抛异常（不用 try-catch）

## 关键陷阱（Java 新手经典坑）

1. **字符串用 `==` 比较** → 永远 false → 改用 `equals`
2. **排序只交换 `scores`** → 学号/姓名/分数错位 → 用 `swapAll` 三换一
3. **修改成绩写成 `scores[count]`** → 落到未分配槽位 → 应写 `scores[idx]`
4. **`nextInt` 混用 `nextLine`** → 读到空行 → 全程只用 `nextLine`
