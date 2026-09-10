#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"

void lexer_init(Lexer *lx, const char *source) {
    lx->src = source;
    lx->pos = 0;
    lx->line = 1;
}

static char lx_peek(Lexer *lx) {
    return lx->src[lx->pos];          /* 末尾自然是 '\0' */
}

static char lx_advance(Lexer *lx) {
    return lx->src[lx->pos++];
}

static int lx_at_end(Lexer *lx) {
    return lx->src[lx->pos] == '\0';
}

/* 读取数字（整数或浮点）*/
static Token lx_read_number(Lexer *lx) {
    size_t start = lx->pos;
    while (!lx_at_end(lx) && isdigit((unsigned char)lx_peek(lx))) lx_advance(lx);
    if (lx_peek(lx) == '.') {
        lx_advance(lx);
        while (!lx_at_end(lx) && isdigit((unsigned char)lx_peek(lx))) lx_advance(lx);
    }
    char buf[64];
    size_t n = lx->pos - start;
    if (n >= sizeof(buf)) n = sizeof(buf) - 1;
    memcpy(buf, lx->src + start, n);
    buf[n] = '\0';
    Token t = {0};
    t.kind = TK_NUMBER;
    t.line = lx->line;
    t.as.number = atof(buf);
    return t;
}

/* 关键字表（线性查找——9 个关键字够用，不需要哈希表）*/
typedef struct { const char *kw; TokKind kind; } KwEntry;
static const KwEntry KEYWORDS[] = {
    {"var",   TK_VAR},   {"print",  TK_PRINT}, {"if",    TK_IF},
    {"else",  TK_ELSE},  {"while",  TK_WHILE}, {"fn",    TK_FN},
    {"return",TK_RETURN},{"true",   TK_TRUE},  {"false", TK_FALSE},
};
#define KEYWORDS_COUNT (sizeof(KEYWORDS) / sizeof(KEYWORDS[0]))

static Token lx_read_ident(Lexer *lx) {
    size_t start = lx->pos;
    while (!lx_at_end(lx) &&
           (isalnum((unsigned char)lx_peek(lx)) || lx_peek(lx) == '_')) {
        lx_advance(lx);
    }
    size_t len = lx->pos - start;
    char *text = (char *)malloc(len + 1);
    if (!text) mycc_error(ERR_IO, lx->line, NULL, "out of memory");
    memcpy(text, lx->src + start, len);
    text[len] = '\0';

    for (size_t i = 0; i < KEYWORDS_COUNT; i++) {
        if (strcmp(text, KEYWORDS[i].kw) == 0) {
            free(text);
            Token t = {0};
            t.kind = KEYWORDS[i].kind;
            t.line = lx->line;
            return t;
        }
    }
    Token t = {0};
    t.kind = TK_IDENT;
    t.line = lx->line;
    t.as.string = text;               /* ⭐ 持有所有权——token_free 时释放 */
    return t;
}

static Token lx_read_string(Lexer *lx) {
    lx_advance(lx);                   /* 吃掉开头的 " */
    size_t start = lx->pos;
    while (!lx_at_end(lx) && lx_peek(lx) != '"') {
        if (lx_peek(lx) == '\n') lx->line++;
        lx_advance(lx);
    }
    if (lx_at_end(lx))
        mycc_error(ERR_LEX, lx->line, NULL, "unterminated string");
    size_t len = lx->pos - start;
    char *s = (char *)malloc(len + 1);
    if (!s) mycc_error(ERR_IO, lx->line, NULL, "out of memory");
    memcpy(s, lx->src + start, len);
    s[len] = '\0';
    lx_advance(lx);                   /* 吃掉结尾的 " */
    Token t = {0};
    t.kind = TK_STRING;
    t.line = lx->line;
    t.as.string = s;
    return t;
}

/* 简易动态数组 */
typedef struct {
    Token *data;
    int    count;
    int    cap;
} TokVec;

