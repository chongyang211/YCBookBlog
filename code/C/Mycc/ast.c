#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static AstNode *ast_alloc(NodeKind k, int line) {
    AstNode *n = (AstNode *)calloc(1, sizeof(AstNode));  /* calloc 自动清零 */
    if (!n) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    n->kind = k;
    n->line = line;
    n->ty = TY_UNKNOWN;
    return n;
}

AstNode *ast_num_lit(double v, int line) {
    AstNode *n = ast_alloc(NK_NUM_LIT, line);
    n->as.num_lit.value = v;
    return n;
}

AstNode *ast_str_lit(char *s, int line) {
    AstNode *n = ast_alloc(NK_STR_LIT, line);
    n->as.str_lit.value = s;
    return n;
}

AstNode *ast_bool_lit(int v, int line) {
    AstNode *n = ast_alloc(NK_BOOL_LIT, line);
    n->as.bool_lit.value = v;
    return n;
}

AstNode *ast_var_ref(char *name, int line) {
    AstNode *n = ast_alloc(NK_VAR_REF, line);
    n->as.var_ref.name = name;
    return n;
}

AstNode *ast_bin_op(TokKind op, AstNode *lhs, AstNode *rhs, int line) {
    AstNode *n = ast_alloc(NK_BIN_OP, line);
    n->as.bin_op.op = op;
    n->as.bin_op.lhs = lhs;
    n->as.bin_op.rhs = rhs;
    return n;
}

AstNode *ast_unary_op(TokKind op, AstNode *operand, int line) {
    AstNode *n = ast_alloc(NK_UNARY_OP, line);
    n->as.unary_op.op = op;
    n->as.unary_op.operand = operand;
    return n;
}

AstNode *ast_assign(char *name, AstNode *value, int line) {
    AstNode *n = ast_alloc(NK_ASSIGN, line);
    n->as.assign.name = name;
    n->as.assign.value = value;
    return n;
}

AstNode *ast_call(char *callee, AstNode **args, int n_args, int line) {
    AstNode *n = ast_alloc(NK_CALL_EXPR, line);
    n->as.call_expr.callee = callee;
    n->as.call_expr.args = args;
    n->as.call_expr.n_args = n_args;
    return n;
}

AstNode *ast_var_decl(char *name, AstNode *init, int line) {
    AstNode *n = ast_alloc(NK_VAR_DECL, line);
    n->as.var_decl.name = name;
    n->as.var_decl.init = init;
    return n;
}

AstNode *ast_print_stmt(AstNode *e, int line) {
    AstNode *n = ast_alloc(NK_PRINT_STMT, line);
    n->as.print_stmt.expr = e;
    return n;
}

AstNode *ast_expr_stmt(AstNode *e, int line) {
    AstNode *n = ast_alloc(NK_EXPR_STMT, line);
    n->as.expr_stmt.expr = e;
    return n;
}

AstNode *ast_block(AstNode **stmts, int nn, int line) {
    AstNode *n = ast_alloc(NK_BLOCK, line);
    n->as.block.stmts = stmts;
    n->as.block.n_stmts = nn;
    return n;
}

AstNode *ast_if(AstNode *cond, AstNode *t, AstNode *e, int line) {
    AstNode *n = ast_alloc(NK_IF_STMT, line);
    n->as.if_stmt.cond = cond;
    n->as.if_stmt.then_branch = t;
    n->as.if_stmt.else_branch = e;
    return n;
}

AstNode *ast_while(AstNode *cond, AstNode *body, int line) {
    AstNode *n = ast_alloc(NK_WHILE_STMT, line);
    n->as.while_stmt.cond = cond;
    n->as.while_stmt.body = body;
    return n;
}

AstNode *ast_fn_decl(char *name, char **params, int np, AstNode *body, int line) {
    AstNode *n = ast_alloc(NK_FN_DECL, line);
    n->as.fn_decl.name = name;
    n->as.fn_decl.params = params;
    n->as.fn_decl.n_params = np;
    n->as.fn_decl.body = body;
    return n;
}

AstNode *ast_return(AstNode *v, int line) {
    AstNode *n = ast_alloc(NK_RETURN_STMT, line);
    n->as.return_stmt.value = v;
    return n;
}

AstNode *ast_program(AstNode **decls, int nn, int line) {
    AstNode *n = ast_alloc(NK_PROGRAM, line);
    n->as.program.decls = decls;
    n->as.program.n_decls = nn;
    return n;
}

