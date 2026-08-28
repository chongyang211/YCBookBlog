cmake_minimum_required(VERSION 3.28)

project(MiniKV)

set(CMAKE_CXX_STANDARD 17)

add_executable(MiniKV src/main.cpp
        src/command.cpp
        include/mkv/command.hpp
        src/entry.cpp
        include/mkv/aof.hpp
        include/mkv/entry.hpp
        include/mkv/errors.hpp
        include/mkv/log.hpp
        include/mkv/server.hpp
        include/mkv/store.hpp
        include/mkv/value.hpp
        src/value.cpp
        tests/test_value.cpp)

target_include_directories(MiniKV PRIVATE ${CMAKE_SOURCE_DIR}/include)
