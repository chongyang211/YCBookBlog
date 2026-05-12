// mkv/errors.hpp —— KV 存储引擎的异常体系
//
// 设计原则（详见 12.迷你KV存储引擎.md §07）：
// 1. 所有自家异常都派生自 KvError，外层只需 catch(const KvError&) 就能兜住；
// 2. 区分"用户错误"(CmdSyntaxError/TypeError) 与"系统错误"(IoError/AofCorrupted)；
// 3. 用 `using BaseClass::BaseClass` 一行继承全部构造函数（C++11 起）。

#pragma once

#include <stdexcept>
#include <string>
#include <cstddef>

namespace mkv {

// 所有 KV 内部异常的根
class KvError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// 命令语法错误（用户输入问题）
class CmdSyntaxError : public KvError {
public:
    using KvError::KvError;
};

// 类型不匹配（如 GET 返回 string 但调用方按 int 用）
class TypeError : public KvError {
public:
    using KvError::KvError;
};

// IO 错误（AOF 写盘失败、目录创建失败）
class IoError : public KvError {
public:
    using KvError::KvError;
};

// AOF 重放时遇到坏行
class AofCorrupted : public KvError {
public:
    AofCorrupted(std::size_t lineNo, const std::string& detail)
        : KvError("AOF corrupted at line " + std::to_string(lineNo) + ": " + detail) {}
};

}  // namespace mkv
