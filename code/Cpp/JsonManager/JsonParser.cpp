//
// Created by 杨充 on 2026/6/5.
//

#include "JsonParser.h"
#include <cctype>
#include <iostream>

namespace jsonkv {

char JsonParser::peek() const {
    // 遇到报错：JsonParser.cpp:12:29: error: incompatible operand types ('char' and 'const char *')
    // '\0' 是字符字面量（类型 char），而 "\0" 是字符串字面量（类型 const char[2]，退化为 const char*）。
    return pos < src.size() ? src[pos] : '\0';
}

char JsonParser::consume() {
    char c = src[pos++];
    if (c == '\n') {
        // 跨行就更新行号
        line++;
        col = 1;
    }
    else { col++; }
    return c;
}

bool JsonParser::match(char c) {
    if (peek() == c) { consume(); return true; }
    return false;
}

void JsonParser::skipWhitespace() {
    while (pos < src.size() && std::isspace(static_cast<unsigned char>(src[pos]))) {
        consume();
    }
}

void JsonParser::error(const std::string& msg) {
    throw JsonParseError(msg, line, col);
}

void JsonParser::debugDump() const{
    std::cout << "src len=" << src.size()
          << " pos="    << pos
          << " line="   << line
          << " col="    << col << "\n";
}

JsonNodePtr JsonParser::parse() {
    skipWhitespace();
    auto node = parseValue();
    skipWhitespace();
    if (pos < src.size()) error("trailing characters after value");
    return node;
}

JsonNodePtr JsonParser::parseNumber() {
    size_t start = pos;
    if (match('-')){}
    while (std::isdigit((static_cast<unsigned char>(peek())))) consume();
    if (peek() == '.') {                                                       // 小数部分
        consume();
        while (std::isdigit(static_cast<unsigned char>(peek()))) consume();
    }
    if (peek() == 'e' || peek() == 'E') {                                      // 科学计数法
        consume();
        if (peek() == '+' || peek() == '-') consume();
        while (std::isdigit(static_cast<unsigned char>(peek()))) consume();
    }
    try {
        double v = std::stod(std::string(src.substr(start, pos - start)));
        return makeNumber(v);
    } catch (...) {
        error("invalid number");
    }
}

JsonNodePtr JsonParser::parseString() {
    if (!match('"')) error("expected '\"'");
    std::string result;
    while (pos < src.size() && peek() != '"') {
        char c = consume();
        if (c == '\\') {                                  // ⭐ 转义起始
            char esc = consume();
            switch (esc) {
                case '"':  result += '"';  break;
                case '\\': result += '\\'; break;
                case '/':  result += '/';  break;
                case 'n':  result += '\n'; break;
                case 't':  result += '\t'; break;
                case 'r':  result += '\r'; break;
                case 'b':  result += '\b'; break;
                case 'f':  result += '\f'; break;
                // \uXXXX Unicode 转义留作挑战 B
                default:   error(std::string("bad escape \\") + esc);
            }
        } else {
            result += c;
        }
    }
    if (!match('"')) error("unterminated string");
    return makeString(std::move(result));
}

JsonNodePtr JsonParser::parseArray() {
    if (!match('[')) error("expected '['");
    auto arr = JsonNode::Array{};
    skipWhitespace();
    if (match(']')) return std::make_unique<JsonNode>(std::move(arr));     // 空数组

    while (true) {
        arr.push_back(parseValue());                                       // ⭐ 递归
        skipWhitespace();
        if (match(']')) break;
        if (!match(',')) error("expected ',' or ']' in array");
    }
    return std::make_unique<JsonNode>(std::move(arr));
}

JsonNodePtr JsonParser::parseObject() {
    if (!match('{')) error("expected '{'");
    auto obj = JsonNode::Object{};
    skipWhitespace();
    if (match('}')) return std::make_unique<JsonNode>(std::move(obj));     // 空对象

    while (true) {
        skipWhitespace();
        auto keyNode = parseString();                                       // key 必须是字符串
        std::string key = keyNode->asString();
        skipWhitespace();
        if (!match(':')) error("expected ':'");
        auto value = parseValue();                                          // ⭐ 递归
        obj[key] = std::move(value);
        skipWhitespace();
        if (match('}')) break;
        if (!match(',')) error("expected ',' or '}' in object");
    }
    return std::make_unique<JsonNode>(std::move(obj));
}

JsonNodePtr JsonParser::parseValue() {
    skipWhitespace();
    char c = peek();
    if (c == 'n' || c == 't' || c == 'f') return parseLiteral();
    if (c == '"') return parseString();
    if (c == '[') return parseArray();                                                 // ⭐ 新增
    if (c == '{') return parseObject();                                                // ⭐ 新增
    if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parseNumber();
    error(std::string("unexpected char: ") + c);
}

JsonNodePtr JsonParser::parseLiteral() {
    if (src.compare(pos, 4, "null") == 0)  { pos += 4; col += 4; return makeNull(); }
    if (src.compare(pos, 4, "true") == 0)  { pos += 4; col += 4; return makeBool(true); }
    if (src.compare(pos, 5, "false") == 0) { pos += 5; col += 5; return makeBool(false); }
    error("invalid literal");
}


}

