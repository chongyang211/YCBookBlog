# cmake/linux-x64-gnu.toolchain.cmake
# 在 ARM64 Linux 容器内交叉编译 x86_64 版 libmp_native.so
# 依赖：apt install g++-x86-64-linux-gnu（Ubuntu arm64 ports 有此交叉包）
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER   x86_64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER x86_64-linux-gnu-g++)

set(CMAKE_FIND_ROOT_PATH /usr/x86_64-linux-gnu)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# 仅构建共享库（无可执行文件），交叉编译无需完整 sysroot
