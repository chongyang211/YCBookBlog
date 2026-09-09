#ifndef STRBUF_H
#define STRBUF_H

#include <stdbool.h>
#include <stddef.h>

/* 动态字符串缓冲区：char* + len + cap 三件套（对应 Java StringBuilder）
 * 翻倍扩容 → 均摊每次 append O(1) */
typedef struct {
    char   *data;    /* NULL 或 NUL 结尾 */
    size_t  len;     /* 不含结尾 NUL */
    size_t  cap;
} strbuf_t;

void strbuf_init(strbuf_t *sb);
void strbuf_free(strbuf_t *sb);
bool strbuf_append_ch(strbuf_t *sb, char c);
bool strbuf_append(strbuf_t *sb, const char *s);
bool strbuf_append_fmt(strbuf_t *sb, const char *fmt, ...)
        __attribute__((format(printf, 2, 3)));

#endif
