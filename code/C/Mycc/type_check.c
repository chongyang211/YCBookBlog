/* TypeChecker（§06）：switch(kind) 分派 + 作用域栈（lexical scoping）
 * C 没有 virtual——switch 就是最实在的"动态分派" */
#include "type_check.h"

#include <stdio.h>
#include <string.h>

#include "errors.h"

static void terr(TypeEnv *env, int line, const char *msg) {
    mycc_error_nolongjmp(ERR_TYPE, line, NULL, "%s", msg);   /* 累积不跳出 */
    env->has_error = 1;
}

void type_env_init(TypeEnv *env) {
    memset(env, 0, sizeof(*env));
    env->depth = 1;                    /* 起始就有"全局"作用域 */
}

static void enter_scope(TypeEnv *env) {
    if (env->depth >= MAX_SCOPE_DEPTH)
        mycc_error(ERR_TYPE, 0, NULL, "scope nesting too deep");
    env->scope_size[env->depth] = 0;
    env->depth++;
}

static void leave_scope(TypeEnv *env) {
    env->depth--;
}

/* 在当前作用域定义变量；已存在则失败返回 0 */
static int define_var(TypeEnv *env, const char *name, Type ty) {
    int s = env->depth - 1;
    for (int i = 0; i < env->scope_size[s]; ++i) {
        if (strcmp(env->scopes[s][i].name, name) == 0) return 0;
    }
    if (env->scope_size[s] >= MAX_SYMS_PER_SCOPE)
        mycc_error(ERR_TYPE, 0, NULL, "too many variables in one scope");
    VarSym *v = &env->scopes[s][env->scope_size[s]++];
    strncpy(v->name, name, 31); v->name[31] = '\0';
    v->ty = ty;
    return 1;
}

/* 从栈顶向下查找（lexical scoping）
 * found 区分"未定义"（报错）与"找到但类型 UNKNOWN"（动态参数，放行）*/
static Type lookup_var_ex(TypeEnv *env, const char *name, int *found) {
    *found = 0;
    for (int s = env->depth - 1; s >= 0; --s) {
        for (int i = 0; i < env->scope_size[s]; ++i) {
            if (strcmp(env->scopes[s][i].name, name) == 0) {
                *found = 1;
                return env->scopes[s][i].ty;
            }
        }
    }
    return TY_UNKNOWN;
}

static FnSig *lookup_fn(TypeEnv *env, const char *name) {
    for (int i = 0; i < env->fn_count; ++i) {
        if (strcmp(env->functions[i].name, name) == 0) return &env->functions[i];
    }
    return NULL;
}

static Type tc_binop(AstNode *n, TypeEnv *env) {
    Type lt = type_check(n->as.bin_op.lhs, env);
    Type rt = type_check(n->as.bin_op.rhs, env);
    TokKind op = n->as.bin_op.op;

    /* 函数参数无类型标注 → TY_UNKNOWN（动态类型语义）：放行不报错 */
    if (lt == TY_UNKNOWN || rt == TY_UNKNOWN) return n->ty = TY_UNKNOWN;

    switch (op) {
    case TK_PLUS:                       /* NUM+NUM=NUM；STR+STR=STR（拼接）*/
        if (lt == TY_NUM && rt == TY_NUM) return n->ty = TY_NUM;
        if (lt == TY_STR && rt == TY_STR) return n->ty = TY_STR;
        terr(env, n->line, "'+' needs Num+Num or Str+Str");
        return n->ty = TY_UNKNOWN;

    case TK_MINUS: case TK_STAR: case TK_SLASH: case TK_PERCENT:
        if (lt != TY_NUM || rt != TY_NUM) {
            terr(env, n->line, "arithmetic needs Num");
            return n->ty = TY_UNKNOWN;
        }
        return n->ty = TY_NUM;

    case TK_EQ: case TK_NE:
        if (lt != rt) {
            terr(env, n->line, "'==' / '!=' need same types");
            return n->ty = TY_UNKNOWN;
        }
        return n->ty = TY_BOOL;

    case TK_LT: case TK_LE: case TK_GT: case TK_GE:
        if (lt != TY_NUM || rt != TY_NUM) {
            terr(env, n->line, "comparison needs Num");
            return n->ty = TY_UNKNOWN;
        }
        return n->ty = TY_BOOL;

    case TK_ANDAND: case TK_OROR:
        if (lt != TY_BOOL || rt != TY_BOOL) {
            terr(env, n->line, "logic needs Bool");
            return n->ty = TY_UNKNOWN;
        }
        return n->ty = TY_BOOL;

    default:
        terr(env, n->line, "unknown binary operator");
        return n->ty = TY_UNKNOWN;
    }
}

static Type tc_unary(AstNode *n, TypeEnv *env) {
    Type t = type_check(n->as.unary_op.operand, env);
    if (n->as.unary_op.op == TK_MINUS) {
        if (t != TY_NUM) terr(env, n->line, "unary '-' needs Num");
        return n->ty = TY_NUM;
    }
    if (n->as.unary_op.op == TK_BANG) {
        if (t != TY_BOOL) terr(env, n->line, "unary '!' needs Bool");
        return n->ty = TY_BOOL;
    }
    return n->ty = TY_UNKNOWN;
}

static Type tc_assign(AstNode *n, TypeEnv *env) {
    Type rhs = type_check(n->as.assign.value, env);
    int found = 0;
    Type lhs = lookup_var_ex(env, n->as.assign.name, &found);
    if (!found) {
        terr(env, n->line, "assignment to undefined variable");
        return n->ty = TY_UNKNOWN;
    }
    if (lhs != TY_UNKNOWN && rhs != TY_UNKNOWN && lhs != rhs) {
        terr(env, n->line, "assignment type mismatch");
        return n->ty = TY_UNKNOWN;
    }
    return n->ty = rhs;
}

