/* AST（§04）：tagged union 模拟多态——12 种节点 + 17 个工厂 + 树形释放 */
#ifndef MYCC_AST_H
#define MYCC_AST_H

#include "token.h"

typedef enum {
    TY_UNKNOWN, TY_NUM, TY_BOOL, TY_STR, TY_VOID
} Type;

typedef enum {
    // 表达式
    NK_NUM_LIT, NK_STR_LIT, NK_BOOL_LIT, NK_VAR_REF,
    NK_BIN_OP, NK_UNARY_OP, NK_ASSIGN, NK_CALL_EXPR,
    // 语句
    NK_VAR_DECL, NK_PRINT_STMT, NK_EXPR_STMT, NK_BLOCK,
    NK_IF_STMT, NK_WHILE_STMT, NK_FN_DECL, NK_RETURN_STMT,
    // 顶层
    NK_PROGRAM,
} NodeKind;

typedef struct AstNode AstNode;

struct AstNode {
    NodeKind kind;
    int      line;
    Type     ty;                 /* TypeChecker 贴的标签（不变形）*/

    union {
        struct { double value; }                        num_lit;
        struct { char  *value; }                        str_lit;   /* 堆上 */
        struct { int    value; }                        bool_lit;
        struct { char  *name; }                         var_ref;   /* 堆上 */

        struct { TokKind op; AstNode *lhs, *rhs; }      bin_op;
        struct { TokKind op; AstNode *operand; }        unary_op;
        struct { char *name; AstNode *value; }          assign;
        struct { char *callee; AstNode **args; int n_args; } call_expr;

        struct { char *name; AstNode *init; }           var_decl;
        struct { AstNode *expr; }                       print_stmt;
        struct { AstNode *expr; }                       expr_stmt;
        struct { AstNode **stmts; int n_stmts; }        block;
        struct { AstNode *cond, *then_branch, *else_branch; } if_stmt;
        struct { AstNode *cond, *body; }                while_stmt;
        struct { char *name; char **params; int n_params; AstNode *body; } fn_decl;
        struct { AstNode *value; }                      return_stmt;

        struct { AstNode **decls; int n_decls; }        program;
    } as;
};

AstNode *ast_num_lit(double v, int line);
AstNode *ast_str_lit(char *s, int line);
AstNode *ast_bool_lit(int v, int line);
AstNode *ast_var_ref(char *name, int line);
AstNode *ast_bin_op(TokKind op, AstNode *lhs, AstNode *rhs, int line);
AstNode *ast_unary_op(TokKind op, AstNode *operand, int line);
AstNode *ast_assign(char *name, AstNode *value, int line);
AstNode *ast_call(char *callee, AstNode **args, int n_args, int line);
AstNode *ast_var_decl(char *name, AstNode *init, int line);
AstNode *ast_print_stmt(AstNode *e, int line);
AstNode *ast_expr_stmt(AstNode *e, int line);
AstNode *ast_block(AstNode **stmts, int n_stmts, int line);
AstNode *ast_if(AstNode *cond, AstNode *t, AstNode *e, int line);
AstNode *ast_while(AstNode *cond, AstNode *body, int line);
AstNode *ast_fn_decl(char *name, char **params, int np, AstNode *body, int line);
AstNode *ast_return(AstNode *v, int line);
AstNode *ast_program(AstNode **decls, int n, int line);

void ast_free(AstNode *n);
void ast_dump(AstNode *n, int depth, FILE *out);

#endif
