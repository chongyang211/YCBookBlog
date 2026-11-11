# mp_native —— 独立跨平台原生库

MiniPlayer 的 C++ 原生扩展库，**可独立于 Electron 工程构建、安装、分发**。

MiniPlayer 的 C++ 原生扩展库，演进为**通用跨平台基础能力库**（core）+ 媒体业务能力（media/extras），
一套 C API，消费方覆盖 Windows / macOS / Linux / Android（iOS 规划中）。

产出 C ABI 头文件组 + 各平台共享库（`.so` / `.dylib` / `.dll`），供任何能调 C 的语言消费。

---

## 模块总览

| 模块 | 头文件 | 依赖 | 平台 | 内容 |
|------|-------|------|------|------|
| **common** | `mp_def.h` | 无 | 全平台 | `MP_API` 导出宏、统一错误码、`mp_error_string()` |
| **core/kv** | `mp_kv.h` | 无 | 全平台 | K-V 存储：append-only 日志 + 内存索引，崩溃安全，可压实 |
| **core/log** | `mp_log.h` | 无 | 全平台 | 日志：文件 sink + 宿主回调 sink（转发 logcat/OSLog），线程安全 |
| **media** | `mp_native.h` | 无 | 全平台 | 文件头嗅探、LRC 解析 |
| **extras/metadata** | `mp_native.h` | TagLib（可选） | 桌面 | 元数据提取（`MP_WITH_TAGLIB=ON`） |
| **extras/sysmedia** | `mp_native.h` | D-Bus / MediaPlayer / WinRT（可选） | 桌面 | MPRIS / MPNowPlaying / SMTC（`MP_WITH_SYSMEDIA=ON`） |

> **core 与 extras 的边界**：core 零第三方依赖，任何平台（含 Android NDK / iOS）开箱即编；
> extras 依赖系统/第三方库，通过编译开关按需启用。所有模块打进**同一个共享库**——FFI 消费方只加载一个文件。

## core 模块 API

### K-V 存储（`mp_kv.h`）

```c
mp_kv* db = NULL;
mp_kv_open("/path/to/data-dir", &db);            // 目录由宿主注入（库不猜路径）

mp_kv_put(db, "user:name", "yangchong", 9);      // 二进制安全（可存任意字节）
void* v = NULL; size_t n = 0;
mp_kv_get(db, "user:name", &v, &n);              // 堆返回，mp_free 释放
char* s = NULL;
mp_kv_get_str(db, "user:name", &s);              // 便利：C 字符串版
mp_kv_has / mp_kv_delete / mp_kv_count
mp_kv_foreach(db, cb, user);                     // 快照遍历，回调可提前停止
mp_kv_compact(db);                               // 压实：清除墓碑与旧值
mp_kv_close(db);
```

| 特性 | 说明 |
|------|------|
| 持久化格式 | append-only 日志（`dir/kv.log`）：`[op u8][klen u32][vlen u32][key][value]` |
| 崩溃安全 | 重放遇到不完整尾部记录自动停止（视为未提交） |
| 线程模型 | 同句柄多线程安全（内部锁）；**不支持多进程**同开一目录（返回 `MP_ERR_STATE`） |
| 容量限制 | key ≤ 1024B · value ≤ 64MB |
| 路径规则 | **由宿主注入**（Android: `getFilesDir()` / iOS: 沙盒 / 桌面: userData）——库不猜路径 |

### 日志（`mp_log.h`）

```c
mp_log_init("/path/to/app.log", MP_LOG_DEBUG);   // 文件 sink
mp_log_add_sink(on_log, NULL);                   // 宿主回调 sink（转发 logcat/OSLog/...）
mp_log_write(MP_LOG_INFO, "kv", "opened at %s", dir);   // printf 风格
mp_log_set_level(MP_LOG_ERROR);                  // 运行时调级别
mp_log_shutdown();
```

输出格式：`2026-11-11 18:55:30.219 [INFO] [kv] opened at ...`（本地时区，毫秒精度）。
回调在**无锁状态**调用——宿主回调里再写日志不会死锁。

### 各语言消费示例

