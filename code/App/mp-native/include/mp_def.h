// mp_def.h —— 公共定义：导出宏 + 统一错误码
//
// 所有模块头文件（mp_native.h / mp_kv.h / mp_log.h）都包含本文件。
// 错误码约定：0 = 成功；负数 = 失败；各模块可共用，语义见各函数注释。
#ifndef MP_DEF_H
#define MP_DEF_H

#include <stdint.h>
#include <stddef.h>

#ifdef _WIN32
  #ifdef MP_NATIVE_EXPORTS        /* CMake 构建库本身时定义 */
    #define MP_API __declspec(dllexport)
  #else
    #define MP_API __declspec(dllimport)   /* Windows 下消费方编译头文件时 */
  #endif
#else
  #define MP_API __attribute__((visibility("default")))
#endif

/* ---- 统一错误码 ---- */
enum {
  MP_OK           =  0,   /* 成功 */
  MP_ERR_ARGS     = -1,   /* 参数非法（NULL / 超长 / 空 key 等） */
  MP_ERR_IO       = -2,   /* 文件读写失败 */
  MP_ERR_NOT_FOUND= -3,   /* key 不存在 / 文件不存在 */
  MP_ERR_STATE    = -4,   /* 状态错误（如重复打开同一 KV 目录） */
  MP_ERR_OOM      = -5,   /* 内存分配失败 */
};

#ifdef __cplusplus
extern "C" {
#endif

/* 错误码 → 人类可读描述（静态字符串，不要 free） */
MP_API const char* mp_error_string(int32_t code);

#ifdef __cplusplus
}
#endif
#endif  /* MP_DEF_H */
