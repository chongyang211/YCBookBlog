# ====================================================================
# MiniKVStore —— 多文件版（与 12.迷你KV存储引擎.md 配套）
# ====================================================================
cmake_minimum_required(VERSION 3.16)
project(mini_kv CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(Threads REQUIRED)

# ============== 核心静态库：把所有 mkv:: 的实现编一份 ==============
add_library(mkv_core STATIC
    src/value.cpp
    src/log.cpp
    src/store.cpp
    src/command.cpp
    src/aof.cpp
    src/server.cpp
)

target_include_directories(mkv_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mkv_core PUBLIC Threads::Threads)

if(MSVC)
    target_compile_options(mkv_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(mkv_core PRIVATE
        -Wall -Wextra -Wpedantic
        -Wshadow -Wnon-virtual-dtor
    )
endif()

# 允许在命令行上调日志级别：cmake -DKV_LOG_LEVEL=0 -B build
if(DEFINED KV_LOG_LEVEL)
    target_compile_definitions(mkv_core PUBLIC KV_LOG_LEVEL=${KV_LOG_LEVEL})
endif()

# ============== 主可执行文件 ==============
add_executable(mini_kv src/main.cpp)
target_link_libraries(mini_kv PRIVATE mkv_core)

# ============== Stress 压力测试 ==============
add_executable(mini_kv_stress tests/stress.cpp)
target_link_libraries(mini_kv_stress PRIVATE mkv_core)

# ============== 安装规则（可选）==============
install(TARGETS mini_kv RUNTIME DESTINATION bin)
