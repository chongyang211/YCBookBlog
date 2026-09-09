/* 命令模式（§05）：enum 解析 + 零拷贝切词 + 基类嵌入的具体命令 + 工厂
 * 多态调用：cmd->execute(cmd, store, out, cap)——传入 set_cmd_t* 但因其
 * 第一成员就是 base，地址相同，向上转型零成本；execute 内部再转回取字段 */
#include "mkv/command.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mkv/entry.h"
#include "mkv/store.h"

char *mkv_xstrdup(const char *s);          /* value.c 提供 */

/* ===== 大小写不敏感比较定长切片（Step 1.2）===== */
static int eq_ignore_case(const char *s, size_t len, const char *upper) {
    if (strlen(upper) != len) return 0;
    for (size_t i = 0; i < len; i++)
        if (toupper((unsigned char)s[i]) != (unsigned char)upper[i]) return 0;
    return 1;
}

enum mkv_cmd_type mkv_parse_cmd_type(const char *s, size_t len) {
    if (eq_ignore_case(s, len, "SET"))    return MKV_CMD_SET;
    if (eq_ignore_case(s, len, "GET"))    return MKV_CMD_GET;
    if (eq_ignore_case(s, len, "DEL"))    return MKV_CMD_DEL;
    if (eq_ignore_case(s, len, "EXPIRE")) return MKV_CMD_EXPIRE;
    if (eq_ignore_case(s, len, "TTL"))    return MKV_CMD_TTL;
    if (eq_ignore_case(s, len, "KEYS"))   return MKV_CMD_KEYS;
    if (eq_ignore_case(s, len, "SAVE"))   return MKV_CMD_SAVE;
    if (eq_ignore_case(s, len, "EXIT"))   return MKV_CMD_EXIT;
    return MKV_CMD_UNKNOWN;
}

/* ===== 零拷贝切词（Step 1.3）：只记位置和长度，全程零 malloc ===== */
size_t mkv_tokenize(const char *line, mkv_tok_t *out, size_t cap) {
    size_t n = 0, i = 0, len = strlen(line);
    while (i < len && n < cap) {
        while (i < len && isspace((unsigned char)line[i])) i++;   /* 跳空白 */
        size_t start = i;
        while (i < len && !isspace((unsigned char)line[i])) i++;  /* 吞 token */
        if (start < i) {
            out[n].p = line + start;      /* ⭐ 零拷贝 */
            out[n].len = i - start;
            n++;
        }
    }
    return n;
}

/* ========== SET ========== */
static mkv_err_t set_cmd_execute(cmd_base_t *self, mkv_store_t *store,
                                 char *out, size_t cap) {
    set_cmd_t *c = (set_cmd_t *)self;           /* ⭐ 向下转型取字段 */
    bool ok = store_set(store, c->key, c->value);
    if (ok)
        c->value = mkv_null();  /* ⭐ 所有权已转移进 store，归位防 double free
                                 *    （失败时所有权仍在，由 command_free 释放）*/
    snprintf(out, cap, ok ? "OK" : "(error) store full");
    return MKV_OK;
}

static char *set_cmd_to_aof(const cmd_base_t *self) {
    const set_cmd_t *c = (const set_cmd_t *)self;
    size_t n = strlen(c->key) + strlen(c->raw_token) + 8;
    char *line = malloc(n);
    if (line) snprintf(line, n, "SET %s %s\n", c->key, c->raw_token);
    return line;
}

static cmd_base_t *set_cmd_new(const char *key, mkv_value_t v, const char *raw) {
    set_cmd_t *c = calloc(1, sizeof *c);
    if (!c) return NULL;
    c->base = (cmd_base_t){ "SET", true, set_cmd_execute, set_cmd_to_aof };
    c->key = mkv_xstrdup(key);
    c->value = v;                              /* value 所有权转移进命令 */
    c->raw_token = mkv_xstrdup(raw);
    return &c->base;
}

/* ========== GET ========== */
static mkv_err_t get_cmd_execute(cmd_base_t *self, mkv_store_t *store,
                                 char *out, size_t cap) {
    get_cmd_t *c = (get_cmd_t *)self;
    mkv_value_t v;
    if (!store_get(store, c->key, &v)) { snprintf(out, cap, "(nil)"); return MKV_OK; }
    mkv_format(&v, out, cap);
    mkv_value_free(&v);
    return MKV_OK;
}

static char *get_cmd_to_aof(const cmd_base_t *self) {
    (void)self;
    return NULL;                               /* 读命令不写 AOF */
}

static cmd_base_t *get_cmd_new(const char *key) {
    get_cmd_t *c = calloc(1, sizeof *c);
    if (!c) return NULL;
    c->base = (cmd_base_t){ "GET", false, get_cmd_execute, get_cmd_to_aof };
    c->key = mkv_xstrdup(key);
    return &c->base;
}

