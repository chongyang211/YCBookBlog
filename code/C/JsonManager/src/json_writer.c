/* 序列化：json_value_t 树 → JSON 字符串（compact / pretty 复用一套代码）*/
#include "json_writer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strbuf.h"

static void write_indent(strbuf_t *sb, int level) {
    for (int i = 0; i < level; i++) strbuf_append(sb, "  ");
}

void json_escape_write(strbuf_t *sb, const char *s) {
    for (const unsigned char *q = (const unsigned char *)s; *q; q++) {
        switch (*q) {
        case '"':  strbuf_append(sb, "\\\""); break;
        case '\\': strbuf_append(sb, "\\\\"); break;
        case '\b': strbuf_append(sb, "\\b");  break;
        case '\f': strbuf_append(sb, "\\f");  break;
        case '\n': strbuf_append(sb, "\\n");  break;
        case '\r': strbuf_append(sb, "\\r");  break;
        case '\t': strbuf_append(sb, "\\t");  break;
        default:
            if (*q < 0x20) strbuf_append_fmt(sb, "\\u%04x", *q);  /* 控制字符 */
            else           strbuf_append_ch(sb, (char)*q);          /* UTF-8 透传 */
        }
    }
}

static void write_value(strbuf_t *sb, const json_value_t *v,
                        int indent, bool pretty) {
    switch (v->type) {                                /* ⭐ 按标签穷尽分发 */
    case JSON_NULL:   strbuf_append(sb, "null"); break;
    case JSON_BOOL:   strbuf_append(sb, v->as.bool_v ? "true" : "false"); break;
    case JSON_NUMBER:
        if (v->as.number_v.is_int)
            strbuf_append_fmt(sb, "%ld", v->as.number_v.v.i);
        else
            strbuf_append_fmt(sb, "%g", v->as.number_v.v.d);
        break;
    case JSON_STRING:
        strbuf_append_ch(sb, '"');
        json_escape_write(sb, v->as.string_v);
        strbuf_append_ch(sb, '"');
        break;
    case JSON_ARRAY: {
        json_array_t *a = v->as.array_v;
        if (a->len == 0) { strbuf_append(sb, "[]"); break; }
        strbuf_append_ch(sb, '[');
        if (pretty) strbuf_append_ch(sb, '\n');
        for (size_t i = 0; i < a->len; i++) {
            if (pretty) write_indent(sb, indent + 1);
            write_value(sb, a->items[i], indent + 1, pretty);
            if (i + 1 < a->len) strbuf_append_ch(sb, ',');
            if (pretty) strbuf_append_ch(sb, '\n');
        }
        if (pretty) write_indent(sb, indent);
        strbuf_append_ch(sb, ']');
        break;
    }
    case JSON_OBJECT: {
        json_object_t *o = v->as.object_v;
        if (o->len == 0) { strbuf_append(sb, "{}"); break; }
        strbuf_append_ch(sb, '{');
        if (pretty) strbuf_append_ch(sb, '\n');
        for (size_t i = 0; i < o->len; i++) {
            if (pretty) write_indent(sb, indent + 1);
            strbuf_append_ch(sb, '"');
            json_escape_write(sb, o->keys[i]);
            strbuf_append(sb, pretty ? "\": " : "\":");
            write_value(sb, o->values[i], indent + 1, pretty);
            if (i + 1 < o->len) strbuf_append_ch(sb, ',');
            if (pretty) strbuf_append_ch(sb, '\n');
        }
        if (pretty) write_indent(sb, indent);
        strbuf_append_ch(sb, '}');
        break;
    }
    }
}

char *json_write(const json_value_t *v, bool pretty) {
    strbuf_t sb;
    strbuf_init(&sb);
    if (!sb.data) strbuf_append(&sb, "");            /* 空树兜底 */
    write_value(&sb, v, 0, pretty);
    return sb.data;                                   /* 所有权移交调用方 */
}
