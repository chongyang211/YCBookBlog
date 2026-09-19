# Demo 工程：线程库（thread）+ 网络库（network）+ 调度器（scheduler）统一入口
#
#   cmake -B build -DCMAKE_BUILD_TYPE=Release
#   cmake --build build -j
#   ./build/demo              # 全部演示
#   ./build/demo thread       # 仅线程库
#   ./build/demo network      # 仅网络库（内置服务器，无需外网）
#   ./build/demo scheduler    # 仅周期任务调度器

cmake_minimum_required(VERSION 3.16)
project(FacilityDemo CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

# 优先使用系统 libcurl，避免 FetchContent 拉取并编译 curl
set(CPR_FORCE_SYSTEM_CURL ON CACHE BOOL "" FORCE)

include(FetchContent)

# cpr：libcurl 的现代 C++ 封装（网络库依赖）
FetchContent_Declare(
  cpr
  GIT_REPOSITORY https://github.com/libcpr/cpr.git
  GIT_TAG 1.10.5
)
FetchContent_MakeAvailable(cpr)

# nlohmann/json：单头 JSON 库（网络库依赖）
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# ====== 通用线程设施库 ======
# 命名线程（Thread/ThreadManager）+ 线程池 + 可扩展线程
# 用法见 main.cpp 上半部分：ThreadManager::IotThread()->Post(...)
add_library(threadfacility STATIC
    thread/thread.cpp
    thread/thread_pool.cpp
    thread/thread_manager.cpp
    thread/extensible_thread.cpp
)
target_include_directories(threadfacility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(threadfacility PUBLIC Threads::Threads)

# ====== 通用网络库 ======
# HttpClient（链式 + 拦截器链）+ ApiClient（Response<T>）+ 内置测试服务器
# 用法见 main.cpp 下半部分：client.Get("/x").Query(...).Send()
add_library(netclient STATIC
    network/http_client.cpp
    network/api_client.cpp
    network/interceptor.cpp
    network/mini_test_server.cpp
)
target_include_directories(netclient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(netclient PUBLIC cpr::cpr nlohmann_json::nlohmann_json Threads::Threads)

# ====== 周期任务调度器 ======
# 基准 tick + 倍频触发 + 错峰 + 优先级 + 同步/异步模式 + 统计
# 用法见 main.cpp 调度器部分：scheduler.RegisterTask("heartbeat", 1, ...)
add_library(taskscheduler STATIC
    scheduler/periodic_task_scheduler.cpp
)
target_include_directories(taskscheduler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(taskscheduler PUBLIC Threads::Threads)

# ====== 统一 Demo 入口 ======
add_executable(demo main.cpp)
target_link_libraries(demo PRIVATE threadfacility netclient taskscheduler)

if(MSVC)
  target_compile_options(threadfacility PRIVATE /W4)
  target_compile_options(netclient PRIVATE /W4)
  target_compile_options(demo PRIVATE /W4)
else()
  target_compile_options(threadfacility PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(netclient PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(demo PRIVATE -Wall -Wextra -Wpedantic)
endif()
