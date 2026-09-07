#include "mycc/lexer.h"
#include "mycc/error.h"

#include <cctype>
#include <unordered_map>

namespace Mycc {

std::vector<Token> Lexer::scanAll() {
    std::vector<Token> out;
    while (!isAtEnd()) {
        char c = peek();

        // 1. 跳过空白
        if (c == ' ' || c == '\t' || c == '\r') { advance(); continue; }
        if (c == '\n') { line++; advance(); continue; }

        // 2. 跳过单行注释
        if (c == '/' && pos + 1 < src.size() && src[pos + 1] == '/') {
            while (!isAtEnd() && peek() != '\n') advance();
            continue;
        }

        // 3. 数字
        if (std::isdigit(static_cast<unsigned char>(c))) {
            out.push_back(readNumber());
            continue;
        }

        // 4. 标识符与关键字
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            out.push_back(readIdent());
            continue;
        }

        // 5. 字符串
        if (c == '"') {
            out.push_back(readString());
            continue;
        }

        // 6. 单字符运算符与分隔符
        switch (c) {
            case '+': out.emplace_back(TokKind::Plus,  line); advance(); continue;
            case '-': out.emplace_back(TokKind::Minus, line); advance(); continue;
            case '*': out.emplace_back(TokKind::Star,  line); advance(); continue;
            case '/': out.emplace_back(TokKind::Slash, line); advance(); continue;
            case '%': out.emplace_back(TokKind::Percent,line);advance(); continue;
            case '(': out.emplace_back(TokKind::LParen,line); advance(); continue;
            case ')': out.emplace_back(TokKind::RParen,line); advance(); continue;
            case '{': out.emplace_back(TokKind::LBrace,line); advance(); continue;
            case '}': out.emplace_back(TokKind::RBrace,line); advance(); continue;
            case ';': out.emplace_back(TokKind::Semicolon,line);advance();continue;
            case ',': out.emplace_back(TokKind::Comma, line); advance(); continue;
            default: break;
        }

        // 7. 多字符运算符
        if (c == '=' || c == '!' || c == '<' || c == '>') {
            advance();   // 吃掉第一个字符
            char first = c;
            char second = peek();
            TokKind two;
            TokKind one;
            switch (first) {
                case '=': two = TokKind::EqEq;  one = TokKind::Assign; break;
                case '!': two = TokKind::BangEq; one = TokKind::Bang;  break;
                case '<': two = TokKind::Le;    one = TokKind::Lt;     break;
                case '>': two = TokKind::Ge;    one = TokKind::Gt;     break;
                default:  two = TokKind::Eof;   one = TokKind::Eof;    break;
            }
            if (second == '=') { advance(); out.emplace_back(two, line); }
            else               { out.emplace_back(one, line); }
            continue;
        }
        if (c == '&') {
            advance();
            if (peek() == '&') { advance(); out.emplace_back(TokKind::AndAnd, line); }
            else throw LexError("single '&' is not supported", line, file_);
            continue;
        }
        if (c == '|') {
            advance();
            if (peek() == '|') { advance(); out.emplace_back(TokKind::OrOr, line); }
            else throw LexError("single '|' is not supported", line, file_);
            continue;
        }

        // 8. 真正的未知字符
        throw LexError(std::string("unexpected character '") + c + "'", line, file_);
    }
    out.emplace_back(TokKind::Eof, line);
    return out;
}

Token Lexer::readNumber() {
    size_t start = pos;
    while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) advance();
    if (peek() == '.') {                  // 浮点
        advance();
        while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) advance();
    }
    double v = std::stod(src.substr(start, pos - start));
    return Token(TokKind::Number, v, line);
}

Token Lexer::readIdent() {
    size_t start = pos;
    while (!isAtEnd() && (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')) {
        advance();
    }
    std::string text = src.substr(start, pos - start);

    static const std::unordered_map<std::string, TokKind> KEYWORDS = {
        {"let",   TokKind::Let},
        {"print", TokKind::Print},
        {"if",    TokKind::If},     {"else",  TokKind::Else},
        {"while", TokKind::While},
        {"fn",    TokKind::Fn},     {"return",TokKind::Return},
        {"true",  TokKind::True},   {"false", TokKind::False},
    };
    if (auto it = KEYWORDS.find(text); it != KEYWORDS.end()) {
        return Token(it->second, line);
    }
    return Token(TokKind::Ident, text, line);
}

Token Lexer::readString() {
    advance();    // 吃掉开头的 "
    std::string s;
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\n') line++;
        s.push_back(advance());
    }
    if (isAtEnd()) {
        throw LexError("unterminated string", line, file_);
    }
    advance();    // 吃掉结尾的 "
    return Token(TokKind::String, s, line);
}

}  // namespace Mycc
