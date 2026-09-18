cmake_minimum_required(VERSION 3.16)
project(OrderSystem CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

# ====== 主程序：交互式 CLI ======
add_executable(OrderSystem main.cpp)
target_include_directories(OrderSystem PRIVATE include)
target_link_libraries(OrderSystem PRIVATE Threads::Threads)

# ====== 压测程序 ======
add_executable(bench benchmark.cpp)
target_include_directories(bench PRIVATE include)
target_link_libraries(bench PRIVATE Threads::Threads)

if(MSVC)
    target_compile_options(OrderSystem PRIVATE /W4)
    target_compile_options(bench PRIVATE /W4)
else()
    target_compile_options(OrderSystem PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(bench PRIVATE -Wall -Wextra -Wpedantic)
endif()
