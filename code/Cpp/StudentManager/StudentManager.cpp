#include "StudentManager.h"
#include <iostream>
#include <limits>

using namespace std;

// ===== 辅助函数 =====

// 清屏：ANSI 转义序列（跨平台，不依赖 system("cls")）
void clearScreen() {
    cout << "\033[2J\033[H" << flush;
}

// 暂停 + 清屏：替代 Windows 的 system("pause") + system("cls")
void pauseAndCls() {
    cout << "按回车键继续..." << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');   // 清掉缓冲区残留的换行
    cin.get();                                             // 等待用户回车
    clearScreen();
}

// ===== 菜单 =====

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

void showMainSelect(AddressBooks* abs) {
    int select = 0;
    while (true) {
        showMenu();
        cout << "请选择功能：" << endl;
        cin >> select;

        switch (select) {
            case 1:  // 添加联系人
                addPerson(abs);
                break;
            case 2:  // 显示联系人
                showPerson(abs);
                break;
            case 3:  // 删除联系人
                deletePerson(abs);
                break;
            case 4:  // 查找联系人
                findPerson(abs);
                break;
            case 5:  // 修改联系人
                modifyPerson(abs);
                break;
            case 6:  // 清空联系人
                cleanPerson(abs);
                break;
            case 0:  // 退出通讯录
                cout << "欢迎下次使用" << endl;
                std::cout << "Press Enter to continue..."  << endl;
                std::cin.get(); // 等待用户输入字符
                return;
            default:
                cout << "输入有误，请重新输入" << endl;
                break;
        }
    }
}

// ===== 业务函数 =====
void addPerson1(AddressBooks* books) {
    if (books->size >= AddressBooks::MAX) {
        cout << "通讯录已经满了，无法添加" << endl;
        return;
    }
    Person& p = books->personArray[books->size];
}

void addPerson2(AddressBooks books) {
    if (books.size >= AddressBooks::MAX) {
        cout << "通讯录已经满了，无法添加" << endl;
        return;
    }
    Person& p = books.personArray[books.size];
    // 因为函数要改到调用方那份原始数据，而值传递只能改到副本。
}


void addPerson(AddressBooks* abs) {
    if (abs->size >= AddressBooks::MAX) {
        cout << "通讯录已经满了，无法添加" << endl;
        return;
    }

    Person& p = abs->personArray[abs->size];   // 引用当前空槽位，后续直接赋值

    cout << "请输入姓名：" << endl;
    cin >> p.name;

    cout << "请输入性别：" << endl;
    cout << "1 -- 男" << endl;
    cout << "2 -- 女" << endl;
    int sex = 0;
    while (true) {
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

    abs->size++;
    cout << "添加用户：" << p.name << " 成功。当前人数：" << abs->size << endl;
    pauseAndCls();
}

void showPerson(AddressBooks* abs) {
    if (abs->size == 0) {
        cout << "通讯录为空，没有联系人" << endl;
        pauseAndCls();
        return;
    }
    for (int i = 0; i < abs->size; i++) {
        const Person& p = abs->personArray[i];
        cout << "姓名：" << p.name << "\t";
        cout << "性别：" << (p.sex == 1 ? "男" : "女") << "\t";
        cout << "年龄：" << p.age << "\t";
        cout << "电话：" << p.phone << "\t";
        cout << "住址：" << p.address << endl;
    }
    pauseAndCls();
}

void deletePerson(AddressBooks* abs) {
    cout << "请输入您要删除的联系人姓名：" << endl;
    string name;
    cin >> name;

    int result = isExist(abs, name);
    if (result == -1) {
        cout << "查无此人" << endl;
        pauseAndCls();
        return;
    }

    // 从 result 开始，后面的元素整体前移一位
    // 注意上界是 size-1：避免访问 personArray[size]（未写入的位置）
    for (int i = result; i < abs->size - 1; i++) {
        abs->personArray[i] = abs->personArray[i + 1];
    }
    abs->size--;
    cout << "删除成功" << endl;
    pauseAndCls();
}

void findPerson(AddressBooks* abs) {
    cout << "请输入您要查找的联系人姓名：" << endl;
    string name;
    cin >> name;

    int result = isExist(abs, name);
    if (result == -1) {
        cout << "查无此人" << endl;
        pauseAndCls();
        return;
    }

    const Person& p = abs->personArray[result];
    cout << "姓名：" << p.name << "\t";
    cout << "性别：" << (p.sex == 1 ? "男" : "女") << "\t";
    cout << "年龄：" << p.age << "\t";
    cout << "电话：" << p.phone << "\t";
    cout << "住址：" << p.address << endl;
    pauseAndCls();
}

void modifyPerson(AddressBooks* abs) {
    cout << "请输入您要修改的联系人姓名：" << endl;
    string name;
    cin >> name;

    int result = isExist(abs, name);
    if (result == -1) {
        cout << "查无此人" << endl;
        pauseAndCls();
        return;
    }

    Person& p = abs->personArray[result];   // ⭐ 引用旧位置，覆盖原值而非追加

    cout << "请输入姓名：" << endl;
    cin >> p.name;

    cout << "请输入性别：" << endl;
    cout << "1 -- 男" << endl;
    cout << "2 -- 女" << endl;
    int sex = 0;
    while (true) {
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

void cleanPerson(AddressBooks* abs) {
    abs->size = 0;   // 逻辑清空：只把人数置 0，无需逐个擦除
    cout << "通讯录已清空" << endl;
    pauseAndCls();
}

// 按姓名查找联系人，返回下标；找不到返回 -1
int isExist(AddressBooks* abs, const string& name) {
    for (int i = 0; i < abs->size; i++) {
        if (abs->personArray[i].name == name) {
            return i;
        }
    }
    return -1;
}