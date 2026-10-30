# =============================================================================
# MiniPlayer · Qt 桌面端 · 一份 CMakeLists.txt 产出 Linux/macOS/Windows 三端二进制
# 对应书中 10.Apps/07.综合音视频案例/05.Qt桌面端实现(C++三端共用).md
# =============================================================================
# 本工程 95% 代码三端共用，5% 平台特化放在 src/platform/<os>/。
# 关键取舍：
#   1. CMake 而非 qmake（Qt 6 官方已转向 CMake，三端 IDE 都原生支持）
#   2. CMAKE_AUTOMOC/AUTORCC 全开（信号槽+资源文件无需手动 moc）
#   3. 不用 if-else 包裹跨平台源码——全靠 find_package 自动桥接
# =============================================================================

cmake_minimum_required(VERSION 3.21)
project(MiniPlayer VERSION 0.1.0 LANGUAGES CXX)

# macOS 平台特化走 Objective-C++ (.mm)
if(APPLE)
    enable_language(OBJCXX)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# MSVC 中文源码必备：/utf-8（否则 C4819 警告 + 可能乱码）
if(MSVC)
    add_compile_options(/utf-8 /W3 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---------------- Qt 6 ----------------
# find_package 会同时在 Linux/macOS/Windows 下工作
# Linux: 需要 DBus（§09 MPRIS）
find_package(Qt6 6.4 REQUIRED COMPONENTS
    Core Gui Widgets Multimedia MultimediaWidgets Sql
)

if(LINUX)
    find_package(Qt6 QUIET COMPONENTS DBus)
endif()

qt_standard_project_setup()

# ---------------- 源码分组 ----------------
set(MINIPLAYER_COMMON_SOURCES
    src/main.cpp

    # types/
    src/types/MediaItem.h

    # data/
    src/data/DbManager.h
    src/data/DbManager.cpp
    src/data/MediaRepository.h
    src/data/MediaRepository.cpp
    src/data/MediaScanner.h
    src/data/MediaScanner.cpp

    # service/
    src/service/PlayerService.h
    src/service/PlayerService.cpp
    src/service/LyricsParser.h
    src/service/LyricsParser.cpp
    src/service/SleepTimer.h
    src/service/SleepTimer.cpp
    src/service/ThemeManager.h
    src/service/ThemeManager.cpp

    # ui/
    src/ui/MainWindow.h
    src/ui/MainWindow.cpp
    src/ui/LibraryPage.h
    src/ui/LibraryPage.cpp
    src/ui/PlayingPage.h
    src/ui/PlayingPage.cpp
    src/ui/SettingsPage.h
    src/ui/SettingsPage.cpp
    src/ui/PlayerBar.h
    src/ui/PlayerBar.cpp
    src/ui/LyricsWidget.h
    src/ui/LyricsWidget.cpp

    # ui/components/
    src/ui/components/ProgressSlider.h
    src/ui/components/ProgressSlider.cpp
    src/ui/components/CoverLabel.h
    src/ui/components/CoverLabel.cpp
    src/ui/components/MediaListModel.h
    src/ui/components/MediaListModel.cpp
    src/ui/components/MediaListDelegate.h
    src/ui/components/MediaListDelegate.cpp
    src/ui/components/ToastHost.h
    src/ui/components/ToastHost.cpp

    # platform/（抽象接口 + 工厂，三端共享）
    src/platform/SystemMediaAdapter.h
    src/platform/SystemMediaAdapter.cpp
    src/platform/stub/StubAdapter.h
    src/platform/stub/StubAdapter.cpp
)

# ---------------- 可执行目标 ----------------
qt_add_executable(MiniPlayer
    WIN32           # Windows 子系统（无控制台窗口）
    MACOSX_BUNDLE   # macOS 产出 .app
    ${MINIPLAYER_COMMON_SOURCES}
)

# 资源文件（QSS、图标……）
qt_add_resources(MiniPlayer "app_resources"
    PREFIX "/"
    FILES
        resources/style.qss
)

target_include_directories(MiniPlayer PRIVATE src)

target_link_libraries(MiniPlayer PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Sql
)

# ---------------- 平台特化（§09 预留；当前全部走 Stub） ----------------
if(LINUX)
    # TODO §09: target_sources(MiniPlayer PRIVATE src/platform/linux/MprisAdapter.cpp)
    # if(Qt6DBus_FOUND)
    #     target_link_libraries(MiniPlayer PRIVATE Qt6::DBus)
    # endif()
    message(STATUS "[MiniPlayer] Linux build (MPRIS not yet wired; using StubAdapter)")
elseif(APPLE)
    # TODO §09: target_sources(MiniPlayer PRIVATE src/platform/macos/MPNowPlayingAdapter.mm)
    # target_link_libraries(MiniPlayer PRIVATE "-framework MediaPlayer" "-framework AVFoundation")
    message(STATUS "[MiniPlayer] macOS build (MPNowPlayingInfoCenter not yet wired; using StubAdapter)")
elseif(WIN32)
    # TODO §09: target_sources(MiniPlayer PRIVATE src/platform/windows/SmtcAdapter.cpp)
    # target_link_libraries(MiniPlayer PRIVATE WindowsApp.lib)
    message(STATUS "[MiniPlayer] Windows build (SMTC not yet wired; using StubAdapter)")
endif()

# ---------------- macOS Bundle 元信息 ----------------
if(APPLE)
    set_target_properties(MiniPlayer PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "MiniPlayer"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.yccoding.miniplayer"
        MACOSX_BUNDLE_COPYRIGHT "© 2026 YCCoding"
    )
endif()

# ---------------- 安装（为 CPack / windeployqt / macdeployqt 铺路） ----------------
install(TARGETS MiniPlayer
    RUNTIME DESTINATION bin
    BUNDLE  DESTINATION .
)
