//
// Created by 杨充 on 2026/6/4.
//

#pragma once
#include <stdexcept>
#include <string>

namespace jsonkv {

// class JsonError : public std::runtime_error {
// public:
//     // 手动写每个构造函数。即使基类有多个构造函数，你也需要在派生类中手动重写每个构造函数。
//     JsonError() : std::runtime_error("") {}
//     JsonError(const std::string& msg) : std::runtime_error(msg) {}
//     JsonError(const char* msg) : std::runtime_error(msg) {}
//     // ... 还有其他重载吗？需要去查 std::runtime_error 的文档
// };

// 顶层基类：所有 jsonkv 自定义异常的根
// 定义了一个自定义异常类 JsonError，它继承自标准库的 std::runtime_error。
class JsonError : public std::runtime_error {
public:
    // ⭐ using 继承构造函数的作用，C++11 引入了继承构造函数特性，允许派生类直接"继承"基类的所有构造函数。
    using std::runtime_error::runtime_error;
};


class JsonParseError: public JsonError {
public:
    JsonParseError(const std::string& msg, size_t line, size_t col)
        : JsonError("[Parse] line " + std::to_string(line) +
                    " col " + std::to_string(col) + ": " + msg) {}
};

// 类型不匹配（如对 number 调 asString()）
class TypeMismatchError : public JsonError {
public:
    TypeMismatchError(const std::string& expected, const std::string& actual)
        : JsonError("[Type] expected " + expected + ", got " + actual) {}
};

// Key 不存在
class KeyNotFoundError : public JsonError {
public:
    explicit KeyNotFoundError(const std::string& key)
        : JsonError("[Key] not found: " + key) {}
};

// IO 错误（文件读写）
class JsonIoError : public JsonError {
public:
    using JsonError::JsonError;
};

}





