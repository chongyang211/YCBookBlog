#ifndef MYCC_TYPE_CHECK_H
#define MYCC_TYPE_CHECK_H

#include "ast.h"

#define MAX_SCOPE_DEPTH    16
#define MAX_SYMS_PER_SCOPE 64
#define MAX_FUNCTIONS      64

typedef struct {
    char name[32];
    Type ty;
} VarSym;

typedef struct {
    char name[32];
    int  param_count;
    int  line;
} FnSig;

typedef struct {
    VarSym scopes[MAX_SCOPE_DEPTH][MAX_SYMS_PER_SCOPE];
    int    scope_size[MAX_SCOPE_DEPTH];
    int    depth;                      /* 0 = 全局作用域 */

    FnSig  functions[MAX_FUNCTIONS];
    int    fn_count;

    int    has_error;                  /* 类型错误累积标志 */
} TypeEnv;

void type_env_init(TypeEnv *env);
Type type_check(AstNode *node, TypeEnv *env);

#endif
