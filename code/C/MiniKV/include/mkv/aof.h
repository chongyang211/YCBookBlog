/* AOF 持久化（§08）：追加日志 + 启动重放
 * 写入格式就是命令本身——重放走和正常运行完全一样的命令管道，
 * 新增命令不用改持久化代码（to_aof_line / command_parse 自动覆盖）*/
#ifndef MKV_AOF_H
#define MKV_AOF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "mkv/command.h"
#include "mkv/errors.h"

typedef struct {
    FILE *f;
    char path[128];
} aof_writer_t;

bool aof_writer_open(aof_writer_t *w, const char *path);
void aof_writer_close(aof_writer_t *w);
bool aof_append(aof_writer_t *w, const char *line);

typedef struct {
    size_t replayed, corrupted, total_lines;
} aof_report_t;

/* tolerant=true：坏行跳过记日志（生产默认，Redis 同款）
 * tolerant=false：坏行中止启动（严格式）*/
aof_report_t aof_replay(const char *path, struct mkv_store *store,
                        mkv_error_t *err, bool tolerant);
#endif
