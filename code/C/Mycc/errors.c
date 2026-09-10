#include "errors.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

ErrorState g_err = { ERR_NONE, 0, "", "<repl>", {0}, 0 };

static const char *names[] = {
    [ERR_NONE]    = "OK",
    [ERR_LEX]     = "Lex",
    [ERR_PARSE]   = "Parse",
    [ERR_TYPE]    = "Type",
    [ERR_RUNTIME] = "Runtime",
    [ERR_IO]      = "IO",
};

const char *error_stage_name(ErrorKind k) { return names[k]; }

int error_exit_code(ErrorKind k) {
    switch (k) {
    case ERR_NONE:    return 0;
    case ERR_LEX:
    case ERR_PARSE:   return 2;
    case ERR_TYPE:    return 3;
    case ERR_RUNTIME: return 4;
    case ERR_IO:      return 1;
    }
    return 99;
}

void error_reset(void) {
    g_err.kind = ERR_NONE;
    g_err.line = 0;
    g_err.message[0] = '\0';
}

static void err_fill(ErrorKind k, int line, const char *file,
                     const char *fmt, va_list ap) {
    g_err.kind = k;
    g_err.line = line;
    if (file) {
        strncpy(g_err.file, file, sizeof g_err.file - 1);
        g_err.file[sizeof g_err.file - 1] = '\0';
    }
    vsnprintf(g_err.message, sizeof g_err.message, fmt, ap);

    fprintf(stderr, "[%s] ", names[k]);
    if (strcmp(g_err.file, "<repl>") != 0) fprintf(stderr, "%s:", g_err.file);
    if (line > 0) fprintf(stderr, "line %d: ", line);
    fprintf(stderr, "%s\n", g_err.message);
}

void mycc_error(ErrorKind k, int line, const char *file, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    err_fill(k, line, file, fmt, ap);
    va_end(ap);

    if (g_err.jmp_armed) longjmp(g_err.jmp, 1);
    /* 没注册跳板——直接退出（仅初始化阶段才会发生）*/
}

/* 只记录不跳转：类型检查用（累积多个错误一起报）*/
void mycc_error_nolongjmp(ErrorKind k, int line, const char *file,
                          const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    err_fill(k, line, file, fmt, ap);
    va_end(ap);
}
