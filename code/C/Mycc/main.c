/* mycc 入口（§09 最终版）：REPL / 文件模式 + setjmp 顶层收口
 * 静态清理锚点：longjmp 跳过正常释放路径时，按推进程度统一回收 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "codegen.h"
#include "errors.h"
#include "lexer.h"
#include "parser.h"
#include "type_check.h"
#include "vm.h"

enum { MODE_RUN, MODE_DUMP, MODE_TCHECK };

/* longjmp 清理锚点（REPL 多轮执行下不泄漏）*/
static AstNode *s_prog;
static Token   *s_toks;
static int      s_toks_count;
static Codegen *s_cg;
static VM      *s_vm;

static void anchors_reset(void) {
    s_prog = NULL; s_toks = NULL; s_toks_count = 0; s_cg = NULL; s_vm = NULL;
}

static void anchors_cleanup(void) {
    if (s_vm)  { vm_free(s_vm);      free(s_vm);  }
    if (s_cg)  { codegen_free(s_cg); free(s_cg);  }
    if (s_prog) ast_free(s_prog);
    if (s_toks) lexer_free_tokens(s_toks, s_toks_count);
}

/* ===== 一次完整编译 + 执行（mode 决定止步于哪一段）===== */
static int compile_and_run(const char *src, const char *filename, int mode) {
    error_reset();
    strncpy(g_err.file, filename, sizeof g_err.file - 1);
    g_err.file[sizeof g_err.file - 1] = '\0';
    anchors_reset();

    g_err.jmp_armed = 1;
    if (setjmp(g_err.jmp) != 0) {
        /* 任何阶段 longjmp 到这里——诊断已打印，按锚点清理 */
        g_err.jmp_armed = 0;
        anchors_cleanup();
        return error_exit_code(g_err.kind);
    }

    /* ① Lexer */
    Lexer lex;
    lexer_init(&lex, src);
    s_toks = lexer_tokenize(&lex, &s_toks_count);

    /* ② Parser */
    Parser psr;
    parser_init(&psr, s_toks, s_toks_count);
    s_prog = parser_parse_program(&psr);

    /* ③ TypeChecker（累积错误不跳出）*/
    TypeEnv env;
    type_env_init(&env);
    type_check(s_prog, &env);
    if (env.has_error) {
        g_err.jmp_armed = 0;
        anchors_cleanup();
        return 3;
    }
    if (mode == MODE_TCHECK) {
        printf("[TypeCheck] OK\n");
        g_err.jmp_armed = 0;
        anchors_cleanup();
        return 0;
    }

    /* ④ Codegen */
    s_cg = (Codegen *)malloc(sizeof(Codegen));
    if (!s_cg) mycc_error(ERR_IO, 0, NULL, "out of memory");
    codegen_init(s_cg);
    codegen_program(s_cg, s_prog);

    if (mode == MODE_DUMP) {
        for (int i = 0; i < s_cg->fn_count; ++i) {
            chunk_disassemble(&s_cg->fns[i].chunk, stdout);
            fputc('\n', stdout);
        }
        g_err.jmp_armed = 0;
        anchors_cleanup();
        return 0;
    }

    /* ⑤ VM */
    s_vm = (VM *)malloc(sizeof(VM));
    if (!s_vm) mycc_error(ERR_IO, 0, NULL, "out of memory");
    vm_init(s_vm);
    vm_load(s_vm, s_cg->fns, s_cg->fn_count);
    vm_run(s_vm);

    g_err.jmp_armed = 0;
    anchors_cleanup();
    return 0;
}

/* ===== 文件模式 ===== */
static int run_file(const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "[IO] cannot open file '%s'\n", path);
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (size < 0 || size > 1 << 20) {
        fclose(fp);
        fprintf(stderr, "[IO] file too large or read error\n");
        return 1;
    }
    char *buf = (char *)malloc((size_t)size + 1);
    if (!buf) { fclose(fp); return 1; }
    fread(buf, 1, (size_t)size, fp);
    buf[size] = '\0';
    fclose(fp);

    int code = compile_and_run(buf, path, MODE_RUN);
    free(buf);
    return code;
}

/* ===== REPL 模式（单行制；多行程序请用文件模式）===== */
static int run_repl(void) {
    char line[1024];
    printf("mycc 1.0 (C version) — :h 查看帮助，:q 退出\n");
    while (1) {
        fputs("mycc> ", stdout);
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;
        size_t len = strlen(line);
        if (len && line[len - 1] == '\n') line[--len] = '\0';
        if (len == 0) continue;

        if (strcmp(line, ":q") == 0) break;
        if (strcmp(line, ":h") == 0) {
            puts("  :run  <code>    编译并执行（默认：直接输入源码）");
            puts("  :dump <code>    反汇编字节码");
            puts("  :tcheck <code>  仅类型检查");
            puts("  :q              退出");
            continue;
        }

        int mode = MODE_RUN;
        const char *code = line;
        if (strncmp(line, ":run ", 5) == 0)         code = line + 5;
        else if (strncmp(line, ":dump ", 6) == 0)  { code = line + 6; mode = MODE_DUMP; }
        else if (strncmp(line, ":tcheck ", 8) == 0){ code = line + 8; mode = MODE_TCHECK; }

        /* REPL 里错误被 setjmp 接住，不会退出循环 */
        compile_and_run(code, "<repl>", mode);
    }
    printf("再见！\n");
    return 0;
}

int main(int argc, char **argv) {
    if (argc == 1) return run_repl();
    if (argc == 2) return run_file(argv[1]);
    fprintf(stderr, "Usage: %s [file.mycc]\n", argv[0]);
    return 1;
}
