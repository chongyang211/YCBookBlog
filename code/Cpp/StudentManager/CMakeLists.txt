# ============================================================================
# 【文件定位】CMakeLists.txt —— CMake 构建脚本（比手敲 g++ 更工程化的构建方式）
# ----------------------------------------------------------------------------
# 【设计思路】声明式描述"要构建什么、用什么标准、开哪些警告"，
#   由 CMake 生成各平台构建系统（Makefile/Ninja/Xcode/VS 工程皆可），
#   自动处理增量编译与依赖跟踪。
# 【常用命令】
#   cmake -B build -DCMAKE_BUILD_TYPE=Release   # 首次（或改本文件后）配置
#   cmake --build build -j                      # 并行增量编译
#   ./build/StudentManager                       # 运行
# ============================================================================

# 声明最低 CMake 版本：低于 3.16 直接报错拒绝配置
# （显式失败优于"旧版本遇到不认识的语法产生诡异行为"）
cmake_minimum_required(VERSION 3.16)

# 定义工程名；CXX 显式声明语言为 C++（避免误启用 C 编译器探测）
project(StudentManager CXX)

# 设置 C++ 标准为 17（本项目用到类内默认成员初始化 NSDMI 等 C++11+ 特性；
# 结构体的几个成员默认值其实 C++11 就够，选 17 是为后续案例平滑升级）
set(CMAKE_CXX_STANDARD 17)

# 标准从"建议"升级为"强制"：若编译器不支持 C++17 直接报错，
# 而不是静默降级到更低标准后爆出一堆难懂的错误
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 定义可执行目标并列出源文件
# 【说明】.h 不参与编译（头文件由 #include 展开进各 .cpp），列出只是
#   为了让 IDE 把它们归入工程树；真正的编译单元是 main.cpp + StudentManager.cpp
add_executable(StudentManager
    main.cpp
    StudentManager.cpp
    StudentManager.h
)

# 编译警告：按编译器分别开启高等级告警，让潜在问题在编译期就暴露
#   MSVC：/W4 —— Visual C++ 的第 4 级警告（含未使用参数、隐式转换等）
#   GCC/Clang：-Wall -Wextra -Wpedantic —— 常规警告 + 扩展警告 + 严格标准检查
if(MSVC)
    target_compile_options(StudentManager PRIVATE /W4)
else()
    target_compile_options(StudentManager PRIVATE -Wall -Wextra -Wpedantic)
endif()
