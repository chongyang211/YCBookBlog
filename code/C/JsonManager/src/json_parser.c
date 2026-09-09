/* 递归下降 JSON 解析器：每个文法规则对应一个函数，互相递归调用
 * 兼容 JSON5 子集（教程挑战 A）：// 行注释与块注释、单引号字符串、
 * 尾逗号、不带引号的裸 key —— 读宽松，写仍是标准 JSON（json_writer）*/
#include "json_parser.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strbuf.h"      /* parse_string 拼接用 */

typedef struct {
    const char *src;
    size_t pos, len;
    int line, column;         /* 游标同步维护行列号 */
    json_error_t *err;
} parser_t;

/* ===== 工具方法 ===== */

static void perr(parser_t *p, json_err_t code, const char *fmt, ...) {
    if (!p->err) return;
    p->err->code = code;
    p->err->line = p->line;
    p->err->column = p->column;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(p->err->msg, sizeof p->err->msg, fmt, ap);
    va_end(ap);
}

static char peek(parser_t *p) {
    /* 越界返回 '\0'：数字/关键字扫描自然终止 */
    return p->pos < p->len ? p->src[p->pos] : '\0';
}

static char next(parser_t *p) {
    char c = peek(p);
    if (c == '\0') return c;
    p->pos++;
    if (c == '\n') { p->line++; p->column = 1; }   /* ⭐ 行列同步推进 */
    else           { p->column++; }
    return c;
}

/* 空白 + JSON5 注释（// 行注释、块注释）都跳过 */
static void skip_ws(parser_t *p) {
    while (p->pos < p->len) {
        char c = p->src[p->pos];
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') { next(p); continue; }
        if (c == '/' && p->pos + 1 < p->len && p->src[p->pos + 1] == '/') {
            next(p); next(p);                          /* 行注释 → 跳到行尾 */
            while (p->pos < p->len && peek(p) != '\n') next(p);
            continue;
        }
        if (c == '/' && p->pos + 1 < p->len && p->src[p->pos + 1] == '*') {
            next(p); next(p);                          /* 块注释 → 跳到收尾 */
            while (p->pos < p->len) {
                if (peek(p) == '*' && p->pos + 1 < p->len &&
                    p->src[p->pos + 1] == '/') { next(p); next(p); break; }
                next(p);
            }
            continue;
        }
        break;                                         /* 其余字符不是空白 */
    }
}

static bool match(parser_t *p, char expected) {
    if (peek(p) == expected) { next(p); return true; }
    return false;
}

static bool expect(parser_t *p, char expected) {
    if (match(p, expected)) return true;
    perr(p, JSON_ERR_PARSE, "期望 '%c'，实际 '%s'",
         expected, peek(p) ? (char[2]){peek(p), 0} : "EOF");
    return false;                            /* ⭐ 失败必须让调用方知道 */
}

/* 前向声明：解析函数互相递归调用，先声明后定义 */
static json_value_t *parse_value(parser_t *p);
static json_value_t *parse_string(parser_t *p);
static json_value_t *parse_number(parser_t *p);
static json_value_t *parse_object(parser_t *p);
static json_value_t *parse_array(parser_t *p);
static char *parse_key(parser_t *p);

/* codepoint → UTF-8（1~4 字节，代理对在 parse_string 合成）*/
static bool utf8_encode(strbuf_t *sb, unsigned long cp);
static unsigned long parse_hex4(parser_t *p);

static bool literal(parser_t *p, const char *word) {
    size_t n = strlen(word);
    if (p->pos + n <= p->len && strncmp(p->src + p->pos, word, n) == 0) {
        for (size_t i = 0; i < n; i++) next(p);
        return true;
    }
    return false;
}

static json_value_t *parse_value(parser_t *p) {
    skip_ws(p);
    char c = peek(p);
    switch (c) {
    case '{': return parse_object(p);
    case '[': return parse_array(p);
    case '"': case '\'':                         /* JSON5：单引号也认 */
        return parse_string(p);
    case 't': case 'f':
        if (literal(p, "true"))  return json_bool_new(true);
        if (literal(p, "false")) return json_bool_new(false);
        perr(p, JSON_ERR_PARSE, "期望 true/false");
        return NULL;
    case 'n':
        if (literal(p, "null")) return json_null_new();
        perr(p, JSON_ERR_PARSE, "期望 null");
        return NULL;
    default:
        if (c == '-' || isdigit((unsigned char)c)) return parse_number(p);
        perr(p, JSON_ERR_PARSE, "非法 JSON 起始字符: '%c'(0x%02x)", c, c);
        return NULL;
    }
}

