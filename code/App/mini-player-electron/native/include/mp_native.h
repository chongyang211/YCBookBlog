// mp_native.h —— C ABI 唯一导出头
//
// 规则：
//   1. 仅使用 C 基础类型；字符串用 UTF-8 const char*
//   2. 变长数据用「指针 + 长度 + 释放函数」三件套
//   3. 所有权显式：谁分配谁释放（本库分配必通过 mp_free* 释放）
//
// 对应文档：
//   docs/06.前端桌面端实现(Electron).md §3 C++ 原生扩展设计
#ifndef MP_NATIVE_H
#define MP_NATIVE_H

#include <stdint.h>
#include <stddef.h>

#ifdef _WIN32
  #define MP_API __declspec(dllexport)
#else
  #define MP_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ============== 版本与通用 ============== */

/**
 * 返回库版本字符串（静态字符串，不要 free）。
 * 用于 FFI 加载后的 smoke test，证明 .so/.dylib/.dll 已成功加载。
 */
MP_API const char* mp_version(void);

/**
 * 释放本库返回的堆内存（malloc 分配的）。
 * 禁止用于释放结构体字段 —— 字段请用对应的 release 函数。
 */
MP_API void mp_free(void* ptr);

/* ============== 文件探测（阶段②）============== */

typedef enum {
  MP_FMT_UNKNOWN = 0,
  MP_FMT_MP3,
  MP_FMT_FLAC,
  MP_FMT_M4A,
  MP_FMT_WAV,
  MP_FMT_OGG,
  MP_FMT_MP4,
  MP_FMT_MKV,
  MP_FMT_MOV,
  MP_FMT_WEBM
} MpFormat;

/**
 * 按文件头嗅探格式，比扩展名可靠。
 * 读取前 16 字节 magic。失败返回 MP_FMT_UNKNOWN。
 */
MP_API int mp_probe(const char* file_path);

/* ============== 元数据（阶段②）============== */

typedef struct {
  const char* title;        // UTF-8，可能为 NULL
  const char* artist;       // UTF-8，可能为 NULL
  const char* album;        // UTF-8，可能为 NULL
  int32_t     duration_ms;
  int32_t     bitrate_kbps;
  int32_t     sample_rate;
  int32_t     channels;
} MpMetadata;

/**
 * 读取媒体文件元数据。
 * - 成功返回 0；失败返回负错误码（-1 参数错；-2 文件打不开/非媒体）
 * - 字段内存归本库所有，必须通过 mp_metadata_release 释放
 * - 若编译时未启用 TagLib（MP_WITH_TAGLIB=OFF），始终返回 -3
 */
MP_API int  mp_metadata_read(const char* file_path, MpMetadata* out);
MP_API void mp_metadata_release(MpMetadata* meta);

/* ============== LRC 解析（阶段⑦）============== */

typedef struct {
  int64_t     time_ms;      // [mm:ss.xx] 解析结果
  const char* text;         // UTF-8，可能为空字符串
} MpLrcLine;

/**
 * 解析 LRC 文本（已读入内存的 UTF-8 字节）。
 * - 返回有效行数（n >= 0）
 * - *out_lines 指向堆数组（含 n 个 MpLrcLine）
 * - 行的 text 字段指向数组后紧跟的同一堆块，禁止单独 free
 * - 释放：mp_lrc_release(*out_lines)
 */
MP_API int  mp_lrc_parse(const char* lrc_utf8, size_t len, MpLrcLine** out_lines);
MP_API void mp_lrc_release(MpLrcLine* lines);

/* ============== 系统媒体控制（阶段⑥）============== */

typedef enum {
  MP_SYSMEDIA_PLAY     = 0,
  MP_SYSMEDIA_PAUSE    = 1,
  MP_SYSMEDIA_TOGGLE   = 2,
  MP_SYSMEDIA_NEXT     = 3,
  MP_SYSMEDIA_PREVIOUS = 4,
  MP_SYSMEDIA_SEEK_MS  = 5    // user_data 当 int64_t* 使用
} MpSysMediaAction;

typedef void (*MpMediaCallback)(int32_t action, void* user_data);

typedef struct {
  const char* title;
  const char* artist;
  const char* album;
  int32_t     duration_ms;
  int32_t     position_ms;
  int32_t     playing;        // 0 暂停 / 1 播放
  const char* cover_path;     // 可为 NULL
} MpNowPlaying;

/**
 * 初始化系统媒体控制。
 * - Linux: 创建 MPRIS D-Bus 服务 (org.mpris.MediaPlayer2.MiniPlayer)
 * - macOS: 注册 MPRemoteCommandCenter 回调
 * - Windows: 创建 SMTC 实例
 * - cb 会在非主线程被调，调用方需自行 marshal 回主线程（Node 侧用 setImmediate）
 */
MP_API int  mp_sysmedia_init(MpMediaCallback cb, void* user_data);
MP_API void mp_sysmedia_update(const MpNowPlaying* info);
MP_API void mp_sysmedia_shutdown(void);

#ifdef __cplusplus
}
#endif
#endif  /* MP_NATIVE_H */
