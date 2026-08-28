//
// Created by 杨充 on 2026/6/4.
//

#include "JsonNode.h"


namespace jsonkv {

// 工具：枚举转字符串（用于错误消息）
static std::string typeName(JsonNode::Type t) {
    switch (t) {
        case JsonNode::Type::Null:   return "null";
        case JsonNode::Type::Bool:   return "bool";
        case JsonNode::Type::Number: return "number";
        case JsonNode::Type::String: return "string";
        case JsonNode::Type::Array:  return "array";
        case JsonNode::Type::Object: return "object";
    }
    return "unknown";
}


bool JsonNode::asBool() const {
    if (!isBool()) throw TypeMismatchError("bool", typeName(type()));    // ⭐ 先校验
    return std::get<bool>(v);
}

double JsonNode::asNumber() const {
    if (!isNumber()) throw TypeMismatchError("number", typeName(type()));
    return std::get<double>(v);
}

const std::string& JsonNode::asString() const {
    if (!isString()) throw TypeMismatchError("string", typeName(type()));
    return std::get<std::string>(v);
}

const JsonNode::Array& JsonNode::asArray() const {
    if (!isArray()) throw TypeMismatchError("array", typeName(type()));
    return std::get<Array>(v);
}

const JsonNode::Object& JsonNode::asObject() const {
    if (!isObject()) throw TypeMismatchError("object", typeName(type()));
    return std::get<Object>(v);
}

// 可变版本：复用 const 版本的检查逻辑
JsonNode::Array&  JsonNode::asArray()  {
    return const_cast<Array&>(static_cast<const JsonNode*>(this)->asArray());
}
JsonNode::Object& JsonNode::asObject() {
    return const_cast<Object&>(static_cast<const JsonNode*>(this)->asObject());
}

JsonNode& JsonNode::operator[](size_t idx) {
    if (!isArray()) {
        throw TypeMismatchError("array", typeName(type()));
    }
    auto& arr = std::get<Array>(v);
    if (idx >= arr.size()) {
        throw std::out_of_range("array index out of range");
    }
    return *arr[idx];
}

JsonNode& JsonNode::operator[](const std::string& key) {
    if (!isObject()) {
        throw TypeMismatchError("object", typeName(type()));
    }
    auto& obj = std::get<Object>(v);
    auto it = obj.find(key);
    if (it == obj.end()) {
        // 方案A：抛异常
        throw KeyNotFoundError(key);
        // 方案B：自动插入（类似 nlohmann/json 的行为）
        // auto [newIt, _] = obj.emplace(key, makeNull());
        // return *newIt->second;
    }
    return *it->second;
}

size_t JsonNode::size() const {
    if (isArray())  return std::get<Array>(v).size();
    if (isObject()) return std::get<Object>(v).size();
    if (isBool()) return 1;
    if (isString()) return std::get<std::string>(v).size();
    return 0;
}

bool JsonNode::contains(const std::string& key) const {
    return isObject() && std::get<Object>(v).count(key) > 0;
}

// 路径解析：递归查找 a.b.c
JsonNode& JsonNode::at(const std::string& path) {
    if (path.empty() || !isObject()) return *this;
    size_t dot = path.find('.');
    std::string head = (dot == std::string::npos) ? path : path.substr(0, dot);
    std::string rest = (dot == std::string::npos) ? "" : path.substr(dot + 1);

    auto& obj = std::get<Object>(v);
    auto it = obj.find(head);
    if (it == obj.end()) throw KeyNotFoundError(head);
    if (rest.empty()) return *it->second;
    return it->second->at(rest);             // ⭐ 递归
}

const JsonNode& JsonNode::at(const std::string& path) const {
    return const_cast<JsonNode*>(this)->at(path);
}

}