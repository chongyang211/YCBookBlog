# 通用网络库（http_client / api_client）构建：
#   cmake -S . -B build && cmake --build build -j && ./build/network_demo

cmake_minimum_required(VERSION 3.16)
project(NetClient CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

# 优先使用系统 libcurl，避免 FetchContent 拉取并编译 curl
set(CPR_FORCE_SYSTEM_CURL ON CACHE BOOL "" FORCE)

include(FetchContent)

# cpr：libcurl 的现代 C++ 封装（https://github.com/libcpr/cpr）
FetchContent_Declare(
  cpr
  GIT_REPOSITORY https://github.com/libcpr/cpr.git
  GIT_TAG 1.10.5
)
FetchContent_MakeAvailable(cpr)

# nlohmann/json：单头 JSON 库
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# ====== 通用网络库 ======
add_library(netclient STATIC
    http_client.cpp
    api_client.cpp
    mini_test_server.cpp
)
target_include_directories(netclient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(netclient PUBLIC cpr::cpr nlohmann_json::nlohmann_json Threads::Threads)

# ====== 用法示例（内置 mini 服务器，无需外网） ======
add_executable(network_demo network_demo.cpp)
target_link_libraries(network_demo PRIVATE netclient)

if(MSVC)
  target_compile_options(netclient PRIVATE /W4)
else()
  target_compile_options(netclient PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(network_demo PRIVATE -Wall -Wextra -Wpedantic)
endif()
