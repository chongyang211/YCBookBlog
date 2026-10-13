// ============================================================================
// 【文件定位】StudentManager.cpp —— 全部函数的实现（项目的"实现层"）
// ----------------------------------------------------------------------------
// 【设计思路】
//   1. 每个业务函数只做一件事（添加/显示/删除/查找/修改/清空），
//      由 showMainSelect 统一分发 —— "菜单是所有功能的入口"。
//   2. 共性子问题下沉为辅助函数：isExist（按姓名找下标）、
//      pauseAndCls / clearScreen（跨平台交互），业务函数只组合不重复。
//   3. 文件内保留 addPerson1 / addPerson2 两个教学反例（不被调用），
//      与正牌 addPerson 对照，直观展示"指针传参 vs 值传参"的差异。
// ============================================================================

#include "StudentManager.h"  // 本项目的契约：结构体定义 + 函数声明（必须最先包含）
#include <iostream>          // cin/cout 标准输入输出流
#include <limits>            // std::numeric_limits：取 streamsize 的最大值，配合 cin.ignore 清缓冲

using namespace std;         // 教学简化：省略 std:: 前缀（生产代码不推荐，易引发命名冲突）

// ===== 辅助函数 =====

// 清屏：ANSI 转义序列（跨平台，不依赖 system("cls")）
// 【技术原理】"\033" 是 ESC（转义）字符的八进制写法，终端收到后把后续文本
//   当作控制指令而非普通字符打印：
//   - [2J = Erase in Display，擦除整个屏幕；
//   - [H  = Cursor Home，光标移回左上角（只清屏不移光标会"在新位置继续打印"）。
//   flush 立即刷新输出缓冲：转义序列只有几个字节，若滞留在缓冲区里不生效。
// 【对比】system("cls") 起一个子进程执行 shell 命令：慢、且 Windows/macOS 命令不同；
//   ANSI 转义在现代终端（含 Win10+ 的 Windows Terminal）通用。
void clearScreen() {
    cout << "\033[2J\033[H" << flush;
}

// 暂停 + 清屏：替代 Windows 的 system("pause") + system("cls")
// 【技术原理·为什么需要 ignore】cin >> x 读取数字后，用户按下的回车（'\n'）
//   仍残留在输入缓冲区里；直接 cin.get() 会"秒吃"这个残留换行、根本不等用户 ——
//   所以必须先 cin.ignore 把"从当前位置到第一个 '\n'（含）"的全部残留清空。
//   numeric_limits<streamsize>::max() 是哨兵值，表示"最多可以丢弃这么多字符"。
void pauseAndCls() {
    cout << "按回车键继续..." << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');   // 清掉缓冲区残留的换行
    cin.get();                                             // 等待用户回车
    clearScreen();                                         // 回车后清屏，回到干净的菜单界面
}

// ===== 菜单 =====

// 菜单界面：所有功能的统一入口（只负责展示，不含任何业务逻辑）
// 【技术原理】endl = "\n" + flush（换行并刷新缓冲），循环里的提示都能立刻可见。
void showMenu() {
    cout << "***************************" << endl;
    cout << "*****  1、添加联系人  *****" << endl;
    cout << "*****  2、显示联系人  *****" << endl;
    cout << "*****  3、删除联系人  *****" << endl;
    cout << "*****  4、查找联系人  *****" << endl;
    cout << "*****  5、修改联系人  *****" << endl;
    cout << "*****  6、清空联系人  *****" << endl;
    cout << "*****  0、退出通讯录  *****" << endl;
    cout << "***************************" << endl;
}

