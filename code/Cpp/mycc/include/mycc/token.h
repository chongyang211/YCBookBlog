#pragma once

#include <string>
#include <variant>
#include <iostream>

namespace mycc {

// Token 类别
enum class TokKind {
    // 字面量
    Number,         // 整数或浮点：42、3.14
    String,         // 字符串："hello"
    True, False,    // 布尔：true、false

    // 标识符与关键字
    Ident,          // 标识符：x、fib
    Let, Print, If, Else, While, Fn, Return,   // 关键字

    // 单/多字符运算符
    Plus, Minus, Star, Slash, Percent,
    Assign,                                  // =
    EqEq, BangEq, Lt, Le, Gt, Ge,            // == != < <= > >=
    AndAnd, OrOr, Bang,                      // && || !
    LParen, RParen, LBrace, RBrace,          // ( ) { }
    Semicolon, Comma,                        // ; ,

    Eof,            // 输入结束哨兵
};

// Token：tag + 载荷（数字 / 字符串 / 空）
struct Token {
    TokKind kind;
    std::variant<std::monostate, double, std::string> value;
    int line = 1;       // 出错时定位

    Token() = default;
    Token(TokKind k, int ln = 1) : kind(k), line(ln) {}
    Token(TokKind k, double n, int ln = 1)            : kind(k), value(n), line(ln) {}
    Token(TokKind k, const std::string& s, int ln = 1): kind(k), value(s), line(ln) {}

    // 调试输出
    void dump(std::ostream& os) const;
};

// 把 TokKind 转成可读字符串（调试 + 错误信息用）
const char* tokKindName(TokKind k);

}  // namespace mycc