/* ========== DEL ========== */
static mkv_err_t del_cmd_execute(cmd_base_t *self, mkv_store_t *store,
                                 char *out, size_t cap) {
    del_cmd_t *c = (del_cmd_t *)self;
    snprintf(out, cap, "(integer) %d", store_del(store, c->key));
    return MKV_OK;
}

static char *del_cmd_to_aof(const cmd_base_t *self) {
    const del_cmd_t *c = (const del_cmd_t *)self;
    size_t n = strlen(c->key) + 8;
    char *line = malloc(n);
    if (line) snprintf(line, n, "DEL %s\n", c->key);
    return line;
}

static cmd_base_t *del_cmd_new(const char *key) {
    del_cmd_t *c = calloc(1, sizeof *c);
    if (!c) return NULL;
    c->base = (cmd_base_t){ "DEL", true, del_cmd_execute, del_cmd_to_aof };
    c->key = mkv_xstrdup(key);
    return &c->base;
}

/* ========== EXPIRE ========== */
static mkv_err_t expire_cmd_execute(cmd_base_t *self, mkv_store_t *store,
                                    char *out, size_t cap) {
    expire_cmd_t *c = (expire_cmd_t *)self;
    snprintf(out, cap, store_expire(store, c->key, c->seconds) ? "OK"
                                                               : "(integer) 0");
    return MKV_OK;
}

static char *expire_cmd_to_aof(const cmd_base_t *self) {
    const expire_cmd_t *c = (const expire_cmd_t *)self;
    size_t n = strlen(c->key) + 32;
    char *line = malloc(n);
    if (line) snprintf(line, n, "EXPIRE %s %d\n", c->key, c->seconds);
    return line;
}

static cmd_base_t *expire_cmd_new(const char *key, int seconds) {
    expire_cmd_t *c = calloc(1, sizeof *c);
    if (!c) return NULL;
    c->base = (cmd_base_t){ "EXPIRE", true, expire_cmd_execute, expire_cmd_to_aof };
    c->key = mkv_xstrdup(key);
    c->seconds = seconds;
    return &c->base;
}

/* ===== 命令工厂（Step 4.4）：输入边界一次性 parse 成枚举 + 错误码拦截 ===== */
static bool need_args(const mkv_tok_t *t, size_t ntok, size_t want,
                      mkv_error_t *err) {
    if (ntok != want + 1) {
        mkv_err_set(err, MKV_ERR_SYNTAX, "%.*s expects %zu arg(s)",
                    (int)t[0].len, t[0].p, want);
        return false;
    }
    return true;
}

cmd_base_t *mkv_command_parse(const mkv_tok_t *t, size_t ntok, mkv_error_t *err) {
    if (ntok == 0) {
        mkv_err_set(err, MKV_ERR_SYNTAX, "empty command");
        return NULL;
    }
    char key[256], raw[256];

    switch (mkv_parse_cmd_type(t[0].p, t[0].len)) {
    case MKV_CMD_SET:
        if (!need_args(t, ntok, 2, err)) return NULL;
        snprintf(key, sizeof key, "%.*s", (int)t[1].len, t[1].p);
        snprintf(raw, sizeof raw, "%.*s", (int)t[2].len, t[2].p);
        return set_cmd_new(key, mkv_value_from_token(t[2].p, t[2].len), raw);

    case MKV_CMD_GET:
        if (!need_args(t, ntok, 1, err)) return NULL;
        snprintf(key, sizeof key, "%.*s", (int)t[1].len, t[1].p);
        return get_cmd_new(key);

    case MKV_CMD_DEL:
        if (!need_args(t, ntok, 1, err)) return NULL;
        snprintf(key, sizeof key, "%.*s", (int)t[1].len, t[1].p);
        return del_cmd_new(key);

    case MKV_CMD_EXPIRE: {
        if (!need_args(t, ntok, 2, err)) return NULL;
        char sec[32];
        snprintf(key, sizeof key, "%.*s", (int)t[1].len, t[1].p);
        snprintf(sec, sizeof sec, "%.*s", (int)t[2].len, t[2].p);
        char *end;
        long n = strtol(sec, &end, 10);        /* ⭐ 失败不抛异常：查 end */
        if (*end != '\0' || n < 0) {
            mkv_err_set(err, MKV_ERR_SYNTAX, "EXPIRE seconds must be a non-negative integer");
            return NULL;
        }
        return expire_cmd_new(key, (int)n);
    }

    default:
        /* TTL/KEYS/SAVE/EXIT 由 main 直接处理；UNKNOWN 到这里统一报错 */
        mkv_err_set(err, MKV_ERR_SYNTAX, "unknown command '%.*s'",
                    (int)t[0].len, t[0].p);
        return NULL;
    }
}

void mkv_command_free(cmd_base_t *cmd) {
    if (!cmd) return;
    if (strcmp(cmd->name, "SET") == 0) {
        set_cmd_t *c = (set_cmd_t *)cmd;
        free(c->key);
        mkv_value_free(&c->value);
        free(c->raw_token);
    } else {
        free(((get_cmd_t *)cmd)->key);          /* get/del/expire 字段布局同 */
    }
    free(cmd);
}