// 主流程分发：事件循环（打印菜单 → 读选项 → switch 分发 → 循环往复）
// 【设计思路】while(true) 保证程序"常驻"：业务函数返回后回到菜单等下一次选择；
//   唯一出口是 case 0 的 return（控制流回到 main 后程序顺势结束）。
// 【传参】接收 AddressBooks*（指针）：把 main 那份通讯录的地址递进来，
//   各业务函数经由它操作同一块内存 —— 改动能被 main 看到（值传递做不到）。
void showMainSelect(AddressBooks* books) {
    int select = 0;        // 用户选择的菜单项编号
    while (true) {         // 事件循环：条件恒真，靠 case 0 的 return 退出
        showMenu();        // 每轮循环先打印菜单
        cout << "请选择功能：" << endl;
        cin >> select;     // 流提取：按 int 解析用户输入（输入非数字会置失败位，教学版未处理）

        switch (select) {  // 整数分发首选 switch：比 if-else 链清晰，编译器可优化成跳转表
            case 1:  // 添加联系人
                addPerson(books);   // 把同一个指针继续下传，业务函数改的就是 main 的原件
                break;              // break 只跳出 switch，回到 while 重新显示菜单
            case 2:  // 显示联系人
                showPerson(books);
                break;
            case 3:  // 删除联系人
                deletePerson(books);
                break;
            case 4:  // 查找联系人
                findPerson(books);
                break;
            case 5:  // 修改联系人
                modifyPerson(books);
                break;
            case 6:  // 清空联系人
                cleanPerson(books);
                break;
            case 0:  // 退出通讯录
                cout << "欢迎下次使用" << endl;
                std::cout << "Press Enter to continue..."  << endl;
                // 【已知细节】cin >> select 之后缓冲区里残留一个 '\n'，
                //   cin.get() 会立即吃掉它、几乎不等用户 —— 但程序马上退出，无伤大雅；
                //   需要真正"等回车"的场景请仿照 pauseAndCls 先 ignore 再 get。
                std::cin.get(); // 等待用户输入字符
                return;         // 结束 showMainSelect，控制流回到 main —— 循环的唯一出口
            default:            // 兜底分支：0~6 之外的输入给明确提示（防呆），不退出程序
                cout << "输入有误，请重新输入" << endl;
                break;
        }
    }
}

// ===== 业务函数 =====
// 【教学反例 1】addPerson1：只搭了骨架（判满 + 取空槽引用），函数体未完成。
//   保留它是为了展示 addPerson 的"施工过程"：
//   先判满 → 再拿到"第 size 号空槽"的引用 → 后续逐字段赋值。
//   未在 .h 中声明、无人调用，编译器不会为"没人用的函数"报错。
void addPerson1(AddressBooks* books) {
    if (books->size >= AddressBooks::MAX) {     // 容量守卫：size 达到上限（100）就拒绝写入
        cout << "通讯录已经满了，无法添加" << endl;
        return;                                 // 提前返回（卫语句），避免越界写 personArray[100]
    }
    // 【技术原理】[[maybe_unused]]（C++17）：显式告诉编译器"这个变量故意不用"，
    //   消除 -Wall/-Wextra 下的 unused 警告 —— 教学占位代码的标准处理手法。
    [[maybe_unused]] Person& p = books->personArray[books->size];    // 引用"下标为 size 的空槽"——写法与正式版一致
}

// 【教学反例 2】addPerson2：值传参（AddressBooks books，没有 *）—— 添加注定失败。
//   ❌ 错在哪：调用时整个通讯录（100 个 Person、数 KB）被完整拷贝一份副本，
//   之后所有操作改的都是副本；函数一结束副本销毁，main 里的原通讯录分毫未动。
//   ✅ 对照：正式版 addPerson 用指针传参，把"同一块内存的地址"递进来，
//   -> 访问的就是原件本身，改动才能保留（详见教程 4.4 节的传参对比表）。
void addPerson2(AddressBooks books) {
    if (books.size >= AddressBooks::MAX) {
        cout << "通讯录已经满了，无法添加" << endl;
        return;
    }
    [[maybe_unused]] Person& p = books.personArray[books.size];
    // 因为函数要改到调用方那份原始数据，而值传递只能改到副本。
}


