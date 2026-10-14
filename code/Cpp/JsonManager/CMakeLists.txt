# ============================================================================
# 【文件定位】CMakeLists.txt —— CMake 构建脚本（工程化构建方式）
# ----------------------------------------------------------------------------
# 【设计思路】声明式描述：最低版本 → 工程名 → 语言标准 → 目标与源文件。
# 【常用流程】
#   cmake -B build          # 配置（生成构建系统到 build/）
#   cmake --build build     # 增量编译
#   ./build/JsonManager     # 运行（数据文件默认 kv.json，可 ./build/JsonManager my.json）
# ============================================================================

# 最低 CMake 版本：低于 3.28 拒绝配置（显式失败优于隐式出错）
cmake_minimum_required(VERSION 3.28)

# 工程名（未指定 LANGUAGES 时默认启用 C/C++ 编译器探测）
project(JsonManager)

# C++17 是硬需求：variant/optional/string_view/结构化绑定 全是 C++17 特性
set(CMAKE_CXX_STANDARD 17)

# 【源文件清单】.h 列入仅为 IDE 工程树展示；实际编译单元是 5 个 .cpp：
#   JsonExceptions.cpp 当前为空实现，但保留它 —— 将来异常加逻辑不用改构建脚本
add_executable(JsonManager main.cpp
        JsonExceptions.cpp
        JsonExceptions.h
        JsonNode.cpp
        JsonNode.h
        JsonParser.cpp
        JsonParser.h
        JsonWriter.cpp
        JsonWriter.h
        KvDatabase.cpp
        KvDatabase.h)
