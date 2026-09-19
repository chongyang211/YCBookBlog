cmake_minimum_required(VERSION 3.16)
project(ThreadFacility CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

# ====== 通用线程设施库 ======
# 命名线程（Thread/Threads）+ 线程池（ThreadPool::submit → future）+ 可扩展线程
# 引用方式： #include "thread/thread_manager.h"   →   facility::ThreadManager::IotThread()
add_library(threadfacility STATIC
    thread/thread.cpp
    thread/extensible_thread.cpp
    thread/thread_pool.cpp
    thread/thread_manager.cpp
)
target_include_directories(threadfacility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(threadfacility PUBLIC Threads::Threads)

# ====== 用法示例 ======
add_executable(thread_demo main.cpp)
target_link_libraries(thread_demo PRIVATE threadfacility)

if(MSVC)
    target_compile_options(threadfacility PRIVATE /W4)
    target_compile_options(thread_demo PRIVATE /W4)
else()
    target_compile_options(threadfacility PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(thread_demo PRIVATE -Wall -Wextra -Wpedantic)
endif()
