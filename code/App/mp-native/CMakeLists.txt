# native/CMakeLists.txt
# ======================================================================
# mp_native —— 独立跨平台原生库工程
#
# 定位：不依赖 Electron 也可独立构建/安装/分发的 C++ 共享库。
#   1. 嵌入模式（默认）  ：产物直出 ../resources/native，供本仓 Electron 工程加载
#   2. 独立模式          ：MP_EMBED_OUTPUT=OFF，标准 build/ + cmake --install
#                          产出可 zip 分发的 include/ + lib/，任何 C ABI 消费方可用
#
# 快速上手：
#   cmake --preset release            # 见 CMakePresets.json
#   cmake --build build -j
#   cmake --install build --prefix dist
#
# 对应文档：packages/website/10.Apps/05.综合音视频案例/06.Electron桌面篇.md §04
# ======================================================================
cmake_minimum_required(VERSION 3.26)
project(mp_native VERSION 1.0.0 LANGUAGES C CXX)

# ========== 全局编译选项 ==========
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 关键：默认隐藏符号，只有标记了 MP_API 的函数才被导出
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)        # 让 clangd / cpptools 跳转工作

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# ========== 用户可选项 ==========
option(MP_WITH_TAGLIB       "Link TagLib for real metadata reading"            OFF)
option(MP_WITH_SYSMEDIA     "System media control (MPRIS/MPNowPlaying/SMTC)"   OFF)
option(MP_BUILD_EXAMPLES    "Build the pure-C smoke example"                   OFF)
option(MP_UNIVERSAL_MACOS   "macOS universal binary (arm64 + x86_64)"          OFF)
option(MP_EMBED_OUTPUT      "Write artifacts into ../resources/native (Electron dev)" ON)

# macOS universal（分发模式常用）
if(MP_UNIVERSAL_MACOS AND APPLE)
  set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS architectures" FORCE)
endif()

# ========== 平台源文件分流 ==========
set(SRC
  src/version.cpp
  # core 模块：零依赖基础能力（全平台，始终编译）
  src/core/kv.cpp
  src/core/log.cpp
  # media 模块：MiniPlayer 业务能力
  src/probe.cpp
  src/metadata.cpp
  src/lrc.cpp
)

if(MP_WITH_SYSMEDIA)
  if(WIN32)
    list(APPEND SRC src/sysmedia/smtc_win.cpp)
  elseif(APPLE)
    enable_language(OBJCXX)
    list(APPEND SRC src/sysmedia/nowplaying_mac.mm)
  elseif(UNIX)
    list(APPEND SRC src/sysmedia/mpris_linux.cpp)
  endif()
else()
  list(APPEND SRC src/sysmedia/stub.cpp)
endif()

