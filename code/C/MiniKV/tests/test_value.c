/* test_value（§03 验收）：类型推断五种全对 + format 输出 */
#include <stdio.h>
#include <string.h>

#include "mkv/value.h"

static int g_fail = 0;

static void expect(const char *token, const char *want) {
    mkv_value_t v = mkv_value_from_token(token, strlen(token));
    char buf[256];
    mkv_format(&v, buf, sizeof buf);
    if (strcmp(buf, want) != 0) {
        printf("FAIL: token=%s got=%s want=%s\n", token, buf, want);
        g_fail++;
    }
    mkv_value_free(&v);
}

int main(void) {
    expect("42",     "(integer) 42");
    expect("-7",     "(integer) -7");
    expect("3.14",   "(double) 3.14");
    expect("true",   "true");
    expect("false",  "false");
    expect("nil",    "(nil)");
    expect("hello",  "\"hello\"");
    expect("42abc",  "\"42abc\"");       /* 不是纯数字落字符串 */

    /* 深拷贝隔离：clone 后 free 原件，clone 仍可用 */
    mkv_value_t a = mkv_string("hello");
    mkv_value_t b = mkv_value_clone(&a);
    mkv_value_free(&a);
    char buf[64];
    mkv_format(&b, buf, sizeof buf);
    if (strcmp(buf, "\"hello\"") != 0) { printf("FAIL: clone\n"); g_fail++; }
    mkv_value_free(&b);

    printf(g_fail ? "test_value: FAIL (%d)\n" : "test_value: PASS\n", g_fail);
    return g_fail ? 1 : 0;
}
