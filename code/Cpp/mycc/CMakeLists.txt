# ============================================================================
# 【文件定位】CMakeLists.txt —— 构建脚本（单可执行文件，9 个源文件）
# ----------------------------------------------------------------------------
# 【设计思路】教学项目选最简结构：全部源文件编进一个 Mycc 可执行文件
#   （对比 MiniKV 的"静态库+多测试"结构）—— 但文件仍按编译器阶段分模块
#   （token/lexer → parser/type_checker → opcode/chunk/codegen → vm），
#   目录结构即五段式管线的"地图"。
# 【常用流程】
#   cmake -B build -DCMAKE_BUILD_TYPE=Release
#   cmake --build build -j
#   ./build/Mycc examples/fib.mycc     # 文件模式
#   ./build/Mycc                       # REPL 模式
# ============================================================================

# 最低 CMake 版本（3.16 起支持现代 target 系列指令的稳定集）
cmake_minimum_required(VERSION 3.16)

project(Mycc CXX)     # CXX：纯 C++ 工程

# C++17 是硬需求：variant / structured bindings / if constexpr / CTAD 全用上了
set(CMAKE_CXX_STANDARD 17)
# REQUIRED：编译器不支持 17 直接报错（而非静默降级后报一堆难懂的错）
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 禁 GNU 扩展：严格标准（-std=c++17 而非 gnu++17）—— 三大编译器行为一致
set(CMAKE_CXX_EXTENSIONS OFF)

# 源文件按"五段式管线"顺序罗列 —— 阅读顺序即编译器数据流顺序：
#   main（装配）→ 前端（token/lexer/parser/type_checker）
#   → 后端（opcode/chunk/codegen）→ 执行（vm）
add_executable(Mycc
    src/main.cpp
    src/token.cpp
    src/lexer.cpp
    src/parser.cpp
    src/type_checker.cpp
    src/opcode.cpp
    src/chunk.cpp
    src/codegen.cpp
    src/vm.cpp
)
target_include_directories(Mycc PRIVATE include)
# 头文件根：源码里 #include "mycc/xxx.h" 的解析起点

# 高等级警告：MSVC /W4；GCC/Clang 三连 —— 逐行注释项目更要零警告基线
if(MSVC)
    target_compile_options(Mycc PRIVATE /W4)
else()
    target_compile_options(Mycc PRIVATE -Wall -Wextra -Wpedantic)
endif()