# ========== 构建共享库 ==========
add_library(mp_native SHARED ${SRC})
target_include_directories(mp_native
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_compile_definitions(mp_native PRIVATE MP_NATIVE_EXPORTS)
# 版本号单一来源：project(VERSION) → 编译期注入 → mp_version() 返回值
target_compile_definitions(mp_native PRIVATE MP_NATIVE_VERSION="${PROJECT_VERSION}")

# ========== 依赖链接 ==========
if(MP_WITH_TAGLIB)
  find_package(TagLib REQUIRED)
  target_link_libraries(mp_native PRIVATE TagLib::tag)
  target_compile_definitions(mp_native PRIVATE MP_WITH_TAGLIB=1)
endif()

if(MP_WITH_SYSMEDIA)
  target_compile_definitions(mp_native PRIVATE MP_WITH_SYSMEDIA=1)
  if(APPLE)
    target_link_libraries(mp_native PRIVATE
      "-framework Foundation" "-framework MediaPlayer" "-framework AppKit")
  elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(DBUS REQUIRED dbus-1)
    target_include_directories(mp_native PRIVATE ${DBUS_INCLUDE_DIRS})
    target_link_libraries(mp_native PRIVATE ${DBUS_LIBRARIES})
  elseif(WIN32)
    target_link_libraries(mp_native PRIVATE windowsapp)   # WinRT 运行时
  endif()
endif()

# ========== MinGW（容器交叉编译 Windows）适配 ==========
if(MINGW)
  # 产物名与 MSVC 一致：mp_native.dll（无 lib 前缀）
  set_target_properties(mp_native PROPERTIES PREFIX "")
  # 静态链接 C++ 运行时 → dll 自包含，消费方机器无需 libstdc++-6.dll 等
  target_link_options(mp_native PRIVATE -static-libgcc -static-libstdc++)
  target_link_libraries(mp_native PRIVATE -Wl,-Bstatic,-lwinpthread,-Bdynamic)
endif()

# ========== 产物输出 ==========
if(MP_EMBED_OUTPUT)
  # 嵌入输出目录：由消费方工程显式传入（如 Electron 的 build-native.cjs）；
  # 缺省回退旧约定（库工程与消费工程同级时 ../resources/native 仍可用）
  if(NOT MP_EMBED_OUTPUT_DIR)
    set(MP_EMBED_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../resources/native)
  endif()
  set(MP_MODE_LABEL "embedded (output → ${MP_EMBED_OUTPUT_DIR})")
  set(MP_OUT_DIR ${MP_EMBED_OUTPUT_DIR})
  set_target_properties(mp_native PROPERTIES
    OUTPUT_NAME mp_native
    LIBRARY_OUTPUT_DIRECTORY ${MP_OUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${MP_OUT_DIR}
    PDB_OUTPUT_DIRECTORY     ${MP_OUT_DIR})
  if(WIN32)
    set_target_properties(mp_native PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${MP_OUT_DIR}
      RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${MP_OUT_DIR})
  endif()
else()
  set(MP_MODE_LABEL "standalone (standard build/ + install)")
endif()

# ========== C 消费示例（证明 C ABI 与语言无关）==========
if(MP_BUILD_EXAMPLES)
  add_executable(mp_smoke examples/smoke.c)
  target_link_libraries(mp_smoke PRIVATE mp_native)
endif()

# ========== 核心模块测试（kv / log）==========
option(MP_BUILD_TESTS "Build core module tests" OFF)
if(MP_BUILD_TESTS)
  enable_testing()
  add_executable(mp_core_test tests/core_test.c)
  target_link_libraries(mp_core_test PRIVATE mp_native)
  add_test(NAME core COMMAND mp_core_test)
  set_tests_properties(core PROPERTIES
    ENVIRONMENT "TMPDIR=$ENV{TMPDIR}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

# ========== 安装规则（独立分发的核心）==========
# 用法：cmake --install build --prefix dist
# 产物布局：
#   dist/include/mp_native.h
#   dist/lib/(libmp_native.so | libmp_native.dylib)   [Unix]
#   dist/bin/mp_native.dll + dist/lib/mp_native.lib   [Windows]
#   dist/lib/cmake/mp-native/*                        [CMake 包]
include(GNUInstallDirs)
install(TARGETS mp_native EXPORT mp-nativeTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}    # Windows .dll
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}    # Linux .so / macOS .dylib
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})   # Windows .lib / 符号链接
install(FILES
  include/mp_native.h
  include/mp_def.h
  include/mp_kv.h
  include/mp_log.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT mp-nativeTargets
  NAMESPACE mp::
  FILE mp-nativeTargets.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mp-native)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/mp-nativeConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/mp-nativeConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/mp-nativeConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mp-native)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/mp-nativeConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/mp-nativeConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mp-native)

# ========== 开发期信息 ==========
message(STATUS "========================================")
message(STATUS "  mp_native v${PROJECT_VERSION}")
message(STATUS "  Platform:      ${CMAKE_SYSTEM_NAME} (${CMAKE_SYSTEM_PROCESSOR})")
message(STATUS "  Compiler:      ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Mode:          ${CMAKE_BUILD_TYPE} / ${MP_MODE_LABEL}")
message(STATUS "  TagLib:        ${MP_WITH_TAGLIB}")
message(STATUS "  SysMedia:      ${MP_WITH_SYSMEDIA}")
message(STATUS "  Examples:      ${MP_BUILD_EXAMPLES}")
message(STATUS "  Universal macOS: ${MP_UNIVERSAL_MACOS}")
message(STATUS "========================================")
