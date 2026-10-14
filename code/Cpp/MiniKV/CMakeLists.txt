# ============================================================================
# 【文件定位】CMakeLists.txt —— 构建脚本（库 + REPL + 5 个测试的三层结构）
# ----------------------------------------------------------------------------
# 【设计思想·工程化演进】对比前几个案例（单一 add_executable）：
#   本项目抽出了"静态库 mkv_core"—— main 与 5 个测试共用同一份实现，
#   源文件只编译一次（改 store.cpp 只重编库 + 重链接，不重编 6 份拷贝）。
# 【编译单元】value/entry/store/command/aof/server 六个 .cpp 进库；
#   main.cpp 与 tests/*.cpp 各自成可执行目标链接这个库。
# ============================================================================

# 最低 CMake 版本（filesystem/jthread 的配置要求）
cmake_minimum_required(VERSION 3.16)

project(MiniKV CXX)      # CXX：纯 C++ 工程（跳过 C 编译器探测）

# C++20 是硬需求：std::jthread / stop_token（server/stress 用到）
set(CMAKE_CXX_STANDARD 20)
# REQUIRED：不支持直接报错，而非静默降级
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 禁 GNU 扩展：严格标准保证三大编译器行为一致（可移植）
set(CMAKE_CXX_EXTENSIONS OFF)

# 线程库：Linux 上 jthread/thread 需要 -lpthread（CMake 跨平台处理）
find_package(Threads REQUIRED)

# ====== 核心库（除 main.cpp 外的所有源文件） ======
add_library(mkv_core STATIC
    src/value.cpp
    src/entry.cpp
    src/store.cpp
    src/command.cpp
    src/aof.cpp
    src/server.cpp
)
# STATIC：打包成 libmkv_core.a —— 链接进各可执行文件（无运行时依赖）
target_include_directories(mkv_core PUBLIC include)
# PUBLIC：这个包含路径"传染"给所有链接 mkv_core 的目标 ——
# 测试文件里 #include "mkv/store.hpp" 不用再各自配路径（接口的一部分）
target_link_libraries(mkv_core PUBLIC Threads::Threads)
# PUBLIC 同理：使用本库的代码可能用到线程设施（头文件里有 jthread）

# ====== 主程序：REPL ======
add_executable(MiniKV src/main.cpp)
target_link_libraries(MiniKV PRIVATE mkv_core)
# PRIVATE：链接关系不外泄（没人再链 MiniKV，PRIVATE 表达最小暴露）

# ====== 测试与压测（可选，BUILD_TESTING=ON 时编译） ======
option(BUILD_TESTING "Build tests and stress" ON)
# option：构建开关（cmake -DBUILD_TESTING=OFF 可关闭 —— 发布构建瘦身）
if(BUILD_TESTING)
    foreach(t test_value test_store test_aof test_race stress)
        # foreach 循环代替 5 段复制粘贴 —— 5 个测试结构完全一致：
        add_executable(${t} tests/${t}.cpp)
        target_link_libraries(${t} PRIVATE mkv_core)
    endforeach()
endif()

# 高等级警告（只给核心库开：测试代码追求简短，警告阈值放宽）
if(MSVC)
    target_compile_options(mkv_core PRIVATE /W4)
else()
    target_compile_options(mkv_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