// 添加联系人：向 personArray[size] 空槽逐字段写入，写完后 size++
// 【流程设计】"三步走"：① 判满卫语句 → ② 取空槽引用、逐字段输入 → ③ 最后才 size++。
//   size++ 必须放在最后：若中途出错/提前返回，size 不变，"半成品记录"不会被
//   后续的显示/查找遍历到（遍历只走 [0, size)），数据一致性有天然保障。
void addPerson(AddressBooks* books) {
    if (books->size >= AddressBooks::MAX) {    // 判满：size 已达容量上限就拒绝（防数组越界）
        cout << "通讯录已经满了，无法添加" << endl;
        return;
    }

    Person& p = books->personArray[books->size];   // 引用当前空槽位，后续直接赋值
    // 【技术原理】引用 = 已存在对象的别名，对 p 的读写就是对该槽位的读写，零拷贝；
    //   若不用引用，下面 5 次输入都要写 books->personArray[books->size].xxx —— 又长又易错。
    // 【注意】此时 size 还没 ++，p 指向"最后一个有效元素之后"的空槽。

    cout << "请输入姓名：" << endl;
    cin >> p.name;      // 流提取直接写进结构体成员（string 自动按需扩容，无长度上限烦恼）

    cout << "请输入性别：" << endl;
    cout << "1 -- 男" << endl;
    cout << "2 -- 女" << endl;
    int sex = 0;                        // 先给安全初值 0（无效值），再进循环校验
    while (true) {                      // 输入校验循环：不合法就永远出不去（把关到底）
        cin >> sex;
        if (sex == 1 || sex == 2) {     // 白名单校验：只接受 1/2 两种编码
            p.sex = sex;                // 合法才写入成员 —— 无效输入永远不会污染数据
            break;                      // 跳出校验循环
        }
        cout << "输入性别有误，请重新输入" << endl;
    }

    cout << "请输入年龄：" << endl;
    cin >> p.age;       // 年龄未做范围校验（教学简化；生产应校验 0~150 并处理非法输入）

    cout << "请输入电话：" << endl;
    cin >> p.phone;     // cin >> 以空白为界：含空格的地址会被截断（教学用 getline 可解）

    cout << "请输入家庭地址：" << endl;
    cin >> p.address;

    books->size++;      // 全部字段写完才自增计数 —— 新联系人自此进入 [0, size) 的"可见区"
    cout << "添加用户：" << p.name << " 成功。当前人数：" << books->size << endl;
    pauseAndCls();      // 等用户确认回车后清屏，再回到主菜单（交互节奏统一）
}

// 显示联系人：遍历 [0, size) 打印全部记录
// 【设计思路】这是所有功能的"验证手段"——增/删/改/清空有没有生效，全靠它看出来。
void showPerson(AddressBooks* books) {
    if (books->size == 0) {         // 空表守卫：先处理边界情况，避免打印一个孤零零的表头
        cout << "通讯录为空，没有联系人" << endl;
        pauseAndCls();
        return;
    }
    for (int i = 0; i < books->size; i++) {      // 只遍历有效区 [0, size)，不碰未写入的空槽
        const Person& p = books->personArray[i];    // const 引用：零拷贝访问元素，且承诺只读不写
        cout << "姓名：" << p.name << "\t";          // "\t" 制表符做简单对齐（中英文宽度不一，近似整齐）
        // 三目运算符把性别编码（1/2）翻译成可读文案（男/女）：存储用编码、显示用翻译
        cout << "性别：" << (p.sex == 1 ? "男" : "女") << "\t";
        cout << "年龄：" << p.age << "\t";
        cout << "电话：" << p.phone << "\t";
        cout << "住址：" << p.address << endl;       // 每条记录以换行收尾
    }
    pauseAndCls();
}

// 删除联系人：定位 → 整体前移补洞 → size--
// 【核心机制】定长数组不能"抽掉一格"，只能用"后面的元素依次覆盖前面"来模拟删除。
void deletePerson(AddressBooks* books) {
    cout << "请输入您要删除的联系人姓名：" << endl;
    string name;
    cin >> name;

    int result = isExist(books, name);    // 复用辅助函数：一次拿到"在不在 + 在第几位"
    if (result == -1) {                    // -1 哨兵值：约定"不存在"
        cout << "查无此人" << endl;
        pauseAndCls();
        return;                            // 卫语句：查无此人就没有后续可言
    }

    // 从 result 开始，后面的元素整体前移一位
    // 注意上界是 size-1：避免访问 personArray[size]（未写入的位置）
    // 【越界陷阱】若上界写成 i < size，最后一轮会读 personArray[size] ——
    //   那是"从未写入过的内存"（未初始化的 string），行为未定义；
    //   本项目因定长数组物理空间存在而"碰巧没崩"，换成 vector 必炸。
    for (int i = result; i < books->size - 1; i++) {
        books->personArray[i] = books->personArray[i + 1];    // 结构体整体赋值（编译器生成的逐成员拷贝）
    }
    books->size--;    // 逻辑删除：可见区缩短一格；最后一格的旧数据仍在内存里，但不可达
    cout << "删除成功" << endl;
    pauseAndCls();
}

