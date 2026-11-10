# MiniPlayer · Electron 桌面端配套工程 (v0.1)

> 对应文档：[06.前端桌面端实现(Electron).md](../../packages/website/10.Apps/07.综合音视频案例/06.前端桌面端实现(Electron).md)
> 本工程是 MiniPlayer "**5 端 × 6 平台**" 教学案例中 Electron 篇的 **v0.1 最小闭环**，演示：
> - **Electron 三进程模型**（main / preload / renderer）
> - **自写 C++ 共享库 `mp_native`**（CMake 产出 Linux `.so` / macOS `.dylib` / Windows `.dll`）
> - **koffi FFI** 从 Node.js 调 C ABI 导出函数
> - 完整三端打包 (electron-builder)

---

## v0.1 覆盖的能力

| 功能 | 实现侧 | 对应文档 |
|------|-------|---------|
| Electron 三进程骨架（含 contextIsolation + sandbox 安全配置） | TS | §4.3 |
| koffi 加载 `.so/.dylib/.dll`，懒加载策略 | TS | §4.1-4.2 |
| C ABI 头文件 `mp_native.h`（3 原则落地） | C | §3.1-3.3 |
| `mp_version()` 版本自检 | C++ | §3 |
| `mp_probe()` 文件头 magic 嗅探（支持 10 种格式） | C++ | §5 阶段② |
| `mp_metadata_read()` 元数据提取（可选 TagLib） | C++ | §3.4 |
| `mp_lrc_parse()` LRC 歌词解析（演示变长数组返回） | C++ | §5 阶段⑦ |
| 系统媒体控制 Stub + 真实实现接入点 | C++ | §6 (接入点) |
| React 19 UI 调 preload → IPC → C++ 全链路 | TS | §5 阶段② |
| electron-builder 三端打包配置 | yml | §5 阶段⑨ |

> **v0.1 刻意保留的 TODO**：
> - MPRIS / MPNowPlaying / SMTC 真实实现（框架已经预留，开 `-DMP_WITH_SYSMEDIA=ON` 并补齐三个平台文件即可）
> - 媒体库 SQLite + Repository
> - Web Audio EQ / A-B 循环 / crossfade
> - 全局快捷键、字幕、PiP

---

## 快速开始

### 环境准备（三端差异）

| 平台 | 必装 |
|------|------|
| **Linux** | `sudo apt install cmake g++ pkg-config`（可选：`libtag1-dev libdbus-1-dev`） |
| **macOS** | `brew install cmake pkg-config`（可选：`brew install taglib`） + Xcode Command Line Tools |
| **Windows** | VS 2022 Build Tools + CMake（可选：`vcpkg install taglib:x64-windows`，并设置环境变量 `VCPKG_ROOT`） |
| **通用** | Node.js ≥ 20.11 + pnpm 9+ |

### 三步跑起来

```bash
# 1. 安装 Node 依赖
pnpm install

# 2. 构建 C++ 共享库（三端都走同一条命令）
pnpm native:build
# 产物:
#   resources/native/libmp_native.so    (Linux)
#   resources/native/libmp_native.dylib (macOS)
#   resources/native\mp_native.dll      (Windows)

# 3. 不启 Electron 先 smoke test 一下，确认 FFI 通路
pnpm smoke
# 期望输出:
#   → loading: /path/to/resources/native/libmp_native.{so|dylib|dll}
#   ✓ loaded
#     version: mp_native 0.1.0

# 4. 启动 Electron
pnpm dev
```

### 可选能力开关

```bash
# 启用 TagLib 做真实元数据提取（需先装 taglib）
pnpm native:build -- --taglib

# 启用系统媒体控制（Linux MPRIS / macOS MPNowPlaying / Windows SMTC）
pnpm native:build -- --sysmedia

# 两者同时启用
pnpm native:build -- --taglib --sysmedia

# 清理
pnpm native:clean
```

### 三端打包

```bash
pnpm dist:linux   # dist/MiniPlayer-0.1.0.AppImage + .deb
pnpm dist:mac     # dist/MiniPlayer-0.1.0-universal.dmg
pnpm dist:win     # dist/MiniPlayer Setup 0.1.0.exe
```

> ⚠ **macOS 公证**：若要分发给其他用户，还需对 `.app` 和内嵌的 `.dylib` 分别 codesign + notarize。详见文档 [§5 阶段⑨](../../packages/website/10.Apps/07.综合音视频案例/06.前端桌面端实现(Electron).md)。

---

## 项目结构

