#ifndef JSON_ERROR_H
#define JSON_ERROR_H

/* ===== 错误码（对应 Java 版 4 个异常类的"分类"）===== */
typedef enum {
    JSON_OK = 0,
    JSON_ERR_PARSE,        /* 对应 JsonParseException */
    JSON_ERR_TYPE,         /* 对应 JsonTypeException */
    JSON_ERR_KEY_MISSING,  /* 对应 JsonKeyMissingException */
    JSON_ERR_MEMORY,       /* malloc 失败（C 特有）*/
    JSON_ERR_IO            /* 文件读写失败（C 特有）*/
} json_err_t;

/* ===== 错误上下文（对应异常对象携带的字段）===== */
typedef struct {
    json_err_t code;
    int line, column;          /* 解析错误用 */
    char msg[256];             /* 人读信息 */
} json_error_t;

const char *json_err_str(json_err_t code);

/* err 可为 NULL（不关心错误详情时）*/
void json_err_set(json_error_t *err, json_err_t code,
                  int line, int column, const char *fmt, ...)
        __attribute__((format(printf, 5, 6)));

/* 便捷宏：一行完成"记错误 + 返回" */
#define RETURN_ERR(e, code, ln, col, ...)              \
    do {                                               \
        json_err_set((e), (code), (ln), (col), __VA_ARGS__); \
        return (code);                                 \
    } while (0)

#endif
