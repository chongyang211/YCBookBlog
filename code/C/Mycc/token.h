/* Token（§03）：tagged union——enum 标签 + union 载荷
 * 三铁律：先看 kind 才能访问 union / malloc 配对 free / 构造时清零 */
#ifndef MYCC_TOKEN_H
#define MYCC_TOKEN_H

#include <stdio.h>

typedef enum {
    // 字面量
    TK_NUMBER,         // 整数或浮点：42、3.14
    TK_STRING,         // 字符串："hello"
    TK_TRUE, TK_FALSE, // 布尔

    // 标识符与关键字
    TK_IDENT,
    TK_VAR, TK_PRINT, TK_IF, TK_ELSE, TK_WHILE, TK_FN, TK_RETURN,

    // 单/多字符运算符
    TK_PLUS, TK_MINUS, TK_STAR, TK_SLASH, TK_PERCENT,
    TK_ASSIGN,                                  // =
    TK_EQ, TK_NE, TK_LT, TK_LE, TK_GT, TK_GE,   // == != < <= > >=
    TK_ANDAND, TK_OROR, TK_BANG,                // && || !
    TK_LPAREN, TK_RPAREN, TK_LBRACE, TK_RBRACE, // ( ) { }
    TK_SEMI, TK_COMMA,                          // ; ,

    TK_EOF,            // 输入结束哨兵
} TokKind;

typedef struct {
    TokKind kind;
    int     line;
    /* number 用 double（int 也存这）；string/ident 用 char*（堆上，token_free 释放）*/
    union {
        double  number;
        char   *string;
    } as;
} Token;

const char *tok_kind_name(TokKind k);
void token_dump(const Token *t, FILE *out);
void token_free(Token *t);

#endif
