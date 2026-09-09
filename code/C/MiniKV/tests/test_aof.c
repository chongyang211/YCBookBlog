/* test_aof（§08 验收）：写 → 重放 → 数据回来；半行损坏宽松式跳过 */
#include <stdio.h>
#include <string.h>

#include "mkv/aof.h"
#include "mkv/errors.h"
#include "mkv/store.h"

static int g_fail = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { printf("FAIL: %s\n", msg); g_fail++; } \
} while (0)

int main(void) {
    remove("data/test_aof.log");

    /* 1. 追加写两条 */
    aof_writer_t w;
    CHECK(aof_writer_open(&w, "data/test_aof.log"), "open aof");
    aof_append(&w, "SET hello world\n");
    aof_append(&w, "SET num 42\n");
    aof_writer_close(&w);

    /* 2. 重放 */
    mkv_store_t s;
    store_init(&s);
    mkv_error_t err = {0};
    aof_report_t rep = aof_replay("data/test_aof.log", &s, &err, true);
    CHECK(rep.replayed == 2, "replayed 2");
    CHECK(rep.corrupted == 0, "no corrupted");

    mkv_value_t v;
    char buf[64];
    CHECK(store_get(&s, "hello", &v), "get hello");
    mkv_format(&v, buf, sizeof buf);
    CHECK(strcmp(buf, "\"world\"") == 0, "hello == world");
    mkv_value_free(&v);
    CHECK(store_get(&s, "num", &v), "get num");
    mkv_format(&v, buf, sizeof buf);
    CHECK(strcmp(buf, "(integer) 42") == 0, "num == 42");
    mkv_value_free(&v);
    store_destroy(&s);

    /* 3. 半行损坏：宽松式跳过且损失只有那一行 */
    FILE *f = fopen("data/test_aof.log", "ab");
    fputs("SET\n", f);                 /* 模拟断电半行 */
    fclose(f);

    store_init(&s);
    err = (mkv_error_t){0};
    rep = aof_replay("data/test_aof.log", &s, &err, true);
    CHECK(rep.replayed == 2, "still replayed 2");
    CHECK(rep.corrupted == 1, "skipped 1 corrupted");
    CHECK(store_get(&s, "hello", &v), "hello survives");
    mkv_value_free(&v);

    /* 4. 严格式：报 AOF_CORRUPT + 行号 */
    store_destroy(&s);
    store_init(&s);
    err = (mkv_error_t){0};
    aof_replay("data/test_aof.log", &s, &err, false);
    CHECK(err.code == MKV_ERR_AOF_CORRUPT, "strict reports corrupt");
    CHECK(err.aof_line == 3, "corrupt at line 3");
    store_destroy(&s);

    remove("data/test_aof.log");
    printf(g_fail ? "test_aof: FAIL (%d)\n" : "test_aof: PASS\n", g_fail);
    return g_fail ? 1 : 0;
}
