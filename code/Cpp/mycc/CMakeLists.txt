cmake_minimum_required(VERSION 3.16)
project(mycc CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(mycc
    src/main.cpp
    src/token.cpp
    src/lexer.cpp
    src/parser.cpp
    src/type_checker.cpp
    src/opcode.cpp
    src/chunk.cpp
    src/codegen.cpp
    src/vm.cpp
)
target_include_directories(mycc PRIVATE include)

if(MSVC)
    target_compile_options(mycc PRIVATE /W4)
else()
    target_compile_options(mycc PRIVATE -Wall -Wextra -Wpedantic)
endif()
