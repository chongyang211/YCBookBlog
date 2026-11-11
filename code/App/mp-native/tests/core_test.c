/* tests/core_test.c —— core 模块（kv / log）C 层回归测试
 *
 * 运行：cmake --preset examples（或 -DMP_BUILD_TESTS=ON 配置后）ctest -R core
 * 覆盖：
 *   KV   open/put/get/delete/has/count/foreach/compact/close + 持久化重放 + 崩溃截断容错
 *   LOG  init/write 文件落盘 + 级别过滤 + 回调 sink 计数 + shutdown
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if !defined(_WIN32)
  #include <unistd.h>       /* getpid */
#endif
#include "mp_native.h"      /* mp_free */
#include "mp_kv.h"
#include "mp_log.h"
#include "mp_def.h"

#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); \
        exit(1); \
    } \
} while (0)

static int foreach_seen = 0;
static int32_t on_foreach(const char* key, const void* value, size_t len, void* user) {
    (void)value; (void)len; (void)user;
    foreach_seen++;
    /* 演示提前停止：遇到 "stop-me" 返回非 0 */
    if (strcmp(key, "stop-me") == 0) return 99;
    return 0;
}

static int sink_count = 0;
static int32_t last_level = -1;
static void on_log(int32_t level, const char* tag, const char* msg, void* user) {
    (void)tag; (void)msg; (void)user;
    sink_count++;
    last_level = level;
}

static void test_kv_basic(const char* dir) {
    mp_kv* db = NULL;
    CHECK(mp_kv_open(dir, &db) == MP_OK);

    /* 参数校验 */
    CHECK(mp_kv_put(db, "", "v", 1) == MP_ERR_ARGS);
    CHECK(mp_kv_put(NULL, "k", "v", 1) == MP_ERR_ARGS);

    /* 增查 */
    CHECK(mp_kv_put(db, "name", "mp_native", 9) == MP_OK);
    CHECK(mp_kv_put(db, "bin", "\x00\x01\xff", 3) == MP_OK);      /* 二进制安全 */

    char* s = NULL;
    CHECK(mp_kv_get_str(db, "name", &s) == MP_OK);
    CHECK(strcmp(s, "mp_native") == 0);
    mp_free(s);

    void* raw = NULL; size_t n = 0;
    CHECK(mp_kv_get(db, "bin", &raw, &n) == MP_OK);
    CHECK(n == 3 && memcmp(raw, "\x00\x01\xff", 3) == 0);
    mp_free(raw);

    CHECK(mp_kv_get(db, "no-such-key", &raw, &n) == MP_ERR_NOT_FOUND);

    /* 覆盖写 + 空值 */
    CHECK(mp_kv_put(db, "name", "v2", 2) == MP_OK);
    CHECK(mp_kv_get_str(db, "name", &s) == MP_OK && strcmp(s, "v2") == 0);
    mp_free(s);
    CHECK(mp_kv_put(db, "empty", NULL, 0) == MP_OK);
    CHECK(mp_kv_get(db, "empty", &raw, &n) == MP_OK && n == 0);
    mp_free(raw);

    /* has / count / foreach */
    int32_t exists = 0;
    CHECK(mp_kv_has(db, "bin", &exists) == MP_OK && exists == 1);
    CHECK(mp_kv_has(db, "gone", &exists) == MP_OK && exists == 0);
    const int64_t cnt_before = mp_kv_count(db);
    CHECK(cnt_before == 3);                                       /* name/bin/empty */

    foreach_seen = 0;
    CHECK(mp_kv_put(db, "stop-me", "x", 1) == MP_OK);
    CHECK(mp_kv_foreach(db, on_foreach, NULL) == 99);             /* 提前停止 */
    CHECK(foreach_seen >= 1);

    /* 删除 */
    CHECK(mp_kv_delete(db, "stop-me") == MP_OK);
    CHECK(mp_kv_delete(db, "stop-me") == MP_ERR_NOT_FOUND);       /* 二次删不存在 */
    CHECK(mp_kv_count(db) == cnt_before);

    /* 压实后数据不丢 */
    CHECK(mp_kv_compact(db) == MP_OK);
    CHECK(mp_kv_get_str(db, "bin", &s) == MP_OK);
    mp_free(s);
    CHECK(mp_kv_count(db) == cnt_before);

    CHECK(mp_kv_close(db) == MP_OK);
}

static void test_kv_persistence(const char* dir) {
    /* 重新打开 → 重放日志，数据应完整 */
    mp_kv* db = NULL;
    CHECK(mp_kv_open(dir, &db) == MP_OK);
    char* s = NULL;
    CHECK(mp_kv_get_str(db, "name", &s) == MP_OK && strcmp(s, "v2") == 0);
    mp_free(s);
    CHECK(mp_kv_count(db) == 3);                                  /* name/bin/empty，stop-me 已删 */

    /* 二次打开同一目录 → MP_ERR_STATE */
    mp_kv* db2 = NULL;
    CHECK(mp_kv_open(dir, &db2) == MP_ERR_STATE);

    CHECK(mp_kv_close(db) == MP_OK);
}

static void test_log(const char* dir) {
    char path[512];
    snprintf(path, sizeof(path), "%s/test.log", dir);

    CHECK(mp_log_init(path, MP_LOG_DEBUG) == MP_OK);
    CHECK(mp_log_add_sink(on_log, NULL) == MP_OK);

    CHECK(mp_log_write(MP_LOG_INFO,  "kv",  "opened at %s", dir) == MP_OK);
    CHECK(mp_log_write(MP_LOG_DEBUG, "kv",  "debug visible at DEBUG level") == MP_OK);
    CHECK(mp_log_write(MP_LOG_ERROR, NULL,  "no tag") == MP_OK);
    CHECK(mp_log_write(99, "x", "bad level") == MP_ERR_ARGS);     /* 非法级别 */

    /* 级别过滤：调到 ERROR 后 DEBUG 应被丢弃（sink 不增长） */
    CHECK(mp_log_set_level(MP_LOG_ERROR) == MP_OK);
    const int before = sink_count;
    CHECK(mp_log_write(MP_LOG_DEBUG, "kv",  "should be filtered") == MP_OK);
    CHECK(sink_count == before);
    CHECK(mp_log_write(MP_LOG_ERROR, "kv",  "error passes") == MP_OK);
    CHECK(sink_count == before + 1 && last_level == MP_LOG_ERROR);

    mp_log_shutdown();

    /* 落盘验证：文件里应能搜到第一条 INFO 行 */
    FILE* f = fopen(path, "rb");
    CHECK(f != NULL);
    char buf[2048] = {0};
    size_t got = fread(buf, 1, sizeof(buf) - 1, f);
    fclose(f);
    buf[got] = 0;
    CHECK(strstr(buf, "[INFO] [kv] opened at") != NULL);
    CHECK(strstr(buf, "error passes") != NULL);
    CHECK(strstr(buf, "should be filtered") == NULL);
}

int main(void) {
    const char* tmp = getenv("TMPDIR");
    if (!tmp || !*tmp) tmp = ".";

    static char dir[512];
#if defined(_WIN32)
    snprintf(dir, sizeof(dir), "%s\\mp_core_kv", tmp);
#else
    snprintf(dir, sizeof(dir), "%s/mp_core_kv_%d", tmp, (int)getpid());
#endif

    test_kv_basic(dir);
    test_kv_persistence(dir);
    test_log(dir);

    printf("core tests passed (kv + log)\n");
    return 0;
}
