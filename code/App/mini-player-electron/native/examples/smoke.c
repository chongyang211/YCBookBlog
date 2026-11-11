/* examples/smoke.c —— 纯 C 消费示例
 *
 * 目的：证明 mp_native 的 C ABI 与语言无关 —— 不经过 Node/koffi，
 *      任何 C/C++/Rust/Go/Python 的 FFI 消费方都能按同样的约定调用。
 *
 * 构建（配合 CMakePresets）：
 *   cmake --preset examples && cmake --build --preset examples
 *   ./build/mp_smoke [可选: 探测的文件路径]
 *   ctest --preset smoke
 *
 * 三个演示点（对应文档 §04.3 C ABI 三原则）：
 *   1. mp_version()        —— 静态字符串，不释放（规则一：所有权显式）
 *   2. mp_probe()          —— 枚举返回
 *   3. mp_lrc_parse()      —— 变长数组 + 单块释放（规则二：三件套）
 */
#include <stdio.h>
#include <string.h>
#include "mp_native.h"

static const char* fmt_name(int code) {
    static const char* names[] = {
        "unknown", "mp3", "flac", "m4a", "wav", "ogg",
        "mp4", "mkv", "mov", "webm"
    };
    return (code >= 0 && code <= 9) ? names[code] : "?";
}

int main(int argc, char** argv) {
    /* ---- 1. 版本：静态字符串，调用方绝不 free ---- */
    printf("mp_native version : %s\n", mp_version());

    /* ---- 2. 文件头嗅探 ---- */
    if (argc > 1) {
        int code = mp_probe(argv[1]);
        printf("probe('%s')       = %d (%s)\n",
               argv[1], code, fmt_name(code));
    }

    /* ---- 3. LRC：变长数组从 C 边界取出，整块释放 ---- */
    const char* demo_lrc =
        "[ti:Demo]\n"
        "[00:01.00]first line\n"
        "[00:02.50][00:08.00]repeated line\n"
        "[00:04.25]last line\n";

    MpLrcLine* lines = NULL;
    int n = mp_lrc_parse(demo_lrc, strlen(demo_lrc), &lines);
    printf("lrc lines         = %d\n", n);
    for (int i = 0; i < n; ++i) {
        printf("  [%6lld ms] %s\n",
               (long long)lines[i].time_ms, lines[i].text);
    }
    mp_lrc_release(lines);            /* ⭐ 单块释放，绝不逐行 free */
    lines = NULL;

    printf("OK\n");
    return 0;
}
