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

## JS 调用 C ABI

Electron 场景完整链路。调用要穿过 **Electron 三进程 + FFI 边界**，共四层。

下面以消费方 `mini-player-electron` 的真实代码为例：读取媒体文件元数据 `mp_metadata_read`。数据流向：

```text
React 组件（renderer/App.tsx）
   │  window.api.readMetadata(p)              ← 只能看到 preload 白名单
   ▼
contextBridge（preload/index.ts）
   │  ipcRenderer.invoke('native:readMetadata', p)   ← 唯一穿越沙箱的桥
   ▼
ipcMain.handle（main/index.ts）                ← 参数校验、错误兜底
   ▼
koffi 绑定层（main/native/bindings.ts）
   │  fn.mp_metadata_read(path, out)          ← FFI 边界：参数/返回值 marshaling
   ▼
mp_native.dll / .dylib / .so  →  mp_metadata_read()  ← C ABI（操作系统固化的调用约定）
```

### 四层各自怎么写（以 `mp_metadata_read` 为例）

**① C 侧**（`include/mp_native.h`，已实现）：

```c
typedef struct {
  const char* title;        // UTF-8，可能为 NULL
  const char* artist;
  const char* album;
  int32_t     duration_ms;
  int32_t     bitrate_kbps;
  int32_t     sample_rate;
  int32_t     channels;
} MpMetadata;

MP_API int  mp_metadata_read(const char* file_path, MpMetadata* out);
MP_API void mp_metadata_release(MpMetadata* meta);
```

**② koffi 绑定**（`main/native/bindings.ts`，FFI 核心——翻译 C 签名）：

```ts
const lib = koffi.load(ensureNativeLibExists());      // 懒加载 .so/.dylib/.dll

// ⭐ koffi struct：字段名/顺序与 C 头文件一字不差
const MpMetadata = koffi.struct('MpMetadata', {
  title:        'string',
  artist:       'string',
  album:        'string',
  duration_ms:  'int32',
  bitrate_kbps: 'int32',
  sample_rate:  'int32',
  channels:     'int32',
});

const mpMetadataRead = lib.func('mp_metadata_read',
                         'int', ['string', koffi.out(koffi.pointer(MpMetadata))]);
const mpMetadataRelease = lib.func('mp_metadata_release', 'void', [koffi.pointer(MpMetadata)]);

export function readMetadata(path: string): NativeMetadata | null {
  const out: any = {};                                // koffi.out 出参容器
  const rc = mpMetadataRead(path, out);
  if (rc !== 0) return null;
  // ⭐ 先浅拷贝一份出来，再释放原生字段（字符串指针随 release 失效）
  const copy = { title: out.title ?? null, artist: out.artist ?? null,
                 album: out.album ?? null, duration_ms: out.duration_ms, /* … */ };
  mpMetadataRelease(out);
  return copy;
}
```

**③ IPC 通道**（main 进程注册 + preload 白名单）：

```ts
// main/index.ts：ipcMain.handle —— try/catch 把异常翻译成可序列化错误
ipcMain.handle('native:readMetadata', (_e, filePath: string) => {
  try {
    const m = getNative().readMetadata(filePath);
    if (!m) return { ok: false, reason: 'metadata_read failed（可能未启用 TagLib）' };
    return { ok: true, title: m.title ?? path.basename(filePath), durationMs: m.duration_ms, /* … */ };
  } catch (e: any) {
    return { ok: false, reason: e.message };
  }
});

// preload/index.ts：contextBridge 白名单——renderer 能看到的全部能力就这些
contextBridge.exposeInMainWorld('api', {
  readMetadata: (p: string) => ipcRenderer.invoke('native:readMetadata', p),
});
```

**④ renderer 调用**（React，类型来自 `shared/types.ts` 的 `Api` 接口）：

```ts
const m = await window.api.readMetadata(p);   // App.tsx 实际调用
```

### 三条必守规则

1. **出参用 `koffi.out`**：C 的 `T* out` 在 koffi 里是 `koffi.out(koffi.pointer(T))`，调用后从容器 `deref()` 取值；**堆内字符串先复制再调 `*_release`**（铁律①所有权配对）
2. **不透明句柄只存不碰**：`mp_kv*` 这类句柄在 JS 侧是不透明指针值，只保存、传递，永不解引用；`open/close` 必须配对
3. **回调必须跨线程 marshal**：C 回调（如 `mp_log_sink_cb`、sysmedia 回调）在库的线程触发，Node 的 `webContents.send` 只能在主线程调——回调体先 `setImmediate` 再碰 Electron API

> koffi struct 字段**顺序与 C 头文件一字不差**；字符串字段 koffi 自动 UTF-8 转换，但仅到 release 前有效。

## 消费

```cmake
# CMake：find_package(mp-native 1.0 REQUIRED)  →  mp::mp_native
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