// 查找联系人：isExist 的第二次复用 —— 不用再写一遍 for 循环找人
void findPerson(AddressBooks* books) {
    cout << "请输入您要查找的联系人姓名：" << endl;
    string name;
    cin >> name;

    int result = isExist(books, name);
    if (result == -1) {              // 未命中分支：明确提示，不带出任何数据
        cout << "查无此人" << endl;
        pauseAndCls();
        return;
    }

    const Person& p = books->personArray[result];    // 命中：引用下标 result 处的元素，const 只读
    cout << "姓名：" << p.name << "\t";
    cout << "性别：" << (p.sex == 1 ? "男" : "女") << "\t";
    cout << "年龄：" << p.age << "\t";
    cout << "电话：" << p.phone << "\t";
    cout << "住址：" << p.address << endl;
    pauseAndCls();
}

// 修改联系人：isExist 的第三次复用；要点是"覆盖原位置"而非"追加新位置"
void modifyPerson(AddressBooks* books) {
    cout << "请输入您要修改的联系人姓名：" << endl;
    string name;
    cin >> name;

    int result = isExist(books, name);
    if (result == -1) {
        cout << "查无此人" << endl;
        pauseAndCls();
        return;
    }

    Person& p = books->personArray[result];   // ⭐ 引用旧位置，覆盖原值而非追加
    // 【下标陷阱】必须用 isExist 返回的 result（旧位置），绝不能写 personArray[size]：
    //   后者会把新值落进"尚未分配的下一个空槽"，老记录原封不动 —— 看起来"修改无效"，
    //   且 size==100 时直接越界。口诀：修改 = 覆盖 result，添加 = 占用 size。

    cout << "请输入姓名：" << endl;
    cin >> p.name;      // 直接对旧槽位的成员重新赋值（覆盖），size 全程不变

    cout << "请输入性别：" << endl;
    cout << "1 -- 男" << endl;
    cout << "2 -- 女" << endl;
    int sex = 0;
    while (true) {      // 与添加功能相同的输入校验循环（两处代码结构一致，是"复制型重复"）
        cin >> sex;
        if (sex == 1 || sex == 2) {
            p.sex = sex;
            break;
        }
        cout << "输入性别有误，请重新输入" << endl;
    }

    cout << "请输入年龄：" << endl;
    cin >> p.age;

    cout << "请输入电话：" << endl;
    cin >> p.phone;

    cout << "请输入家庭地址：" << endl;
    cin >> p.address;

    cout << "修改成功" << endl;
    pauseAndCls();
}

// 清空联系人：逻辑清空 —— 只把 size 置 0，不逐个擦除数据
// 【性能 trick】O(1) 完成"清空"：数组的物理数据全部留在内存里，
//   但遍历只认 [0, size)，size=0 后所有旧数据立即"不可达"；
//   下次添加从下标 0 开始自然覆盖旧值。若存的是密码等敏感数据，
//   则必须物理擦除（逐槽置空或 std::fill）——本项目非该场景。
void cleanPerson(AddressBooks* books) {
    books->size = 0;   // 逻辑清空：只把人数置 0，无需逐个擦除
    cout << "通讯录已清空" << endl;
    pauseAndCls();
}

// 按姓名查找联系人，返回下标；找不到返回 -1
// 【算法】线性查找 O(n)：数据无序也不建索引，逐个比对姓名即可（百级数据无压力）。
// 【契约】返回值 ≥0 表示下标；-1 是"不存在"的哨兵 —— 与 string::find 返回 npos 同一思路。
// 【const 正确性】两个 const 各司其职：
//   const AddressBooks* —— 承诺不改通讯录（调用方放心把只读场景的指针交进来）；
//   const string&       —— 引用传参避免拷贝 string，const 承诺不改。
int isExist(const AddressBooks* books, const string& name) {
    for (int i = 0; i < books->size; i++) {      // 只在有效区 [0, size) 内找
        if (books->personArray[i].name == name) {   // string 的 == 逐字符比较内容（C 风格 strcmp 的正确性替代）
            return i;           // 命中即返回下标（最早匹配者优先——重名时删的是先添加的那个）
        }
    }
    return -1;      // 扫完未命中：返回哨兵值 -1（合法下标 ≥0，二者不冲突）
}
