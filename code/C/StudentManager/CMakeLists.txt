cmake_minimum_required(VERSION 3.16)
project(StudentManager C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(StudentManager
    main.c
    StudentManager.c
    StudentManager.h
)

if(MSVC)
    target_compile_options(StudentManager PRIVATE /W4)
else()
    target_compile_options(StudentManager PRIVATE -Wall -Wextra)
endif()
