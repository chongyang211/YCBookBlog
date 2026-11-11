# mp_native —— 独立跨平台原生库

> MiniPlayer 的 C++ 原生扩展库，**可独立于 Electron 工程构建、安装、分发**。
> 产出一份 C ABI 头文件 + 三平台共享库（`.so` / `.dylib` / `.dll`），供任何能调 C 的语言消费。
>
> 对应文档：`packages/website/10.Apps/05.综合音视频案例/06.Electron篇.md` §04
> 版本：`1.0.0`（语义化版本，与 `mp_version()` 返回值一致）

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
# 从 Electron 工程根目录（mini-player-electron/）
pnpm native:build          # 产物自动落在 resources/native/
pnpm smoke                 # Node/koffi 直接加载验证
```

`MP_EMBED_OUTPUT=ON`（默认）：产物**跳过 build 目录**直接写 `../resources/native/`，`scripts/build-native.cjs` 与 `electron-builder.yml` 的 `extraResources` 都依赖这一行为。

### 模式 B：独立分发（本库作为独立工程）

```bash
cd native

# 方式一：CMakePresets（推荐）
cmake --preset release                 # 独立模式（不入 resources/）
cmake --build --preset release -j
cmake --install build --prefix dist    # → dist/{include,lib,bin}
ctest --preset smoke                   # 需 --preset examples 构建

# 方式二：一键打包脚本
bash scripts/package-native.sh --taglib      # Linux / macOS
.\scripts\package-native.ps1 -TagLib         # Windows

# 方式三：手工 cmake（与预设等价）
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DMP_EMBED_OUTPUT=OFF
cmake --build build -j
cmake --install build --prefix dist
```

**dist 布局**（安装后的标准目录，可直接 zip 分发）：

```text
dist/
├── include/mp_native.h              # 唯一头文件（C ABI 契约）
├── lib/
│   ├── libmp_native.so / .dylib     # Unix
│   ├── mp_native.lib                # Windows 导入库
│   └── cmake/mp-native/             # CMake 包（find_package 用）
│       ├── mp-nativeConfig.cmake
│       ├── mp-nativeConfigVersion.cmake
│       └── mp-nativeTargets.cmake
├── bin/mp_native.dll                # Windows 运行时
└── mp-native-1.0.0-<平台>-<架构>.zip
```

---

## 三种消费方式

### 1. CMake 工程（C/C++ 原生消费）

```cmake
# 消费方的 CMakeLists.txt
find_package(mp-native 1.0 REQUIRED)     # -Dmp-native_DIR=/path/to/dist/lib/cmake/mp-native
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
const lib = koffi.load('/path/to/dist/lib/libmp_native.dylib');  // 按平台选文件
const mp_version = lib.func('mp_version', 'string', []);
console.log(mp_version());   // "mp_native 1.0.0"
```

完整绑定层参考 `../src/main/native/bindings.ts`（结构体镜像 + 所有权规则）。

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
- **无 ELF/Dylib 版本号后缀**：产物文件名固定为 `libmp_native.so/.dylib`（避免 Electron `extraResources` 打包符号链接失效的坑）；版本追溯靠 zip 文件名 + `mp_version()`

---

## 目录结构

```text
native/
├── CMakeLists.txt              # 双模式工程（嵌入 / 独立）
├── CMakePresets.json           # dev / release / full / universal / win-x64 预设
├── .clangd                     # IDE 符号跳转
├── cmake/
│   └── mp-nativeConfig.cmake.in   # CMake 包模板
├── include/
│   └── mp_native.h             # ⭐ C ABI 唯一契约
├── src/
│   ├── version.cpp  probe.cpp  metadata.cpp  lrc.cpp
│   └── sysmedia/{stub,mpris_linux,nowplaying_mac,smtc_win}
├── examples/
│   └── smoke.c                 # 纯 C 消费示例（ctest 接入）
├── scripts/
│   ├── package-native.sh       # Linux/macOS 一键打包
│   └── package-native.ps1      # Windows 一键打包
└── dist/                       # 打包输出（gitignore）
```

---

## CI 分发流水线（建议）

三平台 matrix 各跑 `package-native.*`，上传 zip artifact，打 tag 时发 GitHub Release：

```yaml
strategy:
  matrix:
    include:
      - { os: ubuntu-24.04, script: bash native/scripts/package-native.sh --taglib --sysmedia }
      - { os: macos-14,     script: bash native/scripts/package-native.sh --taglib --sysmedia --universal }
      - { os: windows-2022, script: pwsh native/scripts/package-native.ps1 -TagLib -SysMedia }
```

> macOS universal 产物建议在本篇 §11 的公证流水线中对 `.dylib` 单独签名后再分发。