static Type tc_call(AstNode *n, TypeEnv *env) {
    FnSig *sig = lookup_fn(env, n->as.call_expr.callee);
    if (!sig) {
        terr(env, n->line, "undefined function");
        return n->ty = TY_UNKNOWN;
    }
    if (n->as.call_expr.n_args != sig->param_count) {
        terr(env, n->line, "argument count mismatch");
        return n->ty = TY_UNKNOWN;
    }
    for (int i = 0; i < n->as.call_expr.n_args; ++i)
        type_check(n->as.call_expr.args[i], env);
    return n->ty = TY_NUM;             /* 简化：所有函数返回 Num */
}

static Type tc_var_decl(AstNode *n, TypeEnv *env) {
    Type t = TY_NUM;
    if (n->as.var_decl.init) t = type_check(n->as.var_decl.init, env);
    if (!define_var(env, n->as.var_decl.name, t))
        terr(env, n->line, "duplicate variable definition");
    return TY_VOID;
}

static Type tc_block(AstNode *n, TypeEnv *env) {
    enter_scope(env);                  /* ⭐ §6.5 造 BUG 的修复：配对进出作用域 */
    for (int i = 0; i < n->as.block.n_stmts; ++i)
        type_check(n->as.block.stmts[i], env);
    leave_scope(env);
    return TY_VOID;
}

static Type tc_if(AstNode *n, TypeEnv *env) {
    Type c = type_check(n->as.if_stmt.cond, env);
    if (c != TY_BOOL && c != TY_UNKNOWN)      /* UNKNOWN=动态参数参与的比较，放行 */
        terr(env, n->line, "if condition needs Bool");
    type_check(n->as.if_stmt.then_branch, env);
    if (n->as.if_stmt.else_branch) type_check(n->as.if_stmt.else_branch, env);
    return TY_VOID;
}

static Type tc_while(AstNode *n, TypeEnv *env) {
    Type c = type_check(n->as.while_stmt.cond, env);
    if (c != TY_BOOL && c != TY_UNKNOWN)
        terr(env, n->line, "while condition needs Bool");
    type_check(n->as.while_stmt.body, env);
    return TY_VOID;
}

static Type tc_fn_decl(AstNode *n, TypeEnv *env) {
    /* 函数签名已在 tc_program 第一遍预注册——这里只检查函数体 */
    enter_scope(env);
    for (int i = 0; i < n->as.fn_decl.n_params; ++i)
        define_var(env, n->as.fn_decl.params[i], TY_UNKNOWN);  /* 无标注=动态类型 */
    type_check(n->as.fn_decl.body, env);
    leave_scope(env);
    return TY_VOID;
}

static Type tc_program(AstNode *n, TypeEnv *env) {
    /* 第一遍：预注册所有顶层函数，支持相互递归 */
    for (int i = 0; i < n->as.program.n_decls; ++i) {
        AstNode *d = n->as.program.decls[i];
        if (d->kind == NK_FN_DECL) {
            if (env->fn_count >= MAX_FUNCTIONS)
                mycc_error(ERR_TYPE, 0, NULL, "too many functions");
            FnSig *s = &env->functions[env->fn_count++];
            strncpy(s->name, d->as.fn_decl.name, 31);
            s->name[31] = '\0';
            s->param_count = d->as.fn_decl.n_params;
            s->line = d->line;
        }
    }
    /* 第二遍：真正递归检查 */
    for (int i = 0; i < n->as.program.n_decls; ++i)
        type_check(n->as.program.decls[i], env);
    return TY_VOID;
}

/* === 主入口：switch 分派 === */
Type type_check(AstNode *n, TypeEnv *env) {
    if (!n) return TY_VOID;
    switch (n->kind) {
    case NK_NUM_LIT:    return n->ty = TY_NUM;
    case NK_STR_LIT:    return n->ty = TY_STR;
    case NK_BOOL_LIT:   return n->ty = TY_BOOL;
    case NK_VAR_REF: {
        int found = 0;
        Type t = lookup_var_ex(env, n->as.var_ref.name, &found);
        if (!found) terr(env, n->line, "undefined variable");
        return n->ty = t;
    }
    case NK_BIN_OP:     return tc_binop(n, env);
    case NK_UNARY_OP:   return tc_unary(n, env);
    case NK_ASSIGN:     return tc_assign(n, env);
    case NK_CALL_EXPR:  return tc_call(n, env);

    case NK_VAR_DECL:   return tc_var_decl(n, env);
    case NK_PRINT_STMT: type_check(n->as.print_stmt.expr, env); return TY_VOID;
    case NK_EXPR_STMT:  type_check(n->as.expr_stmt.expr, env);  return TY_VOID;
    case NK_BLOCK:      return tc_block(n, env);
    case NK_IF_STMT:    return tc_if(n, env);
    case NK_WHILE_STMT: return tc_while(n, env);
    case NK_FN_DECL:    return tc_fn_decl(n, env);
    case NK_RETURN_STMT:
        if (n->as.return_stmt.value) type_check(n->as.return_stmt.value, env);
        return TY_VOID;
    case NK_PROGRAM:    return tc_program(n, env);
    }
    terr(env, n ? n->line : 0, "unhandled node kind");
    return TY_UNKNOWN;
}
