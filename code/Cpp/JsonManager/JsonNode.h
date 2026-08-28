//
// Created by 杨充 on 2026/6/4.
//

#pragma once
#include <variant>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include "JsonExceptions.h"

namespace jsonkv {

// 前置声明
class JsonNode;
// 别名：节点智能指针
using JsonNodePtr = std::unique_ptr<JsonNode>;

class JsonNode {
public:
    // JSON 数组
    using Array = std::vector<JsonNodePtr>;
    // JSON 对象
    using Object = std::map<std::string, JsonNodePtr>;
    // ⭐ 核心：6 路 variant。6 种 JSON 类型
    // 使用 std::variant 替代传统 union 或继承体系
    using Value  = std::variant<
        std::nullptr_t,    // 0: null
        bool,              // 1: true/false
        double,            // 2: number
        std::string,       // 3: string
        Array,             // 4: array
        Object             // 5: object
    >;
    // 用 enum class 让"类型枚举"和 variant 索引一一对应
    // variant 索引：       0       1     2       3       4       5
    enum class Type { Null = 0, Bool, Number, String, Array, Object };

    // ====== 构造函数：每种 JSON 类型一个 ======
    JsonNode() : v(nullptr) {}                                   // null
    explicit JsonNode(bool b)                : v(b) {}
    explicit JsonNode(double n)              : v(n) {}
    explicit JsonNode(int n)                 : v(static_cast<double>(n)) {}   // int 转 double
    explicit JsonNode(const std::string& s)  : v(s) {}
    explicit JsonNode(std::string&& s)       : v(std::move(s)) {}             // 移动版
    explicit JsonNode(const char* s)         : v(std::string(s)) {}           // 字面量便利
    explicit JsonNode(Array&& a)             : v(std::move(a)) {}
    explicit JsonNode(Object&& o)            : v(std::move(o)) {}

    // ====== 类型查询 ======
    // ⭐ variant 的 index() 返回当前持有的索引
    Type type() const { return static_cast<Type>(v.index()); }
    bool isNull() const { return type() == Type::Null; }
    bool isBool()   const { return type() == Type::Bool; }
    bool isNumber() const { return type() == Type::Number; }
    bool isString() const { return type() == Type::String; }
    bool isArray()  const { return type() == Type::Array; }
    bool isObject() const { return type() == Type::Object; }

    // ====== 访问器（先裸版，下一步再升级）======
    bool                asBool()   const;
    double              asNumber() const;
    const std::string&  asString() const;
    const Array&        asArray()  const;
    const Object&       asObject() const;
    Array&  asArray();           // 可变版本
    Object& asObject();

    // ====== 容器便捷操作 ======
    JsonNode& operator[](size_t idx);                  // arr[0]
    JsonNode& operator[](const std::string& key);      // obj["name"]
    size_t size() const;
    bool   contains(const std::string& key) const;
    // 路径查询：obj.at("user.address.city")
    JsonNode& at(const std::string& path);
    const JsonNode& at(const std::string& path) const;

private:
    Value v;                // 持有 6 种之一。实际存储的值
};


// ====== 工厂函数（类外）======
inline JsonNodePtr makeNull()                  { return std::make_unique<JsonNode>(); }
inline JsonNodePtr makeBool(bool b)            { return std::make_unique<JsonNode>(b); }
inline JsonNodePtr makeNumber(double n)        { return std::make_unique<JsonNode>(n); }
inline JsonNodePtr makeString(std::string s)   { return std::make_unique<JsonNode>(std::move(s)); }
inline JsonNodePtr makeArray()                 { return std::make_unique<JsonNode>(JsonNode::Array{}); }
inline JsonNodePtr makeObject()                { return std::make_unique<JsonNode>(JsonNode::Object{}); }

}





