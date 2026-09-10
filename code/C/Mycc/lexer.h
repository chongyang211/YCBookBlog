#ifndef MYCC_LEXER_H
#define MYCC_LEXER_H

#include "token.h"

/* Lexer 状态机：主循环按 lx_peek() 分支 = 确定性有限自动机（DFA）*/
typedef struct {
    const char *src;     /* 源字符串（不持有所有权）*/
    size_t      pos;
    int         line;
} Lexer;

void lexer_init(Lexer *lx, const char *source);

/* 一次切完所有 Token，返回动态数组（调用方 lexer_free_tokens 释放）*/
Token *lexer_tokenize(Lexer *lx, int *out_count);

void lexer_free_tokens(Token *tokens, int count);

#endif
