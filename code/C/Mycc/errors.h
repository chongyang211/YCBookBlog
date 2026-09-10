/* 统一错误处理（§09）：四级错误码 + setjmp/longjmp 跳板
 * mycc_error = C 的 throw：打印诊断 + longjmp 回 main 的 setjmp 收口
 * file 传 NULL 时沿用 g_err.file（由 compile_and_run 预设）*/
#ifndef MYCC_ERRORS_H
#define MYCC_ERRORS_H

#include <setjmp.h>

typedef enum {
    ERR_NONE,
    ERR_LEX,
    ERR_PARSE,
    ERR_TYPE,
    ERR_RUNTIME,
    ERR_IO,
} ErrorKind;

typedef struct {
    ErrorKind kind;
    int       line;
    char      message[256];
    char      file[128];

    jmp_buf   jmp;          /* setjmp 跳板——main 注册、各阶段 longjmp */
    int       jmp_armed;    /* 是否已 setjmp——避免裸跳 UB */
} ErrorState;

extern ErrorState g_err;

void mycc_error(ErrorKind k, int line, const char *file, const char *fmt, ...)
        __attribute__((format(printf, 4, 5)));
void mycc_error_nolongjmp(ErrorKind k, int line, const char *file,
                          const char *fmt, ...)
        __attribute__((format(printf, 4, 5)));

/* Unix 惯例退出码：2=词法/语法 3=类型 4=运行时 1=IO */
int  error_exit_code(ErrorKind k);
const char *error_stage_name(ErrorKind k);
void error_reset(void);

#endif
