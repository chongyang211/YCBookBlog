//
// Created by 杨充 on 2026/6/10.
//
// ============================================================================
// 【文件定位】JsonWriter.cpp —— 序列化器实现（紧凑/美化 + 反向转义）
// ----------------------------------------------------------------------------
// 【与 Parser 的对照】Parser: peek/consume 游标前进；Writer: oss 输出累积。
//   两者都以"递归处理 Array/Object"为骨架 —— 一个把文本拆成树，一个把树
//   拼回文本。"解析→序列化"往返（round-trip）一致，是 JSON 库的基本验收。
// ============================================================================

#include "JsonWriter.h"


namespace jsonkv {

    // 唯一入口：重置状态 → 递归整树 → 导出字符串
    std::string JsonWriter::write(const JsonNode& root) {
        oss.str("");          // 重置缓冲区（复用同一 Writer 多次写时不残留上次内容）
        indent = 0;           // 回到第 0 层
        writeNode(root);      // 递归序列化（输出累积在成员 oss 里）
        return oss.str();     // 一次性导出
    }

    // 字符串序列化：与 Parser 的转义表互为"反向映射"
    // Parser 把 \n（两字符）变 '\n'（控制字符）——这里把 '\n' 变回 \n。
    // 【必要性】若不转义，字符串里的引号/换行会破坏 JSON 结构
    //   （如 "a"b" 解析回来就断了）—— 序列化必须保证输出可再解析。
    void JsonWriter::writeString(const std::string& s) {
        oss << '"';                       // 开引号
        for (char c : s) {                // 逐字符检查
            switch (c) {
                // 7 种控制字符/特殊字符 → 反向转义（与 parseString 的表一一对应）
                case '"':  oss << "\\\""; break;    // 引号：\"（否则提前终止字符串）
                case '\\': oss << "\\\\"; break;    // 反斜杠：\\（否则变成转义起始）
                case '\n': oss << "\\n";  break;    // 换行
                case '\t': oss << "\\t";  break;    // 制表符
                case '\r': oss << "\\r";  break;    // 回车
                case '\b': oss << "\\b";  break;    // 退格
                case '\f': oss << "\\f";  break;    // 换页
                default:   oss << c;      break;    // 普通字符原样输出
                // 【教学盲点】非 ASCII（如 UTF-8 中文）原样透传：合法但缺
                //   \uXXXX 转义能力（与 Parser 的挑战 B 同一根短板）。
            }
        }
        oss << '"';                       // 闭引号
    }

    // 递归核心：按 6 种类型分派
    void JsonWriter::writeNode(const JsonNode& n) {
        switch (n.type()) {
            case JsonNode::Type::Null:   oss << "null"; break;      // 字面量直接写
            case JsonNode::Type::Bool:   oss << (n.asBool() ? "true" : "false"); break;
            case JsonNode::Type::Number: oss << n.asNumber(); break;
            // 【已知精度损耗】double 经 operator<< 默认 6 位有效数字 ——
            //   3.1415926 存进去再写出来会变 3.14159。工程版需
            //   setprecision(std::numeric_limits<double>::max_digits10)。
            case JsonNode::Type::String: writeString(n.asString()); break;
            // case JsonNode::Type::Array:  oss << "[/* TODO */]"; break;        // 先占位
            // case JsonNode::Type::Object: oss << "{/* TODO */}"; break;        // 先占位
            // 【开发轨迹】Step 5.1 时 Array/Object 先占位跑通基本类型，
            //   Step 5.2 再补全递归 —— 教程"先立骨架再填肉"的节奏痕迹。
            case JsonNode::Type::Array: {
                const auto& arr = n.asArray();       // 只读引用拿到底层 vector
                if (arr.empty()) { oss << "[]"; break; }     // 空数组特判（紧凑无空格）
                oss << "[";
                indent += step;                       // 深入一层：缩进加宽
                for (size_t i = 0; i < arr.size(); ++i) {
                    if (pretty) { oss << "\n"; writeIndent(); }    // 美化：换行+缩进
                    writeNode(*arr[i]);                                  // ⭐ 递归：元素可以是任何类型
                    if (i + 1 < arr.size()) oss << ",";    // 元素间逗号（末元素后不加）
                }
                indent -= step;                       // 退出容器：缩进收回
                if (pretty) { oss << "\n"; writeIndent(); }    // 闭括号与开头对齐
                oss << "]";
                break;
            }
            case JsonNode::Type::Object: {
                const auto& obj = n.asObject();       // 只读引用拿到底层 map
                if (obj.empty()) { oss << "{}"; break; }     // 空对象特判
                oss << "{";
                indent += step;
                size_t i = 0;
                // 【技术原理】结构化绑定（C++17）：把 pair<const string, JsonNodePtr>
                //   解包成 k（键）与 v（子节点指针）—— 比 it->first/it->second 可读。
                //   map 按字典序遍历：输出键序稳定（JsonNode 选 map 的红利兑现）。
                for (const auto& [k, v] : obj) {                          // ⭐ 结构化绑定
                    if (pretty) { oss << "\n"; writeIndent(); }
                    writeString(k);                            // 键必须是 JSON 字符串（带引号+转义）
                    oss << (pretty ? ": " : ":");              // 美化模式冒号后带空格
                    writeNode(*v);                             // 值递归（任意类型）
                    if (++i < obj.size()) oss << ",";          // 逗号规则同数组
                }
                indent -= step;
                if (pretty) { oss << "\n"; writeIndent(); }
                oss << "}";
                break;
            }
        }
    }

}
