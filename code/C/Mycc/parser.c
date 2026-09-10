/* 递归下降 Parser（§05）：文法规则 → 函数；左结合 = while + 高优先级层
 * 错误统一走 mycc_error → longjmp 回 main 收口 */
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"

void parser_init(Parser *p, Token *toks, int count) {
    p->toks = toks;
    p->count = count;
    p->pos = 0;
    p->had_error = 0;
}

static Token *peek(Parser *p) { return &p->toks[p->pos]; }
static Token *advance(Parser *p) { return &p->toks[p->pos++]; }
static int    check(Parser *p, TokKind k) { return peek(p)->kind == k; }

static int match(Parser *p, TokKind k) {
    if (check(p, k)) { advance(p); return 1; }
    return 0;
}

static Token *expect(Parser *p, TokKind k, const char *what) {
    if (!check(p, k)) {
        mycc_error(ERR_PARSE, peek(p)->line, NULL,
                   "expected %s but got '%s'", what, tok_kind_name(peek(p)->kind));
    }
    return advance(p);
}

static char *str_dup(const char *s) {
    size_t n = strlen(s);
    char *r = (char *)malloc(n + 1);
    if (!r) mycc_error(ERR_IO, 0, NULL, "out of memory");
    memcpy(r, s, n + 1);
    return r;
}

/* === 表达式 9 层优先级（自上而下越来越低）=== */
static AstNode *parse_expression(Parser *p);
static AstNode *parse_assignment(Parser *p);
static AstNode *parse_logic_or(Parser *p);
static AstNode *parse_logic_and(Parser *p);
static AstNode *parse_equality(Parser *p);
static AstNode *parse_comparison(Parser *p);
static AstNode *parse_addition(Parser *p);
static AstNode *parse_multiplication(Parser *p);
static AstNode *parse_unary(Parser *p);
static AstNode *parse_call(Parser *p);
static AstNode *parse_primary(Parser *p);

static AstNode *parse_primary(Parser *p) {
    Token *t = peek(p);
    if (t->kind == TK_NUMBER) {
        advance(p);
        return ast_num_lit(t->as.number, t->line);
    }
    if (t->kind == TK_STRING) {
        advance(p);
        return ast_str_lit(str_dup(t->as.string), t->line);
    }
    if (t->kind == TK_TRUE)  { advance(p); return ast_bool_lit(1, t->line); }
    if (t->kind == TK_FALSE) { advance(p); return ast_bool_lit(0, t->line); }
    if (t->kind == TK_IDENT) {
        advance(p);
        return ast_var_ref(str_dup(t->as.string), t->line);
    }
    if (t->kind == TK_LPAREN) {
        advance(p);
        AstNode *inner = parse_expression(p);
        expect(p, TK_RPAREN, ")");
        return inner;
    }
    mycc_error(ERR_PARSE, t->line, NULL,
               "expected expression but got '%s'", tok_kind_name(t->kind));
    return NULL;                      /* 不会到达 */
}

static AstNode *parse_call(Parser *p) {
    AstNode *expr = parse_primary(p);
    /* 函数调用：foo(a, b)——primary 是 VarRef 且后跟 '(' */
    if (expr->kind == NK_VAR_REF && check(p, TK_LPAREN)) {
        advance(p);
        AstNode **args = NULL;
        int n_args = 0, cap = 0;
        if (!check(p, TK_RPAREN)) {
            do {
                if (n_args >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    AstNode **na = (AstNode **)realloc(args, (size_t)cap * sizeof(AstNode *));
                    if (!na) mycc_error(ERR_IO, expr->line, NULL, "out of memory");
                    args = na;
                }
                args[n_args++] = parse_expression(p);
            } while (match(p, TK_COMMA));
        }
        expect(p, TK_RPAREN, ")");
        /* VarRef 的 name 转移给 CallExpr，释放 VarRef 的壳（防二次释放）*/
        char *callee = expr->as.var_ref.name;
        expr->as.var_ref.name = NULL;
        int line = expr->line;
        ast_free(expr);
        return ast_call(callee, args, n_args, line);
    }
    return expr;
}