static void tv_push(TokVec *v, Token t) {
    if (v->count >= v->cap) {
        v->cap = v->cap == 0 ? 16 : v->cap * 2;
        Token *nd = (Token *)realloc(v->data, (size_t)v->cap * sizeof(Token));
        if (!nd) mycc_error(ERR_IO, t.line, NULL, "out of memory");
        v->data = nd;
    }
    v->data[v->count++] = t;
}

/* 消耗 first 字符；若紧跟 second 产 two，否则产 one */
static void lx_two_char(Lexer *lx, char second, TokKind two, TokKind one,
                        TokVec *vec) {
    int ln = lx->line;
    lx_advance(lx);
    TokKind k = (lx_peek(lx) == second) ? (lx_advance(lx), two) : one;
    Token t = { .kind = k, .line = ln };
    tv_push(vec, t);
}

Token *lexer_tokenize(Lexer *lx, int *out_count) {
    TokVec vec = {0};

    while (!lx_at_end(lx)) {
        char c = lx_peek(lx);

        /* 1. 跳过空白 */
        if (c == ' ' || c == '\t' || c == '\r') { lx_advance(lx); continue; }
        if (c == '\n') { lx->line++; lx_advance(lx); continue; }

        /* 2. 单行注释 */
        if (c == '/' && lx->src[lx->pos + 1] == '/') {
            while (!lx_at_end(lx) && lx_peek(lx) != '\n') lx_advance(lx);
            continue;
        }

        /* 3. 数字与标识符 */
        if (isdigit((unsigned char)c)) { tv_push(&vec, lx_read_number(lx)); continue; }
        if (isalpha((unsigned char)c) || c == '_') { tv_push(&vec, lx_read_ident(lx)); continue; }

        /* 4. 字符串 */
        if (c == '"') { tv_push(&vec, lx_read_string(lx)); continue; }

        /* 5. 多字符运算符 */
        if (c == '=') { lx_two_char(lx, '=', TK_EQ, TK_ASSIGN, &vec); continue; }
        if (c == '!') { lx_two_char(lx, '=', TK_NE, TK_BANG,   &vec); continue; }
        if (c == '<') { lx_two_char(lx, '=', TK_LE, TK_LT,     &vec); continue; }
        if (c == '>') { lx_two_char(lx, '=', TK_GE, TK_GT,     &vec); continue; }
        if (c == '&') {
            lx_advance(lx);
            if (lx_peek(lx) != '&')
                mycc_error(ERR_LEX, lx->line, NULL, "single '&' is not supported");
            lx_advance(lx);
            Token t = { .kind = TK_ANDAND, .line = lx->line };
            tv_push(&vec, t);
            continue;
        }
        if (c == '|') {
            lx_advance(lx);
            if (lx_peek(lx) != '|')
                mycc_error(ERR_LEX, lx->line, NULL, "single '|' is not supported");
            lx_advance(lx);
            Token t = { .kind = TK_OROR, .line = lx->line };
            tv_push(&vec, t);
            continue;
        }

        /* 6. 单字符运算符与分隔符 */
        TokKind k;
        switch (c) {
        case '+': k = TK_PLUS;    break;
        case '-': k = TK_MINUS;   break;
        case '*': k = TK_STAR;    break;
        case '/': k = TK_SLASH;   break;
        case '%': k = TK_PERCENT; break;
        case '(': k = TK_LPAREN;  break;
        case ')': k = TK_RPAREN;  break;
        case '{': k = TK_LBRACE;  break;
        case '}': k = TK_RBRACE;  break;
        case ';': k = TK_SEMI;    break;
        case ',': k = TK_COMMA;   break;
        default:
            mycc_error(ERR_LEX, lx->line, NULL, "unexpected character '%c'", c);
            return NULL;              /* 不会到达 */
        }
        lx_advance(lx);
        Token t = { .kind = k, .line = lx->line };
        tv_push(&vec, t);
    }

    Token eof = { .kind = TK_EOF, .line = lx->line };
    tv_push(&vec, eof);

    *out_count = vec.count;
    return vec.data;
}

void lexer_free_tokens(Token *tokens, int count) {
    for (int i = 0; i < count; i++) token_free(&tokens[i]);
    free(tokens);
}