/* 树形释放——必须按 union 分支递归（C 版虚析构）*/
void ast_free(AstNode *n) {
    if (n == NULL) return;
    switch (n->kind) {
    case NK_NUM_LIT: case NK_BOOL_LIT: break;
    case NK_STR_LIT:    free(n->as.str_lit.value); break;
    case NK_VAR_REF:    free(n->as.var_ref.name); break;
    case NK_BIN_OP:
        ast_free(n->as.bin_op.lhs);
        ast_free(n->as.bin_op.rhs); break;
    case NK_UNARY_OP:   ast_free(n->as.unary_op.operand); break;
    case NK_ASSIGN:
        free(n->as.assign.name);
        ast_free(n->as.assign.value); break;
    case NK_CALL_EXPR:
        free(n->as.call_expr.callee);
        for (int i = 0; i < n->as.call_expr.n_args; i++)
            ast_free(n->as.call_expr.args[i]);
        free(n->as.call_expr.args); break;
    case NK_VAR_DECL:
        free(n->as.var_decl.name);
        ast_free(n->as.var_decl.init); break;
    case NK_PRINT_STMT: ast_free(n->as.print_stmt.expr); break;
    case NK_EXPR_STMT:  ast_free(n->as.expr_stmt.expr); break;
    case NK_BLOCK:
        for (int i = 0; i < n->as.block.n_stmts; i++)
            ast_free(n->as.block.stmts[i]);
        free(n->as.block.stmts); break;
    case NK_IF_STMT:
        ast_free(n->as.if_stmt.cond);
        ast_free(n->as.if_stmt.then_branch);
        ast_free(n->as.if_stmt.else_branch); break;
    case NK_WHILE_STMT:
        ast_free(n->as.while_stmt.cond);
        ast_free(n->as.while_stmt.body); break;
    case NK_FN_DECL:
        free(n->as.fn_decl.name);
        for (int i = 0; i < n->as.fn_decl.n_params; i++)
            free(n->as.fn_decl.params[i]);
        free(n->as.fn_decl.params);
        ast_free(n->as.fn_decl.body); break;
    case NK_RETURN_STMT: ast_free(n->as.return_stmt.value); break;
    case NK_PROGRAM:
        for (int i = 0; i < n->as.program.n_decls; i++)
            ast_free(n->as.program.decls[i]);
        free(n->as.program.decls); break;
    }
    free(n);
}

static void indent(int d, FILE *out) { for (int i = 0; i < d; i++) fputs("  ", out); }

void ast_dump(AstNode *n, int depth, FILE *out) {
    if (n == NULL) return;
    indent(depth, out);
    switch (n->kind) {
    case NK_NUM_LIT:  fprintf(out, "NumLit %g\n", n->as.num_lit.value); break;
    case NK_STR_LIT:  fprintf(out, "StrLit \"%s\"\n", n->as.str_lit.value); break;
    case NK_BOOL_LIT: fprintf(out, "BoolLit %s\n", n->as.bool_lit.value ? "true" : "false"); break;
    case NK_VAR_REF:  fprintf(out, "VarRef %s\n", n->as.var_ref.name); break;
    case NK_BIN_OP:
        fprintf(out, "BinOp %s\n", tok_kind_name(n->as.bin_op.op));
        ast_dump(n->as.bin_op.lhs, depth + 1, out);
        ast_dump(n->as.bin_op.rhs, depth + 1, out); break;
    case NK_UNARY_OP:
        fprintf(out, "UnaryOp %s\n", tok_kind_name(n->as.unary_op.op));
        ast_dump(n->as.unary_op.operand, depth + 1, out); break;
    case NK_ASSIGN:
        fprintf(out, "Assign %s\n", n->as.assign.name);
        ast_dump(n->as.assign.value, depth + 1, out); break;
    case NK_CALL_EXPR:
        fprintf(out, "Call %s (%d args)\n", n->as.call_expr.callee, n->as.call_expr.n_args);
        for (int i = 0; i < n->as.call_expr.n_args; i++)
            ast_dump(n->as.call_expr.args[i], depth + 1, out);
        break;
    case NK_VAR_DECL:
        fprintf(out, "VarDecl %s\n", n->as.var_decl.name);
        if (n->as.var_decl.init) ast_dump(n->as.var_decl.init, depth + 1, out);
        break;
    case NK_PRINT_STMT:
        fprintf(out, "PrintStmt\n");
        ast_dump(n->as.print_stmt.expr, depth + 1, out); break;
    case NK_EXPR_STMT:
        fprintf(out, "ExprStmt\n");
        ast_dump(n->as.expr_stmt.expr, depth + 1, out); break;
    case NK_BLOCK:
        fprintf(out, "Block (%d stmts)\n", n->as.block.n_stmts);
        for (int i = 0; i < n->as.block.n_stmts; i++)
            ast_dump(n->as.block.stmts[i], depth + 1, out);
        break;
    case NK_IF_STMT:
        fprintf(out, "If\n");
        ast_dump(n->as.if_stmt.cond, depth + 1, out);
        ast_dump(n->as.if_stmt.then_branch, depth + 1, out);
        if (n->as.if_stmt.else_branch) ast_dump(n->as.if_stmt.else_branch, depth + 1, out);
        break;
    case NK_WHILE_STMT:
        fprintf(out, "While\n");
        ast_dump(n->as.while_stmt.cond, depth + 1, out);
        ast_dump(n->as.while_stmt.body, depth + 1, out); break;
    case NK_FN_DECL:
        fprintf(out, "FnDecl %s(%d params)\n", n->as.fn_decl.name, n->as.fn_decl.n_params);
        ast_dump(n->as.fn_decl.body, depth + 1, out); break;
    case NK_RETURN_STMT:
        fprintf(out, "Return\n");
        if (n->as.return_stmt.value) ast_dump(n->as.return_stmt.value, depth + 1, out);
        break;
    case NK_PROGRAM:
        fprintf(out, "Program (%d decls)\n", n->as.program.n_decls);
        for (int i = 0; i < n->as.program.n_decls; i++)
            ast_dump(n->as.program.decls[i], depth + 1, out);
        break;
    }
}
