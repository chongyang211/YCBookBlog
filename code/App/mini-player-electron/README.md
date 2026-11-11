# MiniPlayer · Electron 桌面端配套工程

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

# 2. 准备当前平台的 mp_native 产物（dev 命令也会自动执行）
pnpm prepare:native
# 解析优先级：
#   ① ../mp-native/out/multi/<platform-arch>/  的多平台预编译产物（推荐，见下文「产物接入」）
#   ② 回退源码构建（等价于 pnpm native:build，需要 CMake 工具链）
# 产物落到 resources/native/

# 3. 不启 Electron 先 smoke test 一下，确认 FFI 通路
pnpm smoke

# 4. 启动 Electron
pnpm dev
```

### 可选能力开关

```bash
# 启用 TagLib 做真实元数据提取（需先装 taglib）
pnpm native:build -- --taglib

# 启用系统媒体控制（Linux MPRIS / macOS MPNowPlaying / Windows SMTC）
pnpm native:build -- --sysmedia

# 多平台产物一键构建（darwin 本机 + linux/windows 经 Docker，见 native/README.md）
pnpm native:multi

# 清理
pnpm native:clean
```

### 三端打包

```bash
pnpm dist:linux   # 自动取 linux-x64 产物 → dist/MiniPlayer-0.1.0.AppImage + .deb
pnpm dist:mac     # 自动 lipo 合并 arm64+x64 → dist/MiniPlayer-0.1.0-universal.dmg
pnpm dist:win     # 自动取 win32-x64 产物 → dist/MiniPlayer Setup 0.1.0.exe
```

> dist 前会自动执行 `fetch-native`：优先取 `mp-native/out/multi/`，缺失时回退源码构建——所以 **CI 上只要有对应平台的产物（或工具链）就能打包，本机不需要全平台 CMake 工具链**。

> ⚠ **macOS 公证**：若要分发给其他用户，还需对 `.app` 和内嵌的 `.dylib` 分别 codesign + notarize。详见文档 [§5 阶段⑨](../../packages/website/10.Apps/05.综合音视频案例/06.Electron桌面篇.md)。

---

## 单工程策略与 native 产物接入

### 为什么应用 + native 放一个工程？

**推荐保持单工程**（当前结构），理由：

1. **原子变更**：改一个 C ABI 签名 → 头文件、koffi 绑定层、消费代码在同一次 commit 里，永不出现"库和应用版本错位"
2. **构建脚本复用**：`build-native.cjs` 直接知道产物该落到哪、以什么名字
3. **边界依然清晰**：`native/` 是**可独立构建/安装/分发的自洽子工程**（有自己的 CMakePresets / install 规则 / README），拆出去的成本随时为零
4. 拆成独立库仓库的正确时机：多个应用消费同一个库、或 C++ 侧要独立发版——届时应用侧只需把 `MP_NATIVE_MULTI_DIR` 指向"下载解压后的产物目录"，`fetch-native.cjs` 的接口不变

### 产物接入流程（fetch-native）

```text
mp-native/out/multi/                ← 独立库工程的多平台产物（源）
    darwin-arm64/libmp_native.dylib      │
    darwin-x64/libmp_native.dylib        │  目录名 = Electron 的
    linux-arm64/libmp_native.so          │  ${process.platform}-${process.arch}
    linux-x64/libmp_native.so            │
    win32-x64/mp_native.dll              │
    win32-ia32/mp_native.dll             │
         │  node scripts/fetch-native.cjs（dev/dist 前自动执行）
         ▼
resources/native/                   ← 应用运行时加载点
    libmp_native.dylib              ← 当前平台（默认模式）
    或 libmp_native.dylib(fat)      ← --universal（lipo 合并，配 mac universal 包）
    或 <platform-arch>/<lib> ×N     ← --all（打包时携带多平台，loader 按运行时平台挑）
```

```bash
pnpm prepare:native               # 当前平台：multi/ 优先，缺失回退源码构建
pnpm prepare:native:universal     # darwin: 合并 arm64+x64 为 fat dylib（dist:mac 自动用）
pnpm prepare:native:all           # multi/ 全部平台整目录带入 tag 子目录（loader 运行时自选）
```

`loader.ts` 按优先级查找：`resources/native/<platform-arch>/<lib>` → `resources/native/<lib>`，开发期与打包后（`process.resourcesPath`）行为一致。

---

## 项目结构（双工程）

本案例拆分为**两个独立工程**，通过「标准产物接口」协作：

```text
code/App/
├── mp-native/                           # ⭐ 工程一：独立 C++ 库（可单独构建/发版）
│   ├── CMakeLists.txt                   # 双模式：嵌入输出 或 标准 install
│   ├── CMakePresets.json                # dev / release / full / universal 预设
│   ├── include/mp_native.h              # ⭐ C ABI 唯一契约头
│   ├── src/                             # version/probe/metadata/lrc + sysmedia
│   ├── examples/smoke.c                 # 纯 C 消费示例（ctest）
│   ├── scripts/                         # multi-platform.sh + package-native.sh/.ps1
│   └── out/multi/                       # ⭐ 多平台产物（六平台 + 每平台 zip）
│   └── README.md                        # 库的构建/消费/版本策略文档
│
└── mini-player-electron/                # ⭐ 工程二：Electron 应用（本目录）
    ├── package.json
    ├── electron.vite.config.ts          # 三入口构建配置
    ├── electron-builder.yml             # 三端打包配置
    ├── index.html
    ├── tsconfig.json / tsconfig.node.json
    ├── scripts/
    │   ├── build-native.cjs             # 源码构建回退（调 ../mp-native 的 CMake）
    │   ├── fetch-native.cjs             # ⭐ 产物接入：multi/ 优先 → resources/native/
    │   └── smoke-test.cjs               # 不启 Electron 直接验证 FFI
    ├── src/
    │   ├── main/                        # Node.js 主进程（Repository + FFI）
    │   │   ├── index.ts                 # BrowserWindow + IPC 契约
    │   │   └── native/
    │   │       ├── loader.ts            # 平台路径解析（tag 目录优先）
    │   │       └── bindings.ts          # ⭐ koffi 签名绑定（最关键的 TS 文件）
    │   ├── preload/index.ts             # contextBridge 白名单
    │   ├── renderer/                    # React UI（main/App/pages/components）
    │   └── shared/types.ts              # 五端共享类型（MediaItem/PlayerState）
    └── resources/native/                # 运行时加载点（fetch 布点，产物 gitignore）
```

**协作契约**（两边唯一的耦合点，共三条）：

1. 目录命名：`multi/<platform-arch>/` ≡ Electron 的 `${process.platform}-${process.arch}`
2. 文件命名：`libmp_native.so | libmp_native.dylib | mp_native.dll`
3. C ABI：`mp_native.h` 的 10 个导出符号

**路径可配置**（库迁独立仓库/换目录时应用零改动）：

| 环境变量 | 作用 | 默认值 |
|---------|------|-------|
| `MP_NATIVE_DIR` | 源码构建回退时的库工程根 | `../mp-native` |
| `MP_NATIVE_MULTI_DIR` | 预编译产物目录 | `../mp-native/out/multi` |

---

## FFI 边界三条铁律（必读）

对应文档 [§3.3](../../packages/website/10.Apps/05.综合音视频案例/06.Electron桌面篇.md)：

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
两条路线各有优劣，详见文档 [§7 挑战题 1](../../packages/website/10.Apps/05.综合音视频案例/06.Electron桌面篇.md)。
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
