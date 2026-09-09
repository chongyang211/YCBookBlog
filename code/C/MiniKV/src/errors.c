#include "mkv/errors.h"

#include <stdarg.h>
#include <stdio.h>

const char *mkv_err_str(mkv_err_t code) {
    switch (code) {
    case MKV_OK:              return "OK";
    case MKV_ERR_SYNTAX:      return "命令语法错误";
    case MKV_ERR_TYPE:        return "类型不匹配";
    case MKV_ERR_IO:          return "IO 错误";
    case MKV_ERR_AOF_CORRUPT: return "AOF 损坏";
    case MKV_ERR_MEMORY:      return "内存分配失败";
    }
    return "unknown";
}

void mkv_err_set(mkv_error_t *err, mkv_err_t code, const char *fmt, ...) {
    if (!err) return;
    err->code = code;
    err->aof_line = 0;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(err->msg, sizeof err->msg, fmt, ap);
    va_end(ap);
}