static AstNode *parse_unary(Parser *p) {
    if (check(p, TK_MINUS) || check(p, TK_BANG)) {
        Token *t = advance(p);
        AstNode *operand = parse_unary(p);    /* 右结合：--x 合法 */
        return ast_unary_op(t->kind, operand, t->line);
    }
    return parse_call(p);
}

/* ⭐ 左结合：while 循环 + 调高优先级层（§5.6 造 BUG 的修复版）*/
static AstNode *parse_multiplication(Parser *p) {
    AstNode *lhs = parse_unary(p);
    while (check(p, TK_STAR) || check(p, TK_SLASH) || check(p, TK_PERCENT)) {
        TokKind op = advance(p)->kind;
        AstNode *rhs = parse_unary(p);
        lhs = ast_bin_op(op, lhs, rhs, lhs->line);
    }
    return lhs;
}

static AstNode *parse_addition(Parser *p) {
    AstNode *lhs = parse_multiplication(p);
    while (check(p, TK_PLUS) || check(p, TK_MINUS)) {
        TokKind op = advance(p)->kind;
        AstNode *rhs = parse_multiplication(p);
        lhs = ast_bin_op(op, lhs, rhs, lhs->line);
    }
    return lhs;
}

static AstNode *parse_comparison(Parser *p) {
    AstNode *lhs = parse_addition(p);
    while (check(p, TK_LT) || check(p, TK_LE) ||
           check(p, TK_GT) || check(p, TK_GE)) {
        TokKind op = advance(p)->kind;
        AstNode *rhs = parse_addition(p);
        lhs = ast_bin_op(op, lhs, rhs, lhs->line);
    }
    return lhs;
}

static AstNode *parse_equality(Parser *p) {
    AstNode *lhs = parse_comparison(p);
    while (check(p, TK_EQ) || check(p, TK_NE)) {
        TokKind op = advance(p)->kind;
        AstNode *rhs = parse_comparison(p);
        lhs = ast_bin_op(op, lhs, rhs, lhs->line);
    }
    return lhs;
}

static AstNode *parse_logic_and(Parser *p) {
    AstNode *lhs = parse_equality(p);
    while (check(p, TK_ANDAND)) {
        advance(p);
        AstNode *rhs = parse_equality(p);
        lhs = ast_bin_op(TK_ANDAND, lhs, rhs, lhs->line);
    }
    return lhs;
}

static AstNode *parse_logic_or(Parser *p) {
    AstNode *lhs = parse_logic_and(p);
    while (check(p, TK_OROR)) {
        advance(p);
        AstNode *rhs = parse_logic_and(p);
        lhs = ast_bin_op(TK_OROR, lhs, rhs, lhs->line);
    }
    return lhs;
}

static AstNode *parse_assignment(Parser *p) {
    AstNode *lhs = parse_logic_or(p);
    if (check(p, TK_ASSIGN)) {
        advance(p);
        AstNode *rhs = parse_assignment(p);     /* 右结合：a = b = 1 合法 */
        if (lhs->kind != NK_VAR_REF) {
            mycc_error(ERR_PARSE, lhs->line, NULL,
                       "assignment target must be a variable");
        }
        char *name = lhs->as.var_ref.name;
        lhs->as.var_ref.name = NULL;
        int line = lhs->line;
        ast_free(lhs);
        return ast_assign(name, rhs, line);
    }
    return lhs;
}

static AstNode *parse_expression(Parser *p) { return parse_assignment(p); }

/* === 语句层 === */
static AstNode *parse_statement(Parser *p);
static AstNode *parse_block(Parser *p);

static AstNode *parse_var_decl(Parser *p) {
    int ln = advance(p)->line;         /* 吃 var */
    Token *name_tok = expect(p, TK_IDENT, "variable name");
    char *name = str_dup(name_tok->as.string);
    AstNode *init = NULL;
    if (match(p, TK_ASSIGN)) init = parse_expression(p);
    expect(p, TK_SEMI, ";");
    return ast_var_decl(name, init, ln);
}

static AstNode *parse_print_stmt(Parser *p) {
    int ln = advance(p)->line;
    AstNode *e = parse_expression(p);
    expect(p, TK_SEMI, ";");
    return ast_print_stmt(e, ln);
}

