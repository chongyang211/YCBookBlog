cmake_minimum_required(VERSION 3.16)
project(MiniLab CXX)

# =========================================================
# MiniLab — 12 OS 实验盒
# 参见 packages/website/04.计算机/03.操作系统原理/12.MiniLab操作系统实验盒.md
# =========================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra -Wpedantic -g)

# macOS 的 ucontext 需要 _XOPEN_SOURCE，但会屏蔽 MAP_ANON；只在盒 ② 里加

# -------- 依赖：无外部依赖 --------
# 本仓库零依赖：日志/格式化由 common/fmt_shim.hpp 实现（API 兼容 fmt 子集）
# 想换真·fmt 库请参见 README 的"升级依赖"章节

# -------- 公共头 --------
include_directories(${CMAKE_SOURCE_DIR}/common)

# -------- 平台检测 --------
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(MLAB_IS_LINUX ON)
    add_compile_definitions(MLAB_LINUX=1)
else()
    set(MLAB_IS_LINUX OFF)
    message(STATUS "Non-Linux host; boxes 10/11/12 will be skipped.")
endif()

# -------- 12 盒子 --------
set(ALL_BOXES
    01_proc 02_coro 03_sched 04_ipc 05_lock
    06_malloc 07_vm 08_fs 09_io
)
if(MLAB_IS_LINUX)
    list(APPEND ALL_BOXES 11_ns 12_cgrp)
    # 10_drv 是独立内核模块，不走 CMake（见 boxes/10_drv/Makefile）
endif()

foreach(box IN LISTS ALL_BOXES)
    if(EXISTS ${CMAKE_SOURCE_DIR}/boxes/${box}/CMakeLists.txt)
        message(STATUS "Enabling box: ${box}")
        add_subdirectory(boxes/${box})
    endif()
endforeach()

# -------- 一键 demo-all 自定义目标 --------
add_custom_target(demo-all
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/out/csv
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/out/plots
    COMMAND bash ${CMAKE_SOURCE_DIR}/scripts/demo-all.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Run all 12 boxes (约 90 秒)"
)
