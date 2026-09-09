/* 错误码 + 出参上下文（§07）：C++ 异常体系的 C 翻译
 * 错误码用于跨层传递（解析层发现 → main 统一收口）；
 * 业务常态（GET miss / DEL 不存在）用返回值，不走错误通道 */
#ifndef MKV_ERRORS_H
#define MKV_ERRORS_H
#include <stddef.h>

typedef enum {
    MKV_OK = 0,
    MKV_ERR_SYNTAX,        /* 对应 C++ CmdSyntaxError */
    MKV_ERR_TYPE,          /* 对应 TypeError（预留给 INCR 挑战）*/
    MKV_ERR_IO,            /* 对应 IoError */
    MKV_ERR_AOF_CORRUPT,   /* 对应 AofCorrupted */
    MKV_ERR_MEMORY
} mkv_err_t;

/* 错误上下文：code + 人读消息 + 结构化字段
 * 对应 C++ 异常对象的 what() 和业务字段 */
typedef struct {
    mkv_err_t code;
    char      msg[192];
    size_t    aof_line;    /* 仅 AOF 损坏时填行号 */
} mkv_error_t;

const char *mkv_err_str(mkv_err_t code);
void mkv_err_set(mkv_error_t *err, mkv_err_t code, const char *fmt, ...)
        __attribute__((format(printf, 3, 4)));

#endif
