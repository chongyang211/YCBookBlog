/* Value 构造/释放/深拷贝/类型推断（§03）*/
#include "mkv/value.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *mkv_xstrdup(const char *s) {
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

mkv_value_t mkv_null(void)       { return (mkv_value_t){ .type = MKV_NULL }; }
mkv_value_t mkv_bool(bool b)     { return (mkv_value_t){ .type = MKV_BOOL,   .as.b = b }; }
mkv_value_t mkv_int(int64_t i)   { return (mkv_value_t){ .type = MKV_INT,    .as.i = i }; }
mkv_value_t mkv_double(double d) { return (mkv_value_t){ .type = MKV_DOUBLE, .as.d = d }; }
mkv_value_t mkv_string(const char *s) {
    return (mkv_value_t){ .type = MKV_STRING, .as.s = mkv_xstrdup(s) };
}

void mkv_value_free(mkv_value_t *v) {
    if (v->type == MKV_STRING) free(v->as.s);
    v->as.s = NULL;
    v->type = MKV_NULL;                    /* 释放后归位，防 double free */
}

mkv_value_t mkv_value_clone(const mkv_value_t *v) {
    switch (v->type) {
    case MKV_STRING:  return mkv_string(v->as.s);
    case MKV_BOOL:    return mkv_bool(v->as.b);
    case MKV_INT:     return mkv_int(v->as.i);
    case MKV_DOUBLE:  return mkv_double(v->as.d);
    case MKV_NULL:    return mkv_null();
    }
    return mkv_null();
}

void mkv_format(const mkv_value_t *v, char *buf, size_t cap) {
    switch (v->type) {                      /* ⭐ 按标签穷尽分发 */
    case MKV_NULL:   snprintf(buf, cap, "(nil)");                      break;
    case MKV_BOOL:   snprintf(buf, cap, v->as.b ? "true" : "false");   break;
    case MKV_INT:    snprintf(buf, cap, "(integer) %lld",
                              (long long)v->as.i);                     break;
    case MKV_DOUBLE: snprintf(buf, cap, "(double) %g", v->as.d);       break;
    case MKV_STRING: snprintf(buf, cap, "\"%s\"", v->as.s);            break;
    }
}

const char *mkv_type_name(mkv_type_t t) {
    switch (t) {
    case MKV_NULL:   return "nil";
    case MKV_BOOL:   return "bool";
    case MKV_INT:    return "int";
    case MKV_DOUBLE: return "double";
    case MKV_STRING: return "string";
    }
    return "unknown";
}

/* 切片 → NUL 结尾的栈拷贝（strtoll/strtod 只吃 C 字符串）*/
static bool tok_copy(const char *p, size_t len, char *buf, size_t cap) {
    if (len >= cap) return false;
    memcpy(buf, p, len);
    buf[len] = '\0';
    return true;
}

mkv_value_t mkv_value_from_token(const char *p, size_t len) {
    /* 分支 1：特殊字面量 */
    if (len == 4 && strncmp(p, "true", 4) == 0)  return mkv_bool(true);
    if (len == 5 && strncmp(p, "false", 5) == 0) return mkv_bool(false);
    if (len == 3 && strncmp(p, "nil", 3) == 0)   return mkv_null();

    /* 分支 2：全数字（可带负号）→ int64
     * ⭐ 分支顺序不能换：3.14 先被"全数字"拒掉才进 double 分支 */
    size_t start = (len > 0 && p[0] == '-') ? 1 : 0;
    if (start < len) {
        bool all_digit = true;
        for (size_t i = start; i < len; i++)
            if (!isdigit((unsigned char)p[i])) { all_digit = false; break; }
        if (all_digit) {
            char buf[32];
            if (tok_copy(p, len, buf, sizeof buf)) {
                char *end;
                errno = 0;
                long long v = strtoll(buf, &end, 10);
                if (*end == '\0' && errno == 0)      /* ⭐ 溢出查 errno 降级 */
                    return mkv_int((int64_t)v);
            }
        }
    }

    /* 分支 3：含小数点且能解析 → double */
    if (memchr(p, '.', len)) {
        char buf[64];
        if (tok_copy(p, len, buf, sizeof buf)) {
            char *end;
            double d = strtod(buf, &end);
            if (*end == '\0') return mkv_double(d);
        }
    }

    /* 分支 4：兜底字符串 */
    char buf[256];
    if (tok_copy(p, len, buf, sizeof buf)) return mkv_string(buf);
    return mkv_null();
}
