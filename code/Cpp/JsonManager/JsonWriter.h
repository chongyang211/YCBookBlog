//
// Created by 杨充 on 2026/6/10.
//

#pragma once
#include "JsonNode.h"
#include <string>
#include <sstream>

namespace jsonkv {

    class JsonWriter {
    private:
        std::ostringstream oss;
        int indent = 0;        // 当前缩进深度
        int step;              // 每级缩进多少空格（0 = 紧凑）
        bool pretty;           // step > 0 即 pretty=true

        void writeIndent() { for (int i = 0; i < indent; ++i) oss << ' '; }
        void writeNode(const JsonNode& n);
        void writeString(const std::string& s);

    public:
        explicit JsonWriter(int spaces = 2) : step(spaces), pretty(spaces > 0) {}
        std::string write(const JsonNode& root);
    };

}

