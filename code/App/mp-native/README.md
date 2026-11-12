# mp_native —— 独立跨平台原生库

一套 C API，产物覆盖 Windows / macOS / Linux，供任何能调 C 的语言消费。

## 模块与产物

| 模块 | 头文件 | 说明 |
|------|-------|------|
| common | `mp_def.h` | 导出宏、统一错误码、`mp_error_string()` |
| core/kv | `mp_kv.h` | K-V 存储：append-only 日志 + 内存索引，崩溃安全 |
| core/log | `mp_log.h` | 日志：文件 sink + 宿主回调 sink（转发 logcat/OSLog） |
| media | `mp_native.h` | 文件头嗅探、LRC 解析 |
| extras | `mp_native.h` | 元数据（TagLib）、系统媒体控制——`MP_WITH_TAGLIB` / `MP_WITH_SYSMEDIA` 开关，默认关 |

产物（所有模块打进同一个库）：Linux `libmp_native.so` · macOS `libmp_native.dylib`（可选 universal）· Windows `mp_native.dll`

## 构建

```bash
# 日常开发（Electron 工程内）：产物直出 ../mini-player-electron/resources/native/
node scripts/build.cjs smoke                        # core 测试 + 纯 C 冒烟
node scripts/build.cjs package [--taglib] [--sysmedia] [--universal]   # → out/dist/
# 多平台产物（darwin 本机双架构 + Docker 容器出 linux/windows）→ out/multi/
node scripts/build.cjs multi [--no-docker] [--taglib] [--sysmedia]
# 纯 C++ 开发（无需 Node）：CMake 原生预设
cmake --preset release && cmake --build --preset release -j
```

## 消费

```cmake
# CMake：find_package(mp-native 1.0 REQUIRED)  →  mp::mp_native
# Node：koffi.load('<平台库路径>')  +  lib.func('mp_kv_open', ...)
# Python/Rust/Go：按头文件声明 extern "C" 即可
```

```c
// K-V 示例（完整 API 见 include/*.h）
mp_kv* db = NULL;
mp_kv_open("/data/dir", &db);                    // 路径由宿主注入，库不猜路径
mp_kv_put(db, "key", "value", 5);
void* v = NULL; size_t n = 0;
mp_kv_get(db, "key", &v, &n);                    // 堆返回 → mp_free 释放
mp_kv_close(db);
```

## C ABI 接口总览（26 个符号，6 组）

| 组 | 符号（`mp_` 前缀） | 状态管理 | 所有权约定 |
|----|-------------------|---------|-----------|
| 基础 ×3 | `version` `free` `error_string` | 无 | 静态字符串不释放 / 唯一释放口 |
| kv ×10 | `kv_open/put/get/get_str/delete/has/count/foreach/compact/close` | 不透明句柄 `mp_kv*` | `get` 堆返回 → `mp_free`；`open/close` 配对 |
| log ×5 | `log_init/set_level/add_sink/write/shutdown` | 进程级单例 | 回调无锁调用 |
| media ×5 | `probe` `metadata_read/release` `lrc_parse/release` | 无 | 变长返回配对释放 |
| sysmedia ×3 | `sysmedia_init/update/shutdown` | 回调 + 平台会话 | 回调在非主线程，宿主自行 marshal |

设计原则：C ABI 唯一边界 · 不透明句柄 · 所有权配对 · 长度显式传递 · 回调无锁 · 路径宿主注入 · 编译开关隔离平台 · 版本编译期注入。

## C ABI 三铁律

1. **所有权显式**：本库分配的内存只用本库的 `mp_free` / `*_release` 释放（跨 CRT 堆混用必崩）
2. **变长数据三件套**：指针 + 长度 + 释放函数
3. **只传 C 基础类型**：string / int / POD struct（禁止 C++ 对象 / STL）

## 平台备注

- **Android**：NDK 直接编译 core（零改动）；薄 JNI 桥 + 路径由 `getFilesDir()` 注入 + sink 回调转 `__android_log_print`
- **iOS（规划）**：静态库 + Swift bridging header
- **多进程**：KV 不支持多进程同开一目录（返回 `MP_ERR_STATE`）

## 版本与兼容

语义化版本（`mp_version()` 同步）；MAJOR 内 C ABI 只增不删；CMake 包 `SameMajorVersion` 兼容；产物文件名固定无版本后缀（规避 extraResources 符号链接坑）。

## 目录速览

```text
include/   C ABI 头文件（mp_def / mp_kv / mp_log / mp_native）
src/       core/（kv、log）+ media + sysmedia 平台分流
scripts/   build.cjs 统一入口 + container-build.sh（容器内构建）
tests/     core 回归测试（ctest）
out/       全部输出（build* / dist / multi）
```
