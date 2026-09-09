/* AOF（§08）：写端 "ab" 追加 + 读端逐行重放（走和正常运行同一套命令管道）*/
#include "mkv/aof.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "mkv/log.h"
#include "mkv/store.h"

bool aof_writer_open(aof_writer_t *w, const char *path) {
    mkdir("data", 0755);              /* POSIX 建目录：已存在返 EEXIST 不算错 */
    errno = 0;
    snprintf(w->path, sizeof w->path, "%s", path);
    w->f = fopen(path, "ab");         /* ⭐ a=append b=二进制（防 Windows \r\n）*/
    return w->f != NULL;
}

void aof_writer_close(aof_writer_t *w) {
    if (w->f) {
        fflush(w->f);
        fclose(w->f);                 /* C 没有 RAII：忘调这行 = fd 泄漏+数据滞留 */
        w->f = NULL;
    }
}

bool aof_append(aof_writer_t *w, const char *line) {
    if (!w->f || fputs(line, w->f) == EOF) return false;
    fflush(w->f);                     /* 推到内核 page cache：进程被 kill 也不丢 */
    return true;
}

/* 读端：fgets → tokenize → command_parse → execute（不回写 AOF）
 * tolerant=true 跳过坏行记日志（生产默认）；false 中止启动（严格式）*/
aof_report_t aof_replay(const char *path, struct mkv_store *store,
                        mkv_error_t *err, bool tolerant) {
    aof_report_t r = {0};
    FILE *in = fopen(path, "rb");        /* 不存在 = 全新启动，不是错误 */
    if (!in) return r;

    char line[1024];
    size_t line_no = 0;
    while (fgets(line, sizeof line, in)) {
        line_no++;
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') continue;
        r.total_lines++;

        mkv_tok_t t[16];
        size_t n = mkv_tokenize(line, t, 16);
        mkv_error_t e = {0};
        cmd_base_t *cmd = mkv_command_parse(t, n, &e);
        if (!cmd) {                      /* 坏行 */
            r.corrupted++;
            if (!tolerant) {             /* 严格式：中止启动 */
                mkv_err_set(err, MKV_ERR_AOF_CORRUPT,
                            "line %zu: %s", line_no, e.msg);
                err->aof_line = line_no;
                fclose(in);
                return r;
            }
            KV_LOG_WARN("AOF skip corrupted line %zu: %s", line_no, e.msg);
            continue;                    /* 宽松式：记日志继续 */
        }
        if (cmd->is_write) {
            char out[256];
            cmd->execute(cmd, store, out, sizeof out);   /* 重放（不回写 AOF）*/
            r.replayed++;
        }
        mkv_command_free(cmd);
    }
    fclose(in);
    return r;
}
