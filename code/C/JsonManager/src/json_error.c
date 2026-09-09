#include "json_error.h"

#include <stdarg.h>
#include <stdio.h>

const char *json_err_str(json_err_t code) {
    switch (code) {
    case JSON_OK:              return "OK";
    case JSON_ERR_PARSE:       return "解析错误";
    case JSON_ERR_TYPE:        return "类型不匹配";
    case JSON_ERR_KEY_MISSING: return "必填字段缺失";
    case JSON_ERR_MEMORY:      return "内存分配失败";
    case JSON_ERR_IO:          return "文件 IO 失败";
    }
    return "unknown";
}

void json_err_set(json_error_t *err, json_err_t code,
                  int line, int column, const char *fmt, ...) {
    if (!err) return;
    err->code = code;
    err->line = line;
    err->column = column;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(err->msg, sizeof err->msg, fmt, ap);
    va_end(ap);
}
