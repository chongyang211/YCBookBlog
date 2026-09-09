/* REPL 入口（§12 端到端）：REPL → tokenize → 命令分发 → AOF → 收尾
 * 收尾顺序（C 版的"析构顺序"，不能颠倒）：
 *   server_stop（线程不再摸 store）→ aof_close（最后一批 flush）→ store_destroy */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mkv/aof.h"
#include "mkv/command.h"
#include "mkv/errors.h"
#include "mkv/log.h"
#include "mkv/server.h"
#include "mkv/store.h"

static void do_keys(mkv_store_t *store) {
    char **keys = NULL;
    size_t n = store_keys(store, &keys);
    for (size_t i = 0; i < n; i++) {
        printf("%zu) \"%s\"\n", i + 1, keys[i]);
        free(keys[i]);
    }
    free(keys);
}

int main(void) {
    mkv_store_t store;                          /* ① 先建：aof 和 server 都要它 */
    store_init(&store);

    mkv_error_t err = {0};
    aof_report_t rep = aof_replay("data/aof.log", &store, &err, true);
    if (err.code == MKV_ERR_AOF_CORRUPT) {
        fprintf(stderr, "FATAL: %s\n", err.msg);
        return 2;
    }
    if (rep.replayed > 0 || rep.corrupted > 0)
        KV_LOG_INFO("replayed %zu keys, skipped %zu corrupted lines",
                    rep.replayed, rep.corrupted);

    aof_writer_t aof;                           /* ② 再开文件 */
    if (!aof_writer_open(&aof, "data/aof.log")) {
        fprintf(stderr, "FATAL: cannot open data/aof.log\n");
        store_destroy(&store);
        return 3;
    }

    mkv_server_t server;                        /* ③ 最后启动线程 */
    server_start(&server, &store);

    printf("MiniKV v1.0 — type EXIT to quit.\n");
    char line[1024];
    for (;;) {
        printf("> ");
        if (!fgets(line, sizeof line, stdin)) break;   /* Ctrl-D 退出 */
        line[strcspn(line, "\r\n")] = '\0';             /* 掐换行 */
        if (line[0] == '\0') continue;

        mkv_tok_t t[16];
        size_t ntok = mkv_tokenize(line, t, 16);
        if (ntok == 0) continue;                        /* 全空白行 */

        switch (mkv_parse_cmd_type(t[0].p, t[0].len)) {
        case MKV_CMD_EXIT:
            printf("bye.\n");
            goto out;
        case MKV_CMD_SAVE:
            fflush(aof.f);                      /* 数据其实每次写后已 fflush */
            printf("OK\n");
            break;
        case MKV_CMD_KEYS:
            do_keys(&store);
            break;
        case MKV_CMD_TTL: {
            if (ntok != 2) { printf("(syntax) TTL key\n"); break; }
            char key[256];
            snprintf(key, sizeof key, "%.*s", (int)t[1].len, t[1].p);
            printf("(integer) %lld\n", (long long)store_ttl(&store, key));
            break;
        }
        default: {                              /* SET/GET/DEL/EXPIRE 走命令模式 */
            mkv_error_t cerr = {0};
            cmd_base_t *cmd = mkv_command_parse(t, ntok, &cerr);
            if (!cmd) {                         /* 错误命令不击垮服务（§7.3）*/
                switch (cerr.code) {
                case MKV_ERR_SYNTAX: printf("(syntax) %s\n", cerr.msg); break;
                case MKV_ERR_TYPE:   printf("(type) %s\n", cerr.msg);   break;
                default:             printf("(error) %s\n", cerr.msg);  break;
                }
                break;
            }
            char out[256];
            cmd->execute(cmd, &store, out, sizeof out);
            printf("%s\n", out);
            if (cmd->is_write) {                /* 写命令 → 追加 AOF */
                char *aof_line = cmd->to_aof_line(cmd);   /* 命令自己序列化自己 */
                if (aof_line) { aof_append(&aof, aof_line); free(aof_line); }
            }
            mkv_command_free(cmd);
            break;
        }
        }
    }

out:
    server_stop(&server);       /* ⭐ 手动逆序收尾：先停线程 */
    aof_writer_close(&aof);     /*   再关文件 */
    store_destroy(&store);      /*   最后销毁数据 */
    return 0;
}