static json_value_t *parse_string(parser_t *p) {
    char open = peek(p);                           /* JSON5：单双引号都认 */
    if (open != '"' && open != '\'') {
        perr(p, JSON_ERR_PARSE, "期望字符串起始引号");
        return NULL;
    }
    next(p);
    strbuf_t sb;
    strbuf_init(&sb);

    while (p->pos < p->len) {
        char c = next(p);
        if (c == open) {                              /* 闭合（与开引号同种）*/
            json_value_t *v = json_string_new(sb.data ? sb.data : "");
            strbuf_free(&sb);
            return v;
        }
        if (c == '\\') {
            char esc = next(p);
            switch (esc) {
            case '"':  strbuf_append_ch(&sb, '"');  break;
            case '\'': strbuf_append_ch(&sb, '\''); break;   /* JSON5 */
            case '\\': strbuf_append_ch(&sb, '\\'); break;
            case '/':  strbuf_append_ch(&sb, '/');  break;
            case 'b':  strbuf_append_ch(&sb, '\b'); break;
            case 'f':  strbuf_append_ch(&sb, '\f'); break;
            case 'n':  strbuf_append_ch(&sb, '\n'); break;
            case 'r':  strbuf_append_ch(&sb, '\r'); break;
            case 't':  strbuf_append_ch(&sb, '\t'); break;
            case 'u': {                                    /* ⭐ \uXXXX + 代理对 */
                unsigned long cp = parse_hex4(p);
                if (cp == 0xFFFFFFFFUL) goto fail;
                if (cp >= 0xD800 && cp <= 0xDBFF) {  /* 高代理：等低代理拼接 */
                    if (p->pos + 1 < p->len &&
                        p->src[p->pos] == '\\' && p->src[p->pos + 1] == 'u') {
                        next(p); next(p);
                        unsigned long lo = parse_hex4(p);
                        if (lo == 0xFFFFFFFFUL) goto fail;
                        if (lo < 0xDC00 || lo > 0xDFFF) {
                            perr(p, JSON_ERR_PARSE, "高代理后不是有效低代理");
                            goto fail;
                        }
                        cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
                    } else {
                        perr(p, JSON_ERR_PARSE, "孤立高代理 \\u%04lX", cp);
                        goto fail;
                    }
                } else if (cp >= 0xDC00 && cp <= 0xDFFF) {
                    perr(p, JSON_ERR_PARSE, "孤立低代理 \\u%04lX", cp);
                    goto fail;
                }
                if (!utf8_encode(&sb, cp)) goto fail;
                break;
            }
            default:
                perr(p, JSON_ERR_PARSE, "非法转义字符: \\%c", esc);
                goto fail;
            }
        } else {
            strbuf_append_ch(&sb, c);                /* UTF-8 原样透传 */
        }
    }
    perr(p, JSON_ERR_PARSE, "字符串未闭合（缺引号）");

fail:                                                 /* ⭐ goto 统一清理 */
    strbuf_free(&sb);
    return NULL;
}

/* 读 4 位 hex，失败返回 0xFFFFFFFF */
static unsigned long parse_hex4(parser_t *p) {
    unsigned long cp = 0;
    for (int i = 0; i < 4; i++) {
        char c = next(p);
        int d;
        if      (c >= '0' && c <= '9') d = c - '0';
        else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') d = c - 'A' + 10;
        else {
            perr(p, JSON_ERR_PARSE, "\\u 后不足 4 位 hex");
            return 0xFFFFFFFFUL;
        }
        cp = cp * 16 + (unsigned long)d;
    }
    return cp;
}

/* key 解析：JSON 允许引号字符串；JSON5 另允许裸标识符 [A-Za-z_$][0-9A-Za-z_$]*
 * 成功返回 malloc 的 key（调用方 free），失败返回 NULL */
static char *parse_key(parser_t *p) {
    if (peek(p) == '"' || peek(p) == '\'') {
        json_value_t *s = parse_string(p);
        if (!s) return NULL;
        char *key = s->as.string_v;          /* 偷走字符串，只释放节点壳 */
        free(s);
        return key;
    }
    strbuf_t sb;
    strbuf_init(&sb);
    char c = peek(p);
    if (!(isalpha((unsigned char)c) || c == '_' || c == '$')) {
        perr(p, JSON_ERR_PARSE, "期望字段名（引号字符串或裸标识符）");
        return NULL;
    }
    while (isalnum((unsigned char)peek(p)) || peek(p) == '_' || peek(p) == '$')
        strbuf_append_ch(&sb, next(p));
    return sb.data;
}

