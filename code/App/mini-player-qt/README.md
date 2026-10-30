# MiniPlayer · Qt 桌面端（C++ 三端共用）

> 对应书中 `10.Apps/07.综合音视频案例/05.Qt桌面端实现(C++三端共用).md` 的工程落地。
>
> **95% 代码三端共用（Linux / macOS / Windows），5% 平台特化放在 `src/platform/<os>/`。**
> 当前版本：**v0.1.0 (2026-10-30)** · MVP → 核心播放 → SQLite 持久化 → LRC 歌词 → 主题 → 睡眠定时。
> 平台特化（§09 MPRIS / SMTC / MPNowPlaying）保留抽象接口 + Stub 占位，接入见文档路线图。

## 快速上手

### 1. 环境需求

- Qt **6.4+**（推荐 LTS 6.5 或 6.6），通过 Qt 官方 Online Installer 或系统包管理器安装
- 支持 C++17 的编译器：GCC 10+ / Clang 13+ / MSVC 2019+
- CMake **3.21+**
- SQLite 由 Qt 自带的 QSQLITE 驱动承载，无需额外安装

### 2. 构建命令

```bash
cd code/App/mini-player-qt
cmake -S . -B build -DCMAKE_PREFIX_PATH=<你的 Qt 安装路径>  # 例如 ~/Qt/6.6.3/gcc_64
cmake --build build --config Release -j
```

**Qt 路径示例**：

| 平台 | 典型 CMAKE_PREFIX_PATH |
|------|------------------------|
| macOS | `/Users/<me>/Qt/6.6.3/macos` 或 `/opt/homebrew/opt/qt@6` |
| Linux | `/opt/Qt/6.6.3/gcc_64`，或 `apt install qt6-base-dev qt6-multimedia-dev` 后无需传路径 |
| Windows | `C:/Qt/6.6.3/msvc2019_64` |

### 3. 运行

```bash
# macOS
open build/MiniPlayer.app
# Linux
./build/MiniPlayer
# Windows (开发时，部署 → 用 windeployqt)
build\MiniPlayer.exe
```

首次启动会在平台标准数据目录新建 SQLite：

- Linux: `~/.local/share/MiniPlayer/library.db`
- macOS: `~/Library/Application Support/MiniPlayer/library.db`
- Windows: `%APPDATA%\MiniPlayer\library.db`

### 4. 自备测试素材

点右上角「📁 导入目录」选任意本机音乐文件夹，Scanner 会在后台线程扫描（10 万文件内不冻结 UI）。

## 功能覆盖矩阵（对照书中 11 阶段）

| 阶段 | 覆盖 | 对应代码 |
|------|------|---------|
| ① MVP 骨架 | ✅ | `main.cpp` + `MainWindow` |
| ② UI 骨架 + 导航 | ✅ | `MainWindow` 左侧栏 + `QStackedWidget` |
| ③ 媒体扫描与列表 | ✅ | `MediaScanner`（后台 `QThread`） + `MediaListModel/Delegate` + `LibraryPage` |
| ④ 核心播放 + 状态机 | ✅ | `PlayerService`（`QMediaPlayer`+`QAudioOutput`+200ms 节流 tick）+ `PlayerBar` + `ProgressSlider` |
| ⑤ 视频播放 | ✅ 基础 | `PlayingPage` 内嵌 `QVideoWidget` + 全屏按钮 |
| ⑥ 本地持久化 SQLite | ✅ | `DbManager`（WAL/foreign_keys）+ `MediaRepository`（预处理绑定，防注入）|
| ⑦ LRC 歌词 | ✅ | `LyricsParser`（regex + 二分）+ `LyricsWidget`（QPainter + QPropertyAnimation）|
| ⑧ 搜索 + 主题 | ✅ | `LibraryPage` debounce 搜索 + `ThemeManager`（QStyleHints::colorScheme）|
| ⑨ 平台特化（MPRIS/SMTC/MPNowPlaying）| 🟡 骨架 | `SystemMediaAdapter` 抽象 + `StubAdapter` 占位；三端真实实现见文档 §09 路线图 |
| ⑩ EQ + 睡眠定时 | 🟡 睡眠定时 ✅；EQ 保留 | `SleepTimer`（QTimer + 线性淡出）；EQ 需对接 QAudioSink/平台特化 |
| ⑪ CMake 多平台构建 | ✅ | `CMakeLists.txt` 三端条件分支 + Bundle/Install |

