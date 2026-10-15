#pragma once

#include <stdexcept>    // std::runtime_error：标准运行时异常基类
#include <string>       // std::string / std::to_string

namespace Mycc {        // Mycc = Mini-ycc：本项目命名空间（教程全篇的 mycc 家族符号都在这）

// ============================================================
//  根异常：所有 mycc 的诊断都从它派生
//  携带「行号 + 文件名」诊断信息
// ============================================================
// 【设计思想·教程阶段⑧】四级异常类树对应编译器的四个阶段：
//   词法(Lex) → 语法(Parse) → 类型(Type) → 运行时(Runtime) ——
//   每个阶段只抛自己那一级，main 统一 catch 基类并按 exitCode 决定退出码。
// 【与之前的 JsonExceptions/MiniKV errors 同款套路】
//   单根树 + 继承构造函数 + 诊断信息在"构造时"就拼好 ——
//   区别在于本类更进一步：退出码也进异常（虚拟函数）——
//   "错误该以什么状态码退出"是错误自己的属性，不该让 main 逐个 instanceof 判断。
class MyccError : public std::runtime_error {
public:
    MyccError(const std::string& stage,
              const std::string& msg,
              int line = 0,
              std::string file = "<repl>")
        : std::runtime_error(format(stage, msg, line, file))
        , line_(line)
        , file_(std::move(file)) {}
    // 【初始化列表的分工】先调 format 把"[阶段] 文件:line N: 消息"拼成
    //   完整文本交给 runtime_error 基类（what() 的来源），行号/文件名再
    //   存一份成本员 —— what() 给人看，line()/file() 给程序用（比如测试断言）

    int                line() const { return line_; }
    const std::string& file() const { return file_; }

    // 子类用来上报退出码（main 用）
    virtual int exitCode() const = 0;
    // 【纯虚设计】强制每个阶段子类"回答"自己的退出码 ——
    //   main 只写 e.exitCode() 一行，新增异常类型忘实现会编译报错

private:
    // 【static 私有工具】只服务于构造函数的字符串拼装，不碰成员（静态合理）
    static std::string format(const std::string& stage,
                              const std::string& msg,
                              int line,
                              const std::string& file) {
        std::string r = "[" + stage + "] ";
        // 前缀格式：[Lex] / [Parse] / [Type] / [Runtime] —— 一眼认出阶段
        if (!file.empty() && file != "<repl>") r += file + ":";
        // REPL 模式没有真实文件名（默认 "<repl>"）—— 诊断里省去文件段，
        //   文件模式才带文件名（"example/fib.mycc:"）
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
// 【Unix 退出码惯例】0=成功、1=通用错误（main 的用法错误在用）、
//   2+ 按错误类别细分 —— shell 脚本能凭 $? 做不同处理。
//   词法/语法共享 2（都是"源码写错了"，修源码同一类）、
//   类型单独 3（语义层）、运行时单独 4（源码对但执行出错，如除零）。
class LexError : public MyccError {
public:
    LexError(const std::string& msg, int line, std::string file = "<repl>")
        : MyccError("Lex", msg, line, std::move(file)) {}
    // 阶段名写死在子类构造里 —— 调用方只报"什么错"，不报"我是哪个阶段"
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

}  // namespace Mycc
