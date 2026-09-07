#pragma once

#include <stdexcept>
#include <string>

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
