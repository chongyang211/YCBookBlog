#include "strbuf.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void strbuf_init(strbuf_t *sb) { sb->data = NULL; sb->len = sb->cap = 0; }

void strbuf_free(strbuf_t *sb) { free(sb->data); strbuf_init(sb); }

static bool sb_reserve(strbuf_t *sb, size_t extra) {
    if (sb->len + extra + 1 <= sb->cap) return true;
    size_t nc = sb->cap ? sb->cap : 16;
    while (nc < sb->len + extra + 1) nc *= 2;        /* ⭐ 翻倍扩容 */
    char *nd = realloc(sb->data, nc);
    if (!nd) return false;
    sb->data = nd;
    sb->cap = nc;
    return true;
}

bool strbuf_append_ch(strbuf_t *sb, char c) {
    if (!sb_reserve(sb, 1)) return false;
    sb->data[sb->len++] = c;
    sb->data[sb->len] = '\0';
    return true;
}

bool strbuf_append(strbuf_t *sb, const char *s) {
    size_t n = strlen(s);
    if (!sb_reserve(sb, n)) return false;
    memcpy(sb->data + sb->len, s, n + 1);            /* 连结尾 NUL 一起拷 */
    sb->len += n;
    return true;
}

bool strbuf_append_fmt(strbuf_t *sb, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int need = vsnprintf(NULL, 0, fmt, ap);          /* 第一遍：算长度 */
    va_end(ap);
    if (need < 0 || !sb_reserve(sb, (size_t)need)) return false;
    va_start(ap, fmt);
    vsnprintf(sb->data + sb->len, (size_t)need + 1, fmt, ap);
    va_end(ap);
    sb->len += (size_t)need;
    return true;
}
