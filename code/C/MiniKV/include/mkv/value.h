/* tagged union Value（§03）：04 案例 json_value_t 的直系后代 */
#ifndef MKV_VALUE_H
#define MKV_VALUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum { MKV_NULL = 0, MKV_BOOL, MKV_INT, MKV_DOUBLE, MKV_STRING } mkv_type_t;

/* tagged union：type 标签 + union 共享内存 */
typedef struct {
    mkv_type_t type;              /* ⭐ 标签：union 当前是什么 */
    union {
        bool     b;
        int64_t  i;
        double   d;
        char    *s;               /* malloc，所有权归本 value */
    } as;
} mkv_value_t;

mkv_value_t mkv_null(void);
mkv_value_t mkv_bool(bool b);
mkv_value_t mkv_int(int64_t i);
mkv_value_t mkv_double(double d);
mkv_value_t mkv_string(const char *s);             /* 内部拷贝 */

void        mkv_value_free(mkv_value_t *v);        /* 释放字符串并归位 */
mkv_value_t mkv_value_clone(const mkv_value_t *v); /* 深拷贝 */

/* Redis 风格输出：(integer) 42 / "hi" / true / (nil) / (double) 3.14 */
void        mkv_format(const mkv_value_t *v, char *buf, size_t cap);
mkv_value_t mkv_value_from_token(const char *p, size_t len);

const char *mkv_type_name(mkv_type_t t);

/* 全项目共享的小工具（value.c 定义，其他模块 extern 引用）*/
char *mkv_xstrdup(const char *s);

#endif