static json_value_t *parse_number(parser_t *p) {
    char buf[64];
    size_t n = 0;
    bool is_float = false;

    if (peek(p) == '-') buf[n++] = next(p);
    while (n < sizeof buf - 1 && isdigit((unsigned char)peek(p)))
        buf[n++] = next(p);
    if (peek(p) == '.') {
        is_float = true;
        buf[n++] = next(p);
        while (n < sizeof buf - 1 && isdigit((unsigned char)peek(p)))
            buf[n++] = next(p);
    }
    if (peek(p) == 'e' || peek(p) == 'E') {
        is_float = true;
        buf[n++] = next(p);
        if (peek(p) == '+' || peek(p) == '-') buf[n++] = next(p);
        while (n < sizeof buf - 1 && isdigit((unsigned char)peek(p)))
            buf[n++] = next(p);
    }
    buf[n] = '\0';

    char *end;
    if (!is_float) {
        long li = strtol(buf, &end, 10);
        if (*end == '\0') return json_int_new(li);   /* ⭐ 整数优先：精确 */
    }
    double d = strtod(buf, &end);
    if (*end == '\0') return json_double_new(d);
    perr(p, JSON_ERR_PARSE, "非法数字: %s", buf);
    return NULL;
}

static json_value_t *parse_object(parser_t *p) {
    if (!expect(p, '{')) return NULL;
    json_value_t *obj = json_object_new();
    if (!obj) return NULL;

    while (true) {
        skip_ws(p);
        if (match(p, '}')) break;                    /* {} 或尾逗号收尾 */
        char *key = parse_key(p);                    /* JSON5：裸 key 也认 */
        if (!key) { json_value_free(obj); return NULL; }
        skip_ws(p);
        if (!expect(p, ':')) {                     /* ⭐ 失败即清理返回 */
            free(key); json_value_free(obj);
            return NULL;
        }
        json_value_t *val = parse_value(p);          /* ⭐ 递归调用 */
        if (!val) {
            free(key); json_value_free(obj);
            return NULL;
        }
        json_object_put(obj, key, val);
        free(key);                                   /* key 拷进表后释放 */
        skip_ws(p);
        if (match(p, ',')) continue;                 /* 尾逗号由循环头兜住 */
        if (match(p, '}')) break;
        perr(p, JSON_ERR_PARSE, "期望 ',' 或 '}'，实际 '%c'", peek(p));
        json_value_free(obj);
        return NULL;
    }
    return obj;
}

static json_value_t *parse_array(parser_t *p) {
    if (!expect(p, '[')) return NULL;
    json_value_t *arr = json_array_new();
    if (!arr) return NULL;

    while (true) {
        skip_ws(p);
        if (match(p, ']')) break;                    /* [] 或尾逗号收尾 */
        json_value_t *item = parse_value(p);         /* ⭐ 递归调用 */
        if (!item) { json_value_free(arr); return NULL; }
        json_array_push(arr, item);
        skip_ws(p);
        if (match(p, ',')) continue;
        if (match(p, ']')) break;
        perr(p, JSON_ERR_PARSE, "期望 ',' 或 ']'，实际 '%c'", peek(p));
        json_value_free(arr);
        return NULL;
    }
    return arr;
}

/* codepoint → UTF-8 编码：C 字符串是字节序列，中文占 3 字节、emoji 占 4 字节 */
static bool utf8_encode(strbuf_t *sb, unsigned long cp) {
    if (cp < 0x80) {                                  /* ASCII：1 字节 */
        return strbuf_append_ch(sb, (char)cp);
    }
    if (cp < 0x800) {                                 /* 2 字节 */
        return strbuf_append_ch(sb, (char)(0xC0 | (cp >> 6)))
            && strbuf_append_ch(sb, (char)(0x80 | (cp & 0x3F)));
    }
    if (cp < 0x10000) {                               /* 3 字节：中文在这 */
        return strbuf_append_ch(sb, (char)(0xE0 | (cp >> 12)))
            && strbuf_append_ch(sb, (char)(0x80 | ((cp >> 6) & 0x3F)))
            && strbuf_append_ch(sb, (char)(0x80 | (cp & 0x3F)));
    }
    if (cp < 0x110000) {                              /* 4 字节：emoji 在这 */
        return strbuf_append_ch(sb, (char)(0xF0 | (cp >> 18)))
            && strbuf_append_ch(sb, (char)(0x80 | ((cp >> 12) & 0x3F)))
            && strbuf_append_ch(sb, (char)(0x80 | ((cp >> 6) & 0x3F)))
            && strbuf_append_ch(sb, (char)(0x80 | (cp & 0x3F)));
    }
    return false;
}

json_value_t *json_parse(const char *src, json_error_t *err) {
    parser_t p = { .src = src, .len = strlen(src),
                   .pos = 0, .line = 1, .column = 1, .err = err };
    if (err) err->code = JSON_OK;

    skip_ws(&p);
    json_value_t *v = parse_value(&p);
    if (!v) return NULL;
    skip_ws(&p);
    if (p.pos != p.len) {
        perr(&p, JSON_ERR_PARSE, "JSON 末尾还有多余字符: '%c'", peek(&p));
        json_value_free(v);
        return NULL;
    }
    return v;
}
