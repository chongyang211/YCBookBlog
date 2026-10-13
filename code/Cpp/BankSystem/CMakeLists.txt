# ============================================================================
# 【文件定位】CMakeLists.txt —— CMake 构建脚本（工程化构建方式）
# ----------------------------------------------------------------------------
# 【设计思路】声明式描述工程：最低版本 → 工程名 → 语言标准 → 目标与源文件。
#   与 build.sh 的 *.cpp 通配相比，CMake 显式列文件（可精确控制）、
#   支持增量编译与多平台工程生成。
# 【常用流程】
#   cmake -B build          # 配置（生成构建系统到 build/ 目录）
#   cmake --build build      # 增量编译
#   ./build/BankSystem       # 运行
# ============================================================================

# 声明最低 CMake 版本：低于 3.28 直接报错拒绝（显式失败优于隐式出错）
cmake_minimum_required(VERSION 3.28)

# 定义工程名（未指定 LANGUAGES 时默认启用 C/C++ 编译器探测）
project(BankSystem)

# 设置 C++ 标准为 17：本项目用到 static constexpr double 类内初始化
# （C++17 起隐式 inline，无需类外定义）等特性
set(CMAKE_CXX_STANDARD 17)

# 【修复说明】原文件把 main.cpp 列了两遍（CMake 会自动去重所以能编译，
#   但属笔误）—— 已删除重复项；.h 列出来是为让 IDE 把头文件归入工程树，
#   实际编译单元只有 7 个 .cpp
add_executable(BankSystem main.cpp
        Account.cpp
        Account.h
        NormalAccount.cpp
        NormalAccount.h
        VipAccount.cpp
        VipAccount.h
        SavingAccount.cpp
        SavingAccount.h
        Bank.cpp
        Bank.h
        FileManager.cpp
        FileManager.h)
