//
// Created by 杨充 on 2026/6/4.
//
// ============================================================================
// 【文件定位】JsonExceptions.h —— 异常体系（整个项目的"错误语言"底座）
// ----------------------------------------------------------------------------
// 【设计思路·为什么要自定义异常体系】
//   1. 精确表达"哪一种错"：解析错（带行列号）/ 类型不匹配 / Key 不存在 / IO 错
//      是四种语义完全不同的失败 —— 用裸 int 错误码或统一 runtime_error 都会
//      丢失"可分类捕获"的能力。调用方可以只 catch 关心的那一种。
//   2. 一棵单根树：所有异常都从 JsonError 派生 → 顶层 REPL 只需
//      catch (const JsonError&) 一把抓全部库内错误（多态捕获）。
//   3. 继承 std::runtime_error：与标准异常家族接轨 —— 忘了 catch JsonError
//      时仍可被 catch (const std::exception&) 接住，程序不至于直接 terminate。
// ============================================================================

#pragma once        // 头文件保护（编译器内置指令，等价 #ifndef 三件套）
#include <stdexcept>    // std::runtime_error：标准运行时异常基类
#include <string>       // std::string：构造错误消息用

namespace jsonkv {  // 命名空间：库内符号隔离，避免与其他库的同名类冲突

// 【历史反例·手写继承构造函数的痛苦】下面注释掉的写法要为基类的每一个
// 构造函数重载都手写一遍转发 —— 而且永远不确定"还有没有漏掉的"。
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
    // 【技术原理】"using B::B;" 一行等价于为基类每个构造函数生成一个
    //   同签名转发版本 —— 基类新增重载时派生类自动跟上，零维护成本。
    //   runtime_error 内部持有一个引用计数的字符串（what() 的来源）。
    using std::runtime_error::runtime_error;
};

// json解析异常
// 【设计要点】携带"行号+列号"：解析器发现语法错误时用户最想知道的是
//   "错在第几行第几列" —— 错误消息里定位信息比描述文本更有价值。
class JsonParseError: public JsonError {
public:
    // 构造即拼装统一格式的消息："[Parse] line 3 col 12: bad token"
    // std::to_string 把 size_t 转十进制字符串（C++11）
    JsonParseError(const std::string& msg, size_t line, size_t col)
        : JsonError("[Parse] line " + std::to_string(line) +
                    " col " + std::to_string(col) + ": " + msg) {}
};

// 类型不匹配（如对 number 调 asString()）
// 【设计要点】消息带"期望 vs 实际"双侧信息：调用方一眼看出"我以为它是
//   string，其实它是 number" —— 比 "type error" 这种模糊话术高效得多。
class TypeMismatchError : public JsonError {
public:
    TypeMismatchError(const std::string& expected, const std::string& actual)
        : JsonError("[Type] expected " + expected + ", got " + actual) {}
};

// Key 不存在
// 【设计要点】消息带具体 key 名：at("user.name") 失败时明确告知是哪个键没找到
class KeyNotFoundError : public JsonError {
public:
    explicit KeyNotFoundError(const std::string& key)   // explicit：单参构造防隐式转换
        : JsonError("[Key] not found: " + key) {}
};

// IO 错误（文件读写）
// 【设计要点】无自定义构造 → 用"继承构造函数"原样继承 JsonError 的
//   构造集合 —— 异常类与基类只差"类型名"时的最省写法。
class JsonIoError : public JsonError {
public:
    using JsonError::JsonError;     // 一行继承全部构造（同基类的手法）
};

}
