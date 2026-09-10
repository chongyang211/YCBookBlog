#ifndef MYCC_PARSER_H
#define MYCC_PARSER_H

#include "ast.h"
#include "token.h"

typedef struct {
    Token   *toks;
    int      count;
    int      pos;
    int      had_error;
} Parser;

void parser_init(Parser *p, Token *toks, int count);

/* 解析完整程序，返回 NK_PROGRAM 根；失败经 mycc_error 跳出 */
AstNode *parser_parse_program(Parser *p);

#endif