| 语言 | 加载方式 |
|------|---------|
| C/C++ | `#include "mp_kv.h"` + 链接（或 CMake `find_package(mp-native)`） |
| Electron/Node | koffi：`lib.func('mp_kv_open', 'int', ['string', koffi.out(koffi.pointer(koffi.pointer('void*')))])` |
| Python | ctypes：`lib.mp_kv_open.argtypes = [c_char_p, POINTER(c_void_p)]` |
| Android | JNI 桥接（见下文「Android 接入路线」） |
| Rust | `extern "C"` 声明 + `std::os::raw` |

## Android 接入路线

1. **构建**：NDK 交叉编译（`-DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a`），core 模块零改动可编（纯 C++17 + POSIX mkdir）
2. **桥接**：薄 JNI 层（一个 `mp_jni.cpp`），把 C API 包成 `extern "C" JNIEXPORT` 函数，Java/Kotlin 侧 `System.loadLibrary("mp_native")`
3. **路径注入**：`mp_kv_open(context.getFilesDir() + "/kv", ...)` —— 路径永远由 Java 层传入
4. **日志转发**：`mp_log_add_sink` 回调里调 `__android_log_print`
5. extras 模块（sysmedia/taglib）在 Android 上**不编译**——编译开关天然隔离

```kotlin
// 消费示例（Kotlin）
external fun mpKvOpen(dir: String): Long          // 返回句柄
external fun mpKvPut(h: Long, key: String, value: ByteArray): Int
```

## iOS 接入（规划）

- `MP_BUILD_STATIC` 选项产出 `.a`（iOS 禁止动态加载），Swift 经 bridging header 直接调 C API
- 路径注入同理（`FileManager.urls(for: .documentDirectory, ...)`）

## 路线图

| Phase | 内容 | 状态 |
|-------|------|------|
| P1 | core：kv + log + C 测试 + 多平台产物 | ✅ 完成 |
| P2 | Android JNI 桥 + NDK 构建脚本 + 示例工程 | 规划 |
| P3 | iOS 静态库 + Swift 消费示例 | 规划 |
| P4 | 其他语言绑定自动化（Rust bindgen / Kotlin） | 设想 |

---

## 产物矩阵

| 平台 | 库产物 | 链接产物 | 最低工具链 |
|------|-------|---------|-----------|
| Linux x64 / arm64 | `libmp_native.so` | — | g++ ≥ 11 · CMake ≥ 3.26 |
| macOS arm64 / x86_64 / **universal** | `libmp_native.dylib` | — | clang + Xcode CLT |
| Windows x64 | `mp_native.dll` | `mp_native.lib`（导入库） | MSVC 2022 + Windows SDK |

可选能力开关（都关 = 零第三方依赖，任何机器开箱即编）：

| 开关 | 引入的依赖 | 能力 |
|------|-----------|------|
| `MP_WITH_TAGLIB=ON` | TagLib 2.x | 真实元数据提取（默认 OFF，`mp_metadata_read` 返回 -3） |
| `MP_WITH_SYSMEDIA=ON` | libdbus / MediaPlayer.framework / WinRT | MPRIS / MPNowPlaying / SMTC |
| `MP_BUILD_EXAMPLES=ON` | — | 纯 C 冒烟示例 `mp_smoke` + ctest |
| `MP_UNIVERSAL_MACOS=ON` | — | macOS arm64+x86_64 双架构 |
| `MP_EMBED_OUTPUT=ON`（默认） | — | 产物直出 `../resources/native`（Electron 开发流） |

---

## 两种构建模式

### 模式 A：嵌入 Electron（默认，本仓开发流）

```bash
# 从 Electron 工程根目录（mini-player-electron/，与本库工程同级）
pnpm native:build          # 产物自动落在 resources/native/
pnpm smoke                 # Node/koffi 直接加载验证
```

`MP_EMBED_OUTPUT=ON`（默认）：产物**跳过 build 目录**直接写 `../resources/native/`，`scripts/build-native.cjs` 与 `electron-builder.yml` 的 `extraResources` 都依赖这一行为。

### 模式 B：独立分发（本库作为独立工程）

