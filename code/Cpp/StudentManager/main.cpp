#include "StudentManager.h"

int main() {
    AddressBooks abs;          // 创建通讯录（size 已在结构体内初始化为 0）
    showMainSelect(&abs);      // 进入菜单循环
    return 0;
}
