/* 命令模式（§05）：基类嵌入 + 函数指针 = C 的虚函数
 * cmd_base_t 必须是每个命令结构体的第一个成员（向上转型零成本）；
 * mkv_store_t 前置声明打破 command ↔ store 循环 include */
#ifndef MKV_COMMAND_H
#define MKV_COMMAND_H

#include <stdbool.h>
#include <stddef.h>

#include "mkv/errors.h"
#include "mkv/value.h"

/* 零拷贝切片（std::string_view 的 C 版）：只记位置和长度，不 malloc */
typedef struct {
    const char *p;
    size_t      len;
} mkv_tok_t;

enum mkv_cmd_type {
    MKV_CMD_SET, MKV_CMD_GET, MKV_CMD_DEL, MKV_CMD_EXPIRE,
    MKV_CMD_TTL, MKV_CMD_KEYS, MKV_CMD_SAVE, MKV_CMD_EXIT,
    MKV_CMD_UNKNOWN
};

enum mkv_cmd_type mkv_parse_cmd_type(const char *s, size_t len);
size_t mkv_tokenize(const char *line, mkv_tok_t *out, size_t cap);

typedef struct mkv_store mkv_store_t;   /* ⭐ 前置声明：接口只用指针 */

typedef struct cmd_base {
    const char *name;                                    /* "SET" */
    bool is_write;
    mkv_err_t (*execute)(struct cmd_base *self, mkv_store_t *store,
                         char *out, size_t out_cap);
    char *(*to_aof_line)(const struct cmd_base *self);
} cmd_base_t;

typedef struct { cmd_base_t base; char *key; mkv_value_t value; char *raw_token; } set_cmd_t;
typedef struct { cmd_base_t base; char *key; }                                 get_cmd_t;
typedef struct { cmd_base_t base; char *key; }                                 del_cmd_t;
typedef struct { cmd_base_t base; char *key; int seconds; }                    expire_cmd_t;

cmd_base_t *mkv_command_parse(const mkv_tok_t *t, size_t ntok, mkv_error_t *err);
void        mkv_command_free(cmd_base_t *cmd);

#endif