static AstNode *parse_if_stmt(Parser *p) {
    int ln = advance(p)->line;
    expect(p, TK_LPAREN, "(");
    AstNode *cond = parse_expression(p);
    expect(p, TK_RPAREN, ")");
    AstNode *then_b = parse_statement(p);
    AstNode *else_b = NULL;
    if (match(p, TK_ELSE)) else_b = parse_statement(p);
    return ast_if(cond, then_b, else_b, ln);
}

static AstNode *parse_while_stmt(Parser *p) {
    int ln = advance(p)->line;
    expect(p, TK_LPAREN, "(");
    AstNode *cond = parse_expression(p);
    expect(p, TK_RPAREN, ")");
    AstNode *body = parse_statement(p);
    return ast_while(cond, body, ln);
}

static AstNode *parse_block(Parser *p) {
    int ln = advance(p)->line;         /* 吃 { */
    AstNode **stmts = NULL;
    int n = 0, cap = 0;
    while (!check(p, TK_RBRACE) && !check(p, TK_EOF)) {
        if (n >= cap) {
            cap = cap == 0 ? 4 : cap * 2;
            AstNode **ns = (AstNode **)realloc(stmts, (size_t)cap * sizeof(AstNode *));
            if (!ns) mycc_error(ERR_IO, ln, NULL, "out of memory");
            stmts = ns;
        }
        stmts[n++] = parse_statement(p);
    }
    expect(p, TK_RBRACE, "}");
    return ast_block(stmts, n, ln);
}

static AstNode *parse_return_stmt(Parser *p) {
    int ln = advance(p)->line;
    AstNode *v = NULL;
    if (!check(p, TK_SEMI)) v = parse_expression(p);
    expect(p, TK_SEMI, ";");
    return ast_return(v, ln);
}

static AstNode *parse_expr_stmt(Parser *p) {
    AstNode *e = parse_expression(p);
    int ln = e->line;
    expect(p, TK_SEMI, ";");
    return ast_expr_stmt(e, ln);
}

static AstNode *parse_fn_decl(Parser *p) {
    int ln = advance(p)->line;         /* 吃 fn */
    Token *name_tok = expect(p, TK_IDENT, "function name");
    char *name = str_dup(name_tok->as.string);
    expect(p, TK_LPAREN, "(");
    char **params = NULL;
    int np = 0, cap = 0;
    if (!check(p, TK_RPAREN)) {
        do {
            Token *pt = expect(p, TK_IDENT, "parameter name");
            if (np >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                char **nps = (char **)realloc(params, (size_t)cap * sizeof(char *));
                if (!nps) mycc_error(ERR_IO, ln, NULL, "out of memory");
                params = nps;
            }
            params[np++] = str_dup(pt->as.string);
        } while (match(p, TK_COMMA));
    }
    expect(p, TK_RPAREN, ")");
    if (!check(p, TK_LBRACE))
        mycc_error(ERR_PARSE, ln, NULL, "function body must be a block {...}");
    AstNode *body = parse_block(p);
    return ast_fn_decl(name, params, np, body, ln);
}

static AstNode *parse_statement(Parser *p) {
    if (check(p, TK_VAR))    return parse_var_decl(p);
    if (check(p, TK_PRINT))  return parse_print_stmt(p);
    if (check(p, TK_IF))     return parse_if_stmt(p);
    if (check(p, TK_WHILE))  return parse_while_stmt(p);
    if (check(p, TK_RETURN)) return parse_return_stmt(p);
    if (check(p, TK_LBRACE)) return parse_block(p);
    if (check(p, TK_FN))     return parse_fn_decl(p);
    return parse_expr_stmt(p);
}

AstNode *parser_parse_program(Parser *p) {
    AstNode **decls = NULL;
    int n = 0, cap = 0;
    int line = peek(p)->line;
    while (!check(p, TK_EOF)) {
        if (n >= cap) {
            cap = cap == 0 ? 8 : cap * 2;
            AstNode **nd = (AstNode **)realloc(decls, (size_t)cap * sizeof(AstNode *));
            if (!nd) mycc_error(ERR_IO, line, NULL, "out of memory");
            decls = nd;
        }
        decls[n++] = parse_statement(p);
    }
    return ast_program(decls, n, line);
}