## 项目结构

```text
mini-player-qt/
├── CMakeLists.txt            # 一份 CMake 产出三端二进制
├── README.md
├── resources/
│   └── style.qss             # 全局 QSS 设计系统（紫粉渐变 + 深浅两套）
└── src/
    ├── main.cpp              # 入口 + DB 初始化 + 主题 + 挂 QSS
    ├── types/
    │   └── MediaItem.h       # MediaItem / PlaybackState / LoopMode (Q_DECLARE_METATYPE)
    ├── data/                 # ② Repository 层
    │   ├── DbManager.{h,cpp} # SQLite WAL；默认连接名；建表迁移幂等
    │   ├── MediaRepository.{h,cpp}  # 全部预处理 + 绑定（SQL 注入防线）
    │   └── MediaScanner.{h,cpp}     # QDirIterator 后台扫描，分批 emit batchFound
    ├── service/              # ③ PlayerService 层
    │   ├── PlayerService.{h,cpp}    # 心脏：QMediaPlayer + 事件翻译 + seek 标志位 + 200ms 节流
    │   ├── LyricsParser.{h,cpp}     # LRC regex + sort + 二分
    │   ├── SleepTimer.{h,cpp}       # QTimer + 最后 10 秒音量线性淡出
    │   └── ThemeManager.{h,cpp}     # Qt 6.5+ colorScheme 跟随 / 持久化至 QSettings
    ├── ui/                   # ⑤ UI 层
    │   ├── MainWindow.{h,cpp}       # 三栏 shell：Sidebar + Stack + PlayerBar
    │   ├── LibraryPage.{h,cpp}      # 导入目录 + Tab + 搜索 + QListView(Model/Delegate)
    │   ├── PlayingPage.{h,cpp}      # 封面 + 元信息 + 歌词 + 视频页切换
    │   ├── SettingsPage.{h,cpp}     # 主题 + 睡眠定时 + 清空库 + 统计
    │   ├── PlayerBar.{h,cpp}        # 三段式底部控制条 + 自绘进度条集成
    │   ├── LyricsWidget.{h,cpp}     # QPainter 绘歌词 + QPropertyAnimation 滚动
    │   └── components/
    │       ├── ProgressSlider.{h,cpp}  # 自绘进度条：渐变 fill + hover 长高 + 光环 + 时间气泡 + ARIA 键盘
    │       ├── CoverLabel.{h,cpp}      # 封面 widget：id-hash 渐变占位 + 圆形模式 + clicked 信号
    │       ├── MediaListModel.{h,cpp}  # QAbstractListModel + 9 roles
    │       ├── MediaListDelegate.{h,cpp} # 自绘：封面占位 + 两行文字 + 时长 + 正在播放态
    │       └── ToastHost.{h,cpp}       # 全局 Toast（顶部中央毛玻璃风）
    └── platform/
        ├── SystemMediaAdapter.{h,cpp}  # 抽象接口 + create() 工厂
        └── stub/StubAdapter.{h,cpp}    # 空实现（§09 真实实现未上线前的占位）
```

## 快捷键

| 键 | 功能 |
|----|------|
| Space | 播放 / 暂停 |
| ← / → | 后退 / 前进 5 秒 |
| ↑ / ↓ | 音量 ±5% |
| M | 静音切换 |
| N / P | 下一首 / 上一首 |
| Tab + ←→ | 聚焦进度条后微调 5% |

## 关键设计（对照文档）

### 1. 为什么用 Qt Widgets 而不是 QML？
- Widgets 是 **"桌面级"** 设计语言：原生菜单、键盘导航、无障碍、焦点管理都免费
- 代码量更少、启动更快、调试更直观（没有 JS 引擎）
- QSS 已经够现代化（我们做出了与 Web 端一致的紫粉渐变 + 深浅主题）
- 文档 §01 的架构契约（MediaItem / PlayerService / seek 标志位）在 Widgets 下落地干净

