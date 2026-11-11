# cmake/windows-x64-mingw.toolchain.cmake
# 在 Linux 容器内用 MinGW-w64 交叉编译 Windows x64 版 mp_native.dll
# 依赖：apt install g++-mingw-w64-x86-64
#
# 核心：CMAKE_SYSTEM_NAME=Windows 触发 CMake 交叉模式，
#       FIND_ROOT_PATH 让 find_* 只搜 MinGW 自带头文件/库（不混入 Linux 系统）。
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER  x86_64-w64-mingw32-windres)

set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
