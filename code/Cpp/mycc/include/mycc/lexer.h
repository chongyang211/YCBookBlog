#pragma once

#include "token.h"
#include <string>
#include <vector>

namespace Mycc {

class Lexer {
public:
    Lexer(std::string source, std::string filename = "<repl>")
        : src(std::move(source)), file_(std::move(filename)) {}

    std::vector<Token> scanAll();      // 主接口：一次切完所有 Token

private:
    std::string src;
    std::string file_;
    size_t pos = 0;
    int    line = 1;

    char peek()    const { return pos < src.size() ? src[pos] : '\0'; }
    char advance()       { return src[pos++]; }
    bool isAtEnd() const { return pos >= src.size(); }

    Token readNumber();   // 数字（整数/浮点）
    Token readIdent();    // 标识符 / 关键字
    Token readString();   // 字符串字面量
};

}  // namespace Mycc
