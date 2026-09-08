cmake_minimum_required(VERSION 3.16)
project(SchoolManager C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(SchoolManager
    main.c
    SchoolManager.c
    SchoolManager.h
)

if(MSVC)
    target_compile_options(SchoolManager PRIVATE /W4)
else()
    target_compile_options(SchoolManager PRIVATE -Wall -Wextra)
endif()
