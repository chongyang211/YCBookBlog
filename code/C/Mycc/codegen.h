#ifndef MYCC_CODEGEN_H
#define MYCC_CODEGEN_H

#include "ast.h"
#include "chunk.h"

#define MAX_LOCALS 64
#define MAX_FNS    64

typedef struct {
    char  name[32];
    int   depth;             /* 所在作用域深度 */
} LocalVar;

/* 函数表——每个函数一个独立 Chunk */
typedef struct {
    char  name[32];
    int   param_count;
    Chunk chunk;
} CompiledFn;

typedef struct {
    Chunk    *cur;           /* 当前正在写的 Chunk（顶层 / 某函数）*/
    LocalVar  locals[MAX_LOCALS];
    int       local_count;
    int       scope_depth;   /* 当前块嵌套深度（0=全局/函数顶）*/

    CompiledFn fns[MAX_FNS];
    int        fn_count;
    int        in_function;  /* 决定 var 走 LOCAL 还是 GLOBAL */
} Codegen;

void codegen_init(Codegen *cg);
void codegen_free(Codegen *cg);

/* 编译整个 Program 到 cg->fns[0]（"<top>"）*/
void codegen_program(Codegen *cg, AstNode *prog);

#endif
