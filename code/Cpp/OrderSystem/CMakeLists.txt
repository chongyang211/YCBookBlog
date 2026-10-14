# ============================================================================
# 【文件定位】CMakeLists.txt —— CMake 构建脚本（双目标：CLI + 压测）
# ----------------------------------------------------------------------------
# 【设计思路】交互程序与压测程序是两个独立可执行目标（不同 main 入口），
#   共享 include/ 下的 header-only 组件 —— 每个目标独立声明
#   include 路径与链接库（CMake 的 target 级配置，互不传染）。
# 【常用流程】
#   cmake -B build -DCMAKE_BUILD_TYPE=Release    # 配置（Release 才有真实性能）
#   cmake --build build -j                       # 并行编译
#   ./build/OrderSystem    /    ./build/bench
# ============================================================================

# 最低 CMake 版本（低于 3.16 拒绝配置 —— 显式失败优于隐式出错）
cmake_minimum_required(VERSION 3.16)

project(OrderSystem CXX)     # CXX 显式声明 C++ 工程（跳过 C 编译器探测）

# C++17 是硬需求：shared_mutex / invoke_result_t / 折叠表达式 / CTAD
set(CMAKE_CXX_STANDARD 17)
# REQUIRED：编译器不支持 C++17 直接报错（而非静默降级后爆难懂的错）
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# EXTENSIONS OFF：禁用 GNU 扩展（-std=c++17 而非 gnu++17）—— 严格标准，
# 保证代码在 MSVC/Clang/GCC 下行为一致（可移植性）
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找线程库：Linux 上 std::thread 需要 -lpthread，CMake 的 Threads 模块
# 统一处理各平台差异（macOS 系统自带无需链接，Linux 必须链）
find_package(Threads REQUIRED)

# ====== 主程序：交互式 CLI ======
add_executable(OrderSystem main.cpp)
# 头文件搜索路径：源码里写 #include "include/ThreadPool.h" 的根在项目目录
target_include_directories(OrderSystem PRIVATE include)
# 链接线程库（PRIVATE：仅本目标的实现需要，不传染给链接它的目标）
target_link_libraries(OrderSystem PRIVATE Threads::Threads)

# ====== 压测程序 ======
add_executable(bench benchmark.cpp)
target_include_directories(bench PRIVATE include)
target_link_libraries(bench PRIVATE Threads::Threads)

# 高等级警告：MSVC /W4；GCC/Clang -Wall -Wextra -Wpedantic ——
# 并发代码的很多隐患（未初始化/隐式转换）警告期暴露远好于运行期玄学
if(MSVC)
    target_compile_options(OrderSystem PRIVATE /W4)
    target_compile_options(bench PRIVATE /W4)
else()
    target_compile_options(OrderSystem PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(bench PRIVATE -Wall -Wextra -Wpedantic)
endif()