```bash
# 方式一：CMakePresets（推荐）
cmake --preset release                 # 独立模式（构建目录 out/build-standalone/，不入 resources/）
cmake --build --preset release -j
cmake --install out/build-standalone --prefix out/dist    # → out/dist/{include,lib,bin}
ctest --preset smoke                   # 需 --preset examples 构建

# 方式二：一键打包脚本
bash scripts/package-native.sh --taglib      # Linux / macOS
.\scripts\package-native.ps1 -TagLib         # Windows

# 方式三：手工 cmake（与预设等价）
cmake -S . -B out/build-standalone -DCMAKE_BUILD_TYPE=Release -DMP_EMBED_OUTPUT=OFF
cmake --build out/build-standalone -j
cmake --install out/build-standalone --prefix out/dist
```

**out/dist 布局**（安装后的标准目录，目录级分发）：

```text
out/dist/
├── include/mp_native.h              # 唯一头文件（C ABI 契约）
├── lib/
│   ├── libmp_native.so / .dylib     # Unix
│   ├── mp_native.lib                # Windows 导入库
│   └── cmake/mp-native/             # CMake 包（find_package 用）
│       ├── mp-nativeConfig.cmake
│       ├── mp-nativeConfigVersion.cmake
│       └── mp-nativeTargets.cmake
├── bin/mp_native.dll                # Windows 运行时
└── （目录整体即分发单元；如需压缩请消费方自行处理）
```

---

## 多平台产物（out/multi/）

一条命令产出 Electron 生态惯例的 platform-arch 目录（本机 darwin 双架构 + Docker 容器产出 linux/windows）：

```bash
bash scripts/multi-platform.sh             # 全平台（需要 Docker）
bash scripts/multi-platform.sh --no-docker # 只构建 darwin 两平台
```

```text
out/multi/
├── darwin-arm64/libmp_native.dylib    # Mach-O arm64（本机原生）
├── darwin-x64/libmp_native.dylib      # Mach-O x86_64（AppleClang 交叉）
├── linux-arm64/libmp_native.so        # ELF aarch64（容器本机架构）
├── linux-x64/libmp_native.so          # ELF x86-64（容器内交叉，ARM 容器也适用）
├── win32-x64/mp_native.dll            # PE32+ x64（MinGW-w64 交叉，静态链接 C++ 运行时）
├── win32-ia32/mp_native.dll           # PE32 i386（MinGW-w64 i686 交叉）
└── （每个平台目录即分发单元，无额外打包文件）
```

**实现要点**：
- Docker 侧用 `docker cp` 注入源码（**不依赖 bind mount**，本地/远程 Docker 引擎通用）
- ARM 容器会自动附带产出 `linux-arm64`；x86 容器则产出 `linux-x64`，目录名跟随实际架构
- MinGW 产物已 `-static-libgcc -static-libstdc++`，dll 自包含，目标机器无需额外运行时
- 此布局可直接对接 electron-builder / koffi 的 `process.platform-arch` 目录约定

**CI 替代方案**（无本机 Docker 时）：GitHub Actions matrix（ubuntu 出 linux-x64、windows 出两个 dll、macos 出两个 dylib），上传 artifact 汇聚成同样的 multi/ 布局。

---

## 三种消费方式

### 1. CMake 工程（C/C++ 原生消费）

```cmake
# 消费方的 CMakeLists.txt
find_package(mp-native 1.0 REQUIRED)     # -Dmp-native_DIR=/path/to/out/dist/lib/cmake/mp-native
target_link_libraries(myapp PRIVATE mp::mp_native)
```

```c
#include "mp_native.h"
printf("%s\n", mp_version());
```

`mp::mp_native` target 自动携带 include 路径与链接，无需手工配置。

### 2. Node.js / Electron（koffi FFI 消费）

```js
const koffi = require('koffi');
const lib = koffi.load('/path/to/out/dist/lib/libmp_native.dylib');  // 按平台选文件
const mp_version = lib.func('mp_version', 'string', []);
console.log(mp_version());   // "mp_native 1.0.0"
```

完整绑定层参考 Electron 工程 `../mini-player-electron/src/main/native/bindings.ts`（结构体镜像 + 所有权规则）。

### 3. 其他 FFI 语言（同一条 C ABI）

```python
# Python ctypes 示例
import ctypes
lib = ctypes.CDLL("./libmp_native.so")
lib.mp_version.restype = ctypes.c_char_p
print(lib.mp_version().decode())          # mp_native 1.0.0
```

Rust (`extern "C"`)、Go (cgo)、C# (P/Invoke) 均可按 `mp_native.h` 直接声明——**C ABI 是通用语**。

