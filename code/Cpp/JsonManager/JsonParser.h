//
// Created by 杨充 on 2026/6/5.
//

#pragma once
#include "JsonNode.h"
#include <string>
#include <string_view>

namespace jsonkv {

class JsonParser {
private:
    std::string_view src;   // 待解析的 JSON 源字符串（轻量引用，不拷贝）
    size_t pos = 0;         // 当前解析到的位置（索引）
    size_t line = 1, col = 1; // 当前行号和列号（用于报错时定位）

    // ===== 游标工具 =====
    char   peek() const;         // 看当前字符（不前进）
    char   consume();            // 吃掉当前字符并前进
    bool   match(char c);        // 当前字符如果是 c 就吃掉
    void   skipWhitespace();
    [[noreturn]] void error(const std::string& msg);

    // 字面量解析
    JsonNodePtr parseValue();
    JsonNodePtr parseLiteral();
public:
    explicit JsonParser(std::string_view s) : src(s) {}
    // 调试用：打印源串信息
    void debugDump() const;
    JsonNodePtr parse();
    JsonNodePtr parseNumber();
    JsonNodePtr parseString();
    JsonNodePtr parseArray();
    JsonNodePtr parseObject();
};

}

