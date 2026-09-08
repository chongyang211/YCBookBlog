cmake_minimum_required(VERSION 3.16)
project(BankSystem C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(BankSystem
    main.c
    BankSystem.c
    BankSystem.h
)

if(MSVC)
    target_compile_options(BankSystem PRIVATE /W4)
else()
    target_compile_options(BankSystem PRIVATE -Wall -Wextra)
endif()
