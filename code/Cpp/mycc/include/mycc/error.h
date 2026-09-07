#pragma once

#include <stdexcept>
#include <string>

namespace mycc {

// ============================================================
//  根异常：所有 mycc 的诊断都从它派生
//  携带「行号 + 文件名」诊断信息
// ============================================================
class MyccError : public std::runtime_error {
public:
    MyccError(const std::string& stage,
              const std::string& msg,
              int line = 0,
              std::string file = "<repl>")
        : std::runtime_error(format(stage, msg, line, file))
        , line_(line)
        , file_(std::move(file)) {}

    int                line() const { return line_; }
    const std::string& file() const { return file_; }

    // 子类用来上报退出码（main 用）
    virtual int exitCode() const = 0;

private:
    static std::string format(const std::string& stage,
                              const std::string& msg,
                              int line,
                              const std::string& file) {
        std::string r = "[" + stage + "] ";
        if (!file.empty() && file != "<repl>") r += file + ":";
        if (line > 0) r += "line " + std::to_string(line) + ": ";
        r += msg;
        return r;
    }

    int         line_;
    std::string file_;
};

// ============================================================
//  四个阶段各一种
//  退出码沿用 Unix 惯例：2=词法/语法，3=类型/语义，4=运行时
// ============================================================
class LexError : public MyccError {
public:
    LexError(const std::string& msg, int line, std::string file = "<repl>")
        : MyccError("Lex", msg, line, std::move(file)) {}
    int exitCode() const override { return 2; }
};

class ParseError : public MyccError {
public:
    ParseError(const std::string& msg, int line, std::string file = "<repl>")
        : MyccError("Parse", msg, line, std::move(file)) {}
    int exitCode() const override { return 2; }
};

class TypeError : public MyccError {
public:
    TypeError(const std::string& msg, int line, std::string file = "<repl>")
        : MyccError("Type", msg, line, std::move(file)) {}
    int exitCode() const override { return 3; }
};

class RuntimeError : public MyccError {
public:
    RuntimeError(const std::string& msg, int line, std::string file = "<repl>")
        : MyccError("Runtime", msg, line, std::move(file)) {}
    int exitCode() const override { return 4; }
};

}  // namespace mycc
