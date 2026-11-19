//
// Created by 杨充 on 2026/11/2.
//

#include <iostream>
using namespace std;

int main() {
    int a = 10;              // 定义整型变量a

    // 1、指针的定义与初始化（推荐：定义的同时就初始化）
    int* p = &a;             // 指针p指向变量a的地址

    // 2、打印地址
    std::cout << "a的地址: " << &a << endl;    // 0x7ffd4c8e9a1c（每次运行可能不同）
    cout << "p保存的地址: " << p << endl; // 与上一行完全相同

    // 3、指针的使用：通过 * 操作指针指向的内存（解引用）
    cout << "*p = " << *p << endl;        // 10

    // 4、通过指针修改它所指向的内存
    *p = 20;
    cout << "修改后 a = " << a << endl;   // a 被改为 20

    return 0;
}