### 2. QMediaPlayer vs ExoPlayer / AVPlayer / `<audio>`
三端信号对照见文档 §01 的"API 映射表"。本工程只用 Qt 6 的新 API：
- `QMediaPlayer::mediaStatusChanged`（替代 Qt 5 的 `stateChanged`）
- `QMediaPlayer::playbackStateChanged`（替代旧 `state()`）
- `QMediaPlayer::errorOccurred`（替代旧 `error()` 信号重载）
- 单独的 `QAudioOutput`（Qt 6 把音量/静音从 player 剥离出去了）

### 3. 后台扫描：QThread + moveToThread
不要继承 QThread（新手陷阱）；而是：
```cpp
auto* worker = new MediaScanner();     // 没 parent
worker->moveToThread(thread);
QMetaObject::invokeMethod(worker, "scanDirectories", Qt::QueuedConnection, Q_ARG(...));
```
这样 `scanner` 的所有槽都在 `thread` 跑，`emit batchFound(...)` 跨线程 autoConnection 自动 QueuedConnection 回到主线程 UI。

### 4. SQL 注入防线
`MediaRepository` 的**所有**查询都用 `QSqlQuery::prepare(":name")` + `bindValue()`，杜绝字符串拼接。见 `search()` 的 LIKE 用法：`q.bindValue(":kw", "%%%1%%".arg(kw.trimmed()))`。

## 平台特化路线图（§09）

当前 `SystemMediaAdapter::create()` 返回 `StubAdapter`。接入真实实现：

### Linux (MPRIS via D-Bus)
1. CMakeLists：`find_package(Qt6 REQUIRED COMPONENTS DBus)` + `Qt6::DBus`
2. 新建 `src/platform/linux/MprisAdapter.{h,cpp}`：实现 `org.mpris.MediaPlayer2` 和 `org.mpris.MediaPlayer2.Player` 接口
3. 工厂里加 `#if defined(Q_OS_LINUX) return new MprisAdapter(parent);`

### macOS (MPNowPlayingInfoCenter via Obj-C++)
1. 文件名改为 `.mm`（Objective-C++ 混合源文件），link `-framework MediaPlayer -framework AVFoundation`
2. CMakeLists：`enable_language(OBJCXX)` 已启用；追加 `target_link_libraries(... "-framework MediaPlayer")`
3. 实现 `MPNowPlayingInfoCenter` + `MPRemoteCommandCenter`

### Windows (SMTC via C++/WinRT)
1. CMakeLists：`cppwinrt.exe` 生成头 + `link WindowsApp.lib`
2. 实现 `SystemMediaTransportControls` + `GetForCurrentView`
3. 关键：SMTC 要求有一个 `IDesktopWindowXamlSourceNative` 关联 HWND

详细代码片段见文档 §09。

## 常见问题

**Q：构建报错 `Could not find a package configuration file provided by "Qt6"`？**
A：没传 `-DCMAKE_PREFIX_PATH=<Qt 安装目录>`。或者 Linux 下 `sudo apt install qt6-base-dev qt6-multimedia-dev qt6-tools-dev`。

**Q：MP3/AAC 无法播放？**
A：Qt Multimedia 在 Linux 依赖 GStreamer plugins（`gstreamer1.0-plugins-good/bad/ugly/libav`）；Windows/macOS 由系统解码器承载。

**Q：扫描大目录很慢？**
A：目前 scanner 对每个文件只取基础元数据（文件名、大小、mtime），不读 ID3 标签。真正的元数据在**首次播放时**由 `QMediaMetaData` 读取并 `upsertMetadata` 回库（§04 Step 4.6 的 lazy 策略）。

**Q：Mac 窗口关闭 app 还在后台？**
A：QApplication 默认 Mac 下 `quitOnLastWindowClosed = true`，关掉窗口即退出。如需 Dock 常驻改为 `false` 并处理 `aboutToQuit`。

## License

MIT · 仅供配套学习使用。
