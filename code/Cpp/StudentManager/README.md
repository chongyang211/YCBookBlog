# StudentManager — 学生管理通讯系统

一个用 C 风格 `struct` + 定长数组实现的菜单驱动通讯录，是《C++ 入门到精通》综合案例的**第一关**。代码刻意保留最朴素的写法（`struct` 打包数据 + 指针传参 + `switch` 分发），为的是在第 02 案例"银行账户管理"里看到类化 + 多态带来的价值。

## 功能

| 菜单项 | 功能 |
|--------|------|
| 1 添加联系人 | 输入姓名/性别/年龄/电话/地址，加入通讯录 |
| 2 显示联系人 | 打印所有联系人信息 |
| 3 删除联系人 | 按姓名删除（元素前移 + size--） |
| 4 查找联系人 | 按姓名查找并显示 |
| 5 修改联系人 | 按姓名修改信息 |
| 6 清空联系人 | 逻辑清空（size=0） |
| 0 退出通讯录 | 退出程序 |

## 目录结构

```text
StudentManager/
├── StudentManager.h         # 结构体（Person/AddressBooks）+ 函数声明
├── StudentManager.cpp       # 函数实现
├── main.cpp                 # 入口：创建通讯录 + 进入菜单循环
├── CMakeLists.txt
└── README.md
```

## 数据结构

```cpp
struct Person {
    std::string name;    // 姓名
    int sex;             // 性别：1=男 2=女
    int age;             // 年龄
    std::string phone;   // 电话
    std::string address; // 地址
};

struct AddressBooks {
    Person personArray[100];   // 联系人数组（定长）
    int size = 0;              // 当前人数
};
```

## 编译运行

```bash
cd StudentManager
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/StudentManager
```

## 核心知识点

- **`struct` 打包**：5 个字段属于"同一个实体"（联系人），用 `Person` 打包；容器 + 计数用 `AddressBooks` 打包
- **指针传参**：业务函数用 `AddressBooks*` 接收，让函数能修改外部的通讯录（值传递只改拷贝）
- **`isExist` 复用**：删除/查找/修改三个功能都要"按名字找人"，抽成一个返回下标的辅助函数（-1 表示不存在），体现 DRY 原则
- **逻辑清空**：`cleanPerson` 只把 `size = 0`，不逐个擦除，省去 100 个槽位的清理开销
- **跨平台清屏**：用 ANSI 转义 `\033[2J\033[H` 替代 `system("cls")`，`cin.get()` 替代 `system("pause")`

## 关键设计说明

1. **`isExist` 返回 `int` 而非 `bool`**：删除/修改不仅要知道"在不在"，还要知道"在第几位"，返回下标一次搞定。
2. **`modifyPerson` 用 `result` 而非 `size`**：修改是"覆盖旧位置"，不是"追加新位置"。
3. **删除时的越界防护**：前移循环上界是 `size - 1`，避免读到 `personArray[size]` 未初始化的位置。