---

## API 一览（10 个导出符号）

| 符号 | 用途 | 所有权 |
|------|------|-------|
| `mp_version` | 版本字符串 | 静态字符串，**不释放** |
| `mp_free` | 释放本库分配的堆内存 | 配对 malloc |
| `mp_probe` | 文件头嗅探（10 种格式） | 返回枚举 int |
| `mp_metadata_read` / `mp_metadata_release` | 元数据（可选 TagLib） | 结构体字段配对释放 |
| `mp_lrc_parse` / `mp_lrc_release` | LRC 歌词解析 | 单块内存，一次释放 |
| `mp_sysmedia_init` / `mp_sysmedia_update` / `mp_sysmedia_shutdown` | 系统媒体控制（可选） | 回调须自行 marshal 主线程 |

**C ABI 三铁律**（详见文档 §04.3）：
1. **所有权显式**：本库分配的内存只用本库的 release 函数释放（跨 CRT 堆混用必崩）
2. **变长数据三件套**：指针 + 长度 + 释放函数
3. **只传 C 基础类型**：string / int / POD struct

---

## 版本与兼容策略

- **语义化版本**：`project(mp_native VERSION 1.0.0)` 与 `mp_version()` 返回值保持同步
- **C ABI 兼容承诺**：MAJOR 版本内，已有符号签名不变、只增不删
- **CMake 包兼容**：`SameMajorVersion`——消费方 `find_package(mp-native 1.0)` 在 1.x 内通用
- **无 ELF/Dylib 版本号后缀**：产物文件名固定为 `libmp_native.so/.dylib`（避免 Electron `extraResources` 打包符号链接失效的坑）；版本追溯靠 `mp_version()`

---

## 目录结构

```text
mp-native/
├── CMakeLists.txt              # 双模式工程（嵌入 / 独立）
├── CMakePresets.json           # dev / release / full / universal / win-x64 预设
├── .clangd                     # IDE 符号跳转
├── cmake/
│   └── mp-nativeConfig.cmake.in   # CMake 包模板
├── include/
│   ├── mp_native.h             # ⭐ C ABI 契约（media/extras）
│   ├── mp_def.h                # 公共：导出宏 + 错误码
│   ├── mp_kv.h                 # core：K-V 存储
│   └── mp_log.h                # core：日志
├── src/
│   ├── version.cpp             # mp_version / mp_free / mp_error_string
│   ├── core/                   # 零依赖基础能力（kv.cpp / log.cpp）
│   ├── probe.cpp  metadata.cpp  lrc.cpp
│   └── sysmedia/{stub,mpris_linux,nowplaying_mac,smtc_win}
├── examples/
│   └── smoke.c                 # 纯 C 消费示例（ctest 接入）
├── tests/
│   └── core_test.c             # core 模块回归测试
├── scripts/
│   ├── multi-platform.sh       # 六平台一键构建
│   ├── container-build.sh      # 容器内构建（被 multi-platform 调用）
│   ├── package-native.sh       # Linux/macOS 独立分发打包
│   └── package-native.ps1      # Windows 独立分发打包
└── out/                        # ⭐ 全部输出统一收敛（gitignore 单条 out/）
    ├── build/                  # 嵌入模式 CMake 构建
    ├── build-standalone/       # 预设独立构建
    ├── build-multi/            # 多平台脚本中间产物
    ├── build-dist/             # 独立分发包构建
    ├── dist/                   # install 树（include/ lib/ cmake 包）
    └── multi/                  # ⭐ 最终六平台产物（消费方接口）
```

---

## CI 分发流水线（建议）

三平台 matrix 各跑 `package-native.*`，上传 `out/dist` 目录 artifact，打 tag 时发 GitHub Release：

```yaml
strategy:
  matrix:
    include:
      - { os: ubuntu-24.04, script: bash scripts/package-native.sh --taglib --sysmedia }
      - { os: macos-14,     script: bash scripts/package-native.sh --taglib --sysmedia --universal }
      - { os: windows-2022, script: pwsh scripts/package-native.ps1 -TagLib -SysMedia }
```

> macOS universal 产物建议在本篇 §11 的公证流水线中对 `.dylib` 单独签名后再分发。
