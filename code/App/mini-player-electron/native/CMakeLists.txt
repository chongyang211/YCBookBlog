# native/CMakeLists.txt
# ----------------------------------------------------------------------
# mp_native —— MiniPlayer Electron 篇配套 C++ 共享库
# 一份 CMake 工程产出 Linux .so / macOS .dylib / Windows .dll
# ----------------------------------------------------------------------
cmake_minimum_required(VERSION 3.26)
project(mp_native LANGUAGES CXX VERSION 0.1.0)

# ========== 全局编译选项 ==========
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 导出 compile_commands.json 供 clangd / cpptools 使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 关键：默认隐藏符号，只有标记了 MP_API 的函数才被导出
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# ========== 用户可选项 ==========
option(MP_WITH_TAGLIB "Enable TagLib for real metadata reading"           OFF)
option(MP_WITH_SYSMEDIA "Enable system media control (MPRIS/MPNowPlaying/SMTC)" OFF)

# ========== 平台源文件分流 ==========
set(SRC
  src/version.cpp
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
target_include_directories(mp_native PUBLIC include)

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
      "-framework Foundation"
      "-framework MediaPlayer"
      "-framework AppKit")
  elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(DBUS REQUIRED dbus-1)
    target_include_directories(mp_native PRIVATE ${DBUS_INCLUDE_DIRS})
    target_link_libraries(mp_native PRIVATE ${DBUS_LIBRARIES})
  elseif(WIN32)
    target_link_libraries(mp_native PRIVATE windowsapp)
  endif()
endif()

# ========== 产物命名与输出位置 ==========
# Linux:    libmp_native.so
# macOS:    libmp_native.dylib
# Windows:  mp_native.dll + mp_native.lib
set_target_properties(mp_native PROPERTIES
  OUTPUT_NAME mp_native
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../resources/native
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../resources/native
  PDB_OUTPUT_DIRECTORY     ${CMAKE_SOURCE_DIR}/../resources/native)

if(WIN32)
  # Windows 下还要确保 Release/Debug 子目录也能命中（MSBuild 默认行为）
  set_target_properties(mp_native PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/../resources/native
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/../resources/native)
endif()

# ========== 开发期便利 ==========
message(STATUS "========================================")
message(STATUS "  mp_native v${PROJECT_VERSION}")
message(STATUS "  Platform:    ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Compiler:    ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  TagLib:      ${MP_WITH_TAGLIB}")
message(STATUS "  SysMedia:    ${MP_WITH_SYSMEDIA}")
message(STATUS "  Output dir:  ${CMAKE_SOURCE_DIR}/../resources/native")
message(STATUS "========================================")