```text
mini-player-electron/
├── package.json
├── electron.vite.config.ts              # 三入口构建配置
├── electron-builder.yml                 # 三端打包配置
├── index.html                           # Vite renderer HTML entry
├── tsconfig.json / tsconfig.node.json
├── scripts/
│   ├── build-native.cjs                 # ⭐ 跨平台 CMake 构建脚本
│   └── smoke-test.cjs                   # ⭐ 不启 Electron 直接验证 FFI
├── src/
│   ├── main/                            # Node.js 主进程（Repository + FFI）
│   │   ├── index.ts                     # BrowserWindow + IPC 契约
│   │   └── native/
│   │       ├── loader.ts                # 平台路径解析
│   │       └── bindings.ts              # ⭐ koffi 签名绑定（最关键的 TS 文件）
│   ├── preload/
│   │   └── index.ts                     # contextBridge 白名单
│   ├── renderer/                        # React UI
│   │   ├── main.tsx
│   │   ├── App.tsx                      # v0.1 页面：版本自检 + 文件探测 + 音频播放
│   │   ├── App.css
│   │   └── env.d.ts
│   └── shared/
│       └── types.ts                     # 五端共享类型（MediaItem/PlayerState）
├── native/                              # ⭐ 独立 C++ 子工程
│   ├── CMakeLists.txt                   # 三端 .so/.dylib/.dll 产出
│   ├── include/mp_native.h              # ⭐ C ABI 唯一头文件
│   └── src/
│       ├── version.cpp                  # mp_version + mp_free
│       ├── probe.cpp                    # mp_probe 文件头嗅探
│       ├── metadata.cpp                 # mp_metadata_* (可选 TagLib)
│       ├── lrc.cpp                      # mp_lrc_* 变长数组返回示范
│       └── sysmedia/
│           ├── stub.cpp                 # 默认空实现
│           ├── mpris_linux.cpp          # TODO: Linux MPRIS D-Bus
│           ├── nowplaying_mac.mm        # TODO: macOS MPNowPlaying
│           └── smtc_win.cpp             # TODO: Windows SMTC
└── resources/native/.gitkeep            # CMake 产物最终位置
```

---

## FFI 边界三条铁律（必读）

对应文档 [§3.3](../../packages/website/10.Apps/07.综合音视频案例/06.前端桌面端实现(Electron).md)：

1. **所有权显式**：本库返回的堆内存必须通过 `mp_free` / `mp_metadata_release` / `mp_lrc_release` 释放
2. **变长数据三件套**：指针 + 长度 + 释放函数（禁止 `\0` 结尾字符串夹二进制数据）
3. **只传 C 基础类型**：string / int / buffer / POD struct；禁止传 C++ 对象或 STL 容器

**查看符号导出是否正确**（排查 `koffi.load` 成功但函数返回 0 的常见坑）：

```bash
# Linux
nm -gD resources/native/libmp_native.so | grep mp_
# macOS
nm -gU resources/native/libmp_native.dylib | grep mp_
# Windows (需 Visual Studio 开发者命令行)
dumpbin /EXPORTS resources\native\mp_native.dll | findstr mp_
```

应看到：
```
mp_free
mp_lrc_parse
mp_lrc_release
mp_metadata_read
mp_metadata_release
mp_probe
mp_sysmedia_init
mp_sysmedia_shutdown
mp_sysmedia_update
mp_version
```

---

## 常见问题

### Q: `Error: Could not load library` / `cannot open shared object file`
`.so/.dylib/.dll` 产物不在预期位置、或缺少系统依赖。
- Linux: `ldd resources/native/libmp_native.so`
- macOS: `otool -L resources/native/libmp_native.dylib`
- Windows: 用 Dependency Walker 查依赖

### Q: `mp_metadata_read failed（可能未启用 TagLib）`
默认构建不链 TagLib。装好 TagLib 后用 `pnpm native:build -- --taglib` 重新构建。

### Q: Electron 启动闪退 / 白屏
- 开发期：按 `F12` 打开 DevTools 看 renderer 错误
- 主进程错误：在 `pnpm dev` 的终端输出里查
- `app.isPackaged` 判断不同路径时常出 bug，可在 main/index.ts 打 `console.log(process.resourcesPath)` 验证

### Q: Windows 下为什么要 vcpkg？
Windows 的 C++ 库生态碎片严重，vcpkg 是微软官方跨编译器包管理器。
若你全家用 MinGW，也可以用 pacman 装 taglib，但 CMake 的 `find_package(TagLib)` 需要有相应的 CMake config 文件。

### Q: 为什么没用 node-addon-api（N-API）而用 koffi FFI？
两条路线各有优劣，详见文档 [§7 挑战题 1](../../packages/website/10.Apps/07.综合音视频案例/06.前端桌面端实现(Electron).md)。
简言之：**koffi 调用没有编译期绑定**（库换了不用重编 Node 侧），**N-API 性能略好且异常传播更自然**。本案例优先 koffi 是为了演示 "**C ABI 作为通用桥**" 的工程哲学。

---

## 跟 Qt 桌面篇的横向对照

| 维度 | Qt 篇 (`code/App/mini-player-qt/`) | 本工程 |
|------|-----------------------------------|-------|
| 代码量 (v0.1) | ~2600 行 C++ | 450 行 C++ + 500 行 TS + 配置 |
| 启动速度 | 冷启 < 500ms | 冷启 1.5-2.5s（Chromium 初始化） |
| 产物大小 | 30-50 MB | 150-200 MB（Electron 含 Chromium） |
| UI 一致性 | 平台原生外观 (QSS 可深度自定义) | Web UI (完全跨平台一致) |
| 用 C++ 的方式 | 作主语言 | 作服务提供者 (koffi FFI 边界) |
| 典型代表产品 | Krita / Qt Creator | VS Code / Figma Desktop / Obsidian |

**学完两篇最大的收获**：你会清楚"**何时该下沉到 C++**"的工程决策——这远比掌握某个具体框架更重要。
