// ============================================================================
// 【文件定位】StudentManager.h —— 数据结构定义 + 全部函数声明（项目的"契约层"）
// ----------------------------------------------------------------------------
// 【设计思路】
//   1. 声明与实现分离：头文件只放"有什么"（struct 布局 + 函数签名），
//      "怎么做"全部收在 StudentManager.cpp —— 修改实现不动头文件时，
//      依赖方无需重新编译；函数名/参数一旦确定，main 与实现可并行开发。
//   2. 数据结构设计来自"灵魂三问"（对应教程 04 节）：
//      ① 联系人的 5 个字段属于"同一个实体" → 用 struct Person 打包；
//      ② 容器（数组）+ 计数（size）属于"同一本通讯录" → 再打包成 AddressBooks；
//      ③ "被包含的类型必须先定义"：Person 在上、AddressBooks 在下 ——
//        编译器自上而下扫描，AddressBooks 的成员用到了 Person，
//        若顺序颠倒会直接报 field has incomplete type（不完整类型）。
// ============================================================================

#pragma once  // 头文件保护：保证同一翻译单元中被 #include 多次时内容只展开一次
             // 【技术原理】编译器内置指令（非 C++ 标准但全平台支持），
             // 等价于传统 #ifndef/#define/#endif 三件套，防止 struct/函数声明重定义。

#include <string>  // std::string：name/phone/address 三个成员依赖
                   // 【技术原理】string 自动管理堆内存（赋值/拷贝/析构全自动），
                   //             相比 C 风格 char[20] 不存在"越界写、忘记\0"的坑。

// #define MAX = 100;  //最大人数
// 【历史反例·宏定义的三宗罪】①不能带等号（展开成 MAX = = 100 语法错误）；
// ②不能加分号（分号会被一起替换到使用处）③无类型、无作用域、调试器不可见。
// ✅ 所以下面的容量常量改用"类内 static const int"——有类型、有作用域、可调试。

// 误区
// ❌ 反例：5 个数组分别存
// std::string names[100];
// int         sexes[100];
// int         ages[100];
// std::string phones[100];
// std::string addresses[100];
// int         size = 0;
// 【为什么是误区】① 删除一个联系人要同步挪动 5 个数组，漏挪一个就
// "张三的姓名配李四的电话"；② addPerson 得传 6 个参数，签名又长又乱；
// ③ 以后加"邮箱"字段要改所有函数。→ 字段的相关性必须用类型表达出来。

// 联系人结构体：姓名、性别、年龄、电话、地址
// 其实是面向对象的一种设计思想，把属性放到一起
struct Person {
    std::string name;       // 姓名
    int sex;                // 性别：1=男 2=女
    // 【设计思路】性别存 int 而非 string：取值只有两种，用编码收敛取值空间，
    //             便于校验（cin >> sex 后判断 1/2）；显示时再翻译成"男/女"。
    //             更现代的写法是 enum class Sex { Male, Female }（强类型防误用）。
    int age;                // 年龄
    std::string phone;      // 电话
    std::string address;    // 家庭住址
    // 【技术原理】struct 默认成员 public（C++ 中 struct 与 class 唯一区别是默认权限）：
    //             纯数据聚合体（POD 风格）不需要隐藏成员，直接访问最直观。
};

// 通讯录结构体：联系人数组 + 当前人数
// 增删改查需求，添加人，删除人，查找，替换等
struct AddressBooks {
    static const int MAX = 100;         // 最大联系人数量
    // 【技术原理】"类内初始化的静态整型常量"（C++ 特例）：static const int 可在
    //             声明处直接给初值，且是编译期常量——所以能当数组长度用。
    //             相比 #define：有类型（int）、有作用域（只属于 AddressBooks）、可调试。
    Person personArray[MAX];            // 联系人数组
    // 【技术原理】定长数组的长度必须是编译期常量：编译器要据此算出
    //             sizeof(AddressBooks)，才能给栈上的变量分配确定大小的内存；
    //             若写 Person personArray[]（不完整类型），编译直接失败。
    //             动态容量的方案是 std::vector<Person>（下一案例的升级点）。
    int size = 0;                       // 当前联系人数量
    // 【技术原理】类内默认初始化（C++11 NSDMI）：创建 AddressBooks 变量时
    //             size 自动为 0，main 里不用再手写 address.size = 0 —— 消灭
    //             "忘初始化导致 size 是随机值"这类未定义行为。
    // 【设计思路】size 是"有效数据的边界"：所有遍历只走 [0, size)，
    //             增=size++，删=size--，清空=size=0 —— 数组物理上永远 100 格。
};

// ===== 菜单 =====
void showMenu();                        // 显示菜单
void showMainSelect(AddressBooks* abs); // 主流程分发
// 【设计思路】主循环外置成函数：main 保持 3 行"入口即全貌"，
//             菜单循环的进出（选 0 return）不污染 main 的职责。

// ===== 业务函数 =====
// 【统一签名】全部接收 AddressBooks*：
//   因为业务函数都要"改到 main 里那份通讯录"——值传递会拷贝整个
//   （100 个联系人、数 KB 大）结构体且改动只落在副本上，函数一返回就丢；
//   传指针是把"同一块内存的地址"递进去，-> 访问的就是原件本身。
void addPerson(AddressBooks* abs);      // 添加联系人
void showPerson(AddressBooks* abs);     // 显示联系人
void deletePerson(AddressBooks* abs);   // 删除联系人
void findPerson(AddressBooks* abs);     // 查找联系人
void modifyPerson(AddressBooks* abs);   // 修改联系人
void cleanPerson(AddressBooks* abs);    // 清空联系人

// ===== 辅助函数 =====
int  isExist(const AddressBooks* abs, const std::string& name); // 按姓名查找，返回下标，-1 表示不存在
// 【修复说明】原声明少了参数一的 const，与 StudentManager.cpp 中的定义
//   （const AddressBooks*）构成两个不同的重载 → 链接时"声明的版本没定义"而报
//   Undefined symbols。两个 const 都是 const 正确性要求：函数只读不写通讯录与姓名。
// 【设计思路·isExist 的诞生】写 deletePerson 时发现"按姓名找人"这段逻辑
//   后面 find/modify 还要用 → 抽成辅助函数（DRY 原则：复用 3 次，改逻辑只改 1 处）。
// 【返回值设计】返回 int 下标而非 bool：删除/修改不仅要知道"在不在"，
//   还要知道"在第几位"，一次调用同时回答两个问题（同 std::string::find 的 npos 思路）。
void clearScreen();                    // 清屏（ANSI 转义，跨平台）
void pauseAndCls();                    // 暂停 + 清屏
