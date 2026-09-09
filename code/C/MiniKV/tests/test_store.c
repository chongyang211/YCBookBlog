/* test_store（§06/§11 验收）：SET/GET/DEL/EXPIRE/TTL + purge + 扩容 rehash */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "mkv/store.h"

static int g_fail = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { printf("FAIL: %s\n", msg); g_fail++; } \
} while (0)

int main(void) {
    mkv_store_t s;
    store_init(&s);

    /* 基本 SET/GET */
    store_set(&s, "name", mkv_string("zhangsan"));
    store_set(&s, "age", mkv_int(30));
    mkv_value_t v;
    CHECK(store_get(&s, "name", &v), "get name");
    mkv_value_free(&v);
    CHECK(!store_get(&s, "nokey", &v), "get nokey must miss");

    /* 覆盖 + DEL */
    store_set(&s, "name", mkv_string("lisi"));
    store_get(&s, "name", &v);
    char buf[64];
    mkv_format(&v, buf, sizeof buf);
    CHECK(strcmp(buf, "\"lisi\"") == 0, "overwrite name");
    mkv_value_free(&v);
    CHECK(store_del(&s, "age") == 1, "del age");
    CHECK(store_del(&s, "age") == 0, "del age again = 0（幂等）");
    CHECK(!store_get(&s, "age", &v), "age gone");

    /* EXPIRE / TTL / 惰性删除 / 主动清理 */
    CHECK(store_expire(&s, "name", 1), "expire name 1s");
    CHECK(store_ttl(&s, "name") == 1, "ttl name == 1");
    CHECK(store_ttl(&s, "nokey") == -2, "ttl nokey == -2");
    CHECK(store_ttl(&s, "persist") == -2, "ttl persist nokey");
    sleep(2);
    CHECK(!store_get(&s, "name", &v), "expired name miss（惰性删除）");
    store_purge_expired(&s);                    /* 主动清理 */
    CHECK(store_size(&s) == 0, "purge 后 size == 0");

    /* 扩容 rehash：1000 key 后桶数从 16 涨到 2048，全部可查 */
    char key[16];
    for (int i = 0; i < 1000; i++) {
        snprintf(key, sizeof key, "k%d", i);
        store_set(&s, key, mkv_int(i));
    }
    CHECK(store_size(&s) == 1000, "1000 keys");
    /* 负载因子 ≥2 才翻倍：1000 key → cap 512（1000/512≈1.95 停止扩容）*/
    CHECK(s.map.cap == 512, "rehash growth（16→512，负载因子<2 收手）");
    for (int i = 0; i < 1000; i += 137) {       /* 抽查 */
        snprintf(key, sizeof key, "k%d", i);
        CHECK(store_get(&s, key, &v), "find after rehash");
        if (v.type == MKV_INT && v.as.i != i) { printf("FAIL: k%d wrong\n", i); g_fail++; }
        mkv_value_free(&v);
    }

    /* 删一半：指针的指针接链后无断链 */
    for (int i = 0; i < 1000; i += 2)
        snprintf(key, sizeof key, "k%d", i), store_del(&s, key);
    CHECK(store_size(&s) == 500, "del half = 500");
    for (int i = 1; i < 1000; i += 2) {
        snprintf(key, sizeof key, "k%d", i);
        if (!store_get(&s, key, &v)) { printf("FAIL: broken chain k%d\n", i); g_fail++; break; }
        mkv_value_free(&v);
    }

    store_destroy(&s);
    printf(g_fail ? "test_store: FAIL (%d)\n" : "test_store: PASS\n", g_fail);
    return g_fail ? 1 : 0;
}
