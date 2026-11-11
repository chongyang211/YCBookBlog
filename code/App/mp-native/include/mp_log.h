// mp_log.h —— 跨平台日志（文件 sink + 宿主回调 sink）
//
// 设计：
//   - 文件 sink：追加写入宿主指定路径（路径规则同 mp_kv，由宿主注入）
//   - 回调 sink：宿主注册回调，把日志转发到平台设施
//     （Android → logcat / macOS → OSLog / Windows → OutputDebugString）
//   - 线程安全；回调在**无锁状态**调用，宿主回调里再写日志不会死锁
//
// fmt 使用 printf 风格（跨语言 FFI 无法传 C++ iostream）。
#ifndef MP_LOG_H
#define MP_LOG_H

#include "mp_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 日志级别（数值越小越详细） */
enum {
  MP_LOG_TRACE = 0,
  MP_LOG_DEBUG = 1,
  MP_LOG_INFO  = 2,
  MP_LOG_WARN  = 3,
  MP_LOG_ERROR = 4,
  MP_LOG_OFF   = 5,   /* 关闭全部输出 */
};

/**
 * 初始化文件 sink。
 * @param file_path 日志文件路径（追加模式）；传 NULL = 不写文件（仅回调 sink）
 * @param level     全局级别过滤
 * @return MP_OK / MP_ERR_IO（文件打不开）/ MP_ERR_ARGS
 */
MP_API int32_t mp_log_init(const char* file_path, int32_t level);

/** 动态调整级别（运行时切换 DEBUG/ERROR 等） */
MP_API int32_t mp_log_set_level(int32_t level);

/**
 * 追加宿主回调 sink（可多次调用，多 sink 并存；全部按注册顺序回调）。
 * 回调签名：void cb(int32_t level, const char* tag, const char* message, void* user)
 * 注意：message 生命周期仅限回调内；需要保存请自行拷贝。
 */
typedef void (*mp_log_sink_cb)(int32_t level, const char* tag,
                               const char* message, void* user_data);
MP_API int32_t mp_log_add_sink(mp_log_sink_cb cb, void* user_data);

/** printf 风格写日志。tag 可为 NULL（输出 "-"）。低于当前级别直接丢弃。 */
MP_API int32_t mp_log_write(int32_t level, const char* tag,
                            const char* fmt, ...);

/** 关闭文件 sink 并清空全部回调（进程退出前调用） */
MP_API void mp_log_shutdown(void);

#ifdef __cplusplus
}
#endif
#endif  /* MP_LOG_H */
