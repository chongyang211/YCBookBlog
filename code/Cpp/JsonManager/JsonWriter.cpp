//
// Created by 杨充 on 2026/6/10.
//

#include "JsonWriter.h"


namespace jsonkv {

    std::string JsonWriter::write(const JsonNode& root) {
        oss.str("");          // 重置缓冲区
        indent = 0;
        writeNode(root);
        return oss.str();
    }

    void JsonWriter::writeString(const std::string& s) {
        oss << '"';
        for (char c : s) {
            switch (c) {
                case '"':  oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\n': oss << "\\n";  break;
                case '\t': oss << "\\t";  break;
                case '\r': oss << "\\r";  break;
                case '\b': oss << "\\b";  break;
                case '\f': oss << "\\f";  break;
                default:   oss << c;      break;
            }
        }
        oss << '"';
    }

    void JsonWriter::writeNode(const JsonNode& n) {
        switch (n.type()) {
            case JsonNode::Type::Null:   oss << "null"; break;
            case JsonNode::Type::Bool:   oss << (n.asBool() ? "true" : "false"); break;
            case JsonNode::Type::Number: oss << n.asNumber(); break;
            case JsonNode::Type::String: writeString(n.asString()); break;
            // case JsonNode::Type::Array:  oss << "[/* TODO */]"; break;        // 先占位
            // case JsonNode::Type::Object: oss << "{/* TODO */}"; break;        // 先占位
            case JsonNode::Type::Array: {
                const auto& arr = n.asArray();
                if (arr.empty()) { oss << "[]"; break; }
                oss << "[";
                indent += step;
                for (size_t i = 0; i < arr.size(); ++i) {
                    if (pretty) { oss << "\n"; writeIndent(); }
                    writeNode(*arr[i]);                                  // ⭐ 递归
                    if (i + 1 < arr.size()) oss << ",";
                }
                indent -= step;
                if (pretty) { oss << "\n"; writeIndent(); }
                oss << "]";
                break;
            }
            case JsonNode::Type::Object: {
                const auto& obj = n.asObject();
                if (obj.empty()) { oss << "{}"; break; }
                oss << "{";
                indent += step;
                size_t i = 0;
                for (const auto& [k, v] : obj) {                          // ⭐ 结构化绑定
                    if (pretty) { oss << "\n"; writeIndent(); }
                    writeString(k);
                    oss << (pretty ? ": " : ":");
                    writeNode(*v);
                    if (++i < obj.size()) oss << ",";
                }
                indent -= step;
                if (pretty) { oss << "\n"; writeIndent(); }
                oss << "}";
                break;
            }
        }
    }

}