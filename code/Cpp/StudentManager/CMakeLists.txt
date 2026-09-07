cmake_minimum_required(VERSION 3.16)
project(StudentManager CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(StudentManager
    main.cpp
    StudentManager.cpp
    StudentManager.h
)

if(MSVC)
    target_compile_options(StudentManager PRIVATE /W4)
else()
    target_compile_options(StudentManager PRIVATE -Wall -Wextra -Wpedantic)
endif()
