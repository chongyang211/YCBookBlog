cmake_minimum_required(VERSION 3.16)
project(MiniKV CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
target_include_directories(mkv_core PUBLIC include)
target_link_libraries(mkv_core PUBLIC Threads::Threads)

# ====== 主程序：REPL ======
add_executable(MiniKV src/main.cpp)
target_link_libraries(MiniKV PRIVATE mkv_core)

# ====== 测试与压测（可选，BUILD_TESTING=ON 时编译） ======
option(BUILD_TESTING "Build tests and stress" ON)
if(BUILD_TESTING)
    foreach(t test_value test_store test_aof test_race stress)
        add_executable(${t} tests/${t}.cpp)
        target_link_libraries(${t} PRIVATE mkv_core)
    endforeach()
endif()

if(MSVC)
    target_compile_options(mkv_core PRIVATE /W4)
else()
    target_compile_options(mkv_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
