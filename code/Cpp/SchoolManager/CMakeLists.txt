# ============================================================================
# 【文件定位】CMakeLists.txt —— CMake 构建脚本（build.sh 的工程化替代方案）
# ----------------------------------------------------------------------------
# 【设计思路】CMake 通过"声明式描述"生成对应平台的构建系统
#   （Unix → Makefile，配合 IDE 可生成 Xcode/VS 工程），
#   自动处理：增量编译（只重编改动文件）、依赖跟踪、多平台适配。
# 【常用流程】
#   cmake -B build      # 在 build/ 目录生成构建系统（只需首次或改本文件后执行）
#   cmake --build build # 增量编译
#   ./build/SchoolManager   # 运行产物
# ============================================================================

# 声明本工程要求的最低 CMake 版本：低于 3.28 的 cmake 会直接报错拒绝配置
# （防止旧版 CMake 不认识新语法导致诡异行为——"显式失败"优于"隐式出错"）
cmake_minimum_required(VERSION 3.28)

# 定义工程名；可选参数 LANGUAGES 指定语言（缺省时 C/C++ 均启用）
project(SchoolManager)

# 设置 C++ 标准为 C++17：对应 g++ 的 -std=c++17 编译选项
# 【技术原理】本项目依赖 C++17 特性：结构化绑定 auto& [id, room]、
#             auto 形参的泛型 lambda（accumulate 中的 const auto& kv）等。
#             默认只作用于本目录及子目录（CMAKE_CXX_STANDARD_REQUIRED ON 可强制）
set(CMAKE_CXX_STANDARD 17)

# 定义可执行目标 SchoolManager，并列出参与编译的全部源文件
# 【说明】.h 也列出来是为了让 IDE（CLion/VS）把它们归入项目树展示；
#         实际编译只使用 .cpp，头文件通过 #include 自动进入各翻译单元。
# 【注意】原文件中列出了 Test.cpp，但该文件当前不存在——
#         CMake 配置阶段会报"Cannot find source file: Test.cpp"而中断，
#         故暂时注释掉；将来补充 Test.cpp 后取消注释即可。
add_executable(SchoolManager main.cpp
        User.cpp
        User.h
        Student.cpp
        Student.h
        Teacher.cpp
        Teacher.h
        Admin.cpp
        Admin.h
        Computer.cpp
        Computer.h
        Speech.cpp
        Speech.h
        Reservation.cpp
        Reservation.h
        CampusSystem.cpp
        CampusSystem.h
        # Test.cpp    # ← 不存在，暂注释（取消注释前请先创建该文件）
)
