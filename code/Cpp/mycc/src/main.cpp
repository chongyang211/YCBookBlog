// ============================================================================
// 【文件定位】main.cpp —— 入口层：编译管线 + 文件模式 + REPL（教程阶段⑧收官）
// ----------------------------------------------------------------------------
// 【设计思路】main 是"五段式的总装配车间"：
//   compileSource 串起 Lexer→Parser→TypeChecker→Codegen（一条函数到底）；
//   run/dump/typecheck 是三个"观察窗口"（执行/看字节码/只检查）；
//   REPL 把三个窗口包成命令循环 —— 单条命令失败只报错继续（错误局部化）。
// ============================================================================
#include "mycc/error.h"
#include "mycc/lexer.h"
#include "mycc/parser.h"
#include "mycc/type_checker.h"
#include "mycc/codegen.h"
#include "mycc/vm.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

using namespace Mycc;

// 一次编译的产物：主 chunk + 函数表
struct Compiled {
    Chunk top;
    std::unordered_map<std::string, Chunk> functions;
};
// 【产物聚合】"一个程序 = 主指令流 + 一堆函数指令流" ——
// Codegen 的两份产出（compile 返回值 + functions 成员）打包一起走

// 完整编译管线：源码 → Token → AST → 类型检查 → 字节码（可能抛 MyccError）
static Compiled compileSource(const std::string& src, const std::string& filename) {
    Lexer lex(src, filename);
    auto toks = lex.scanAll();       // 第一段：字符流 → Token 流

    Parser psr(std::move(toks), filename);
    auto prog = psr.parseProgram();  // 第二段：Token 流 → AST

    TypeChecker tc(filename);
    tc.check(prog);                   // 第三段：AST → 标注（查语义错误）

    Codegen cg;
    Chunk top = cg.compile(*prog);   // 第四段：AST → 字节码
    return Compiled{ std::move(top), std::move(cg.functions) };
    // 【管线串联的形状】每段的输出恰好是下一段的输入 ——
    // 各段独立可测（REPL 的 tcheck 就只取前三段）
}

// 编译并执行（第五段：VM）
static int run(const std::string& src, const std::string& filename) {
    try {
        auto compiled = compileSource(src, filename);
        VM vm;
        vm.load(std::move(compiled.top), std::move(compiled.functions));
        vm.run();
        return 0;
    } catch (const MyccError& e) {
        std::cerr << e.what() << "\n";
        return e.exitCode();
        // 【退出码的兑现处】四级异常在 error.h 里各定 exitCode ——
        // main 一行转发，shell 脚本凭 $? 分诊（Unix 惯例闭环）
    } catch (const std::exception& e) {
        std::cerr << "[Internal] " << e.what() << "\n";
        return 99;
        // 编译器自身的 bug（"too many constants" 等内部错误）——
        // 与"用户源码错误"严格区分（99 = 兜底段）
    }
}

// 编译并反汇编（dump 命令：观察第四段的产物，不执行第五段）
static int dump(const std::string& src, const std::string& filename) {
    try {
        auto compiled = compileSource(src, filename);
        std::cout << "[DUMP main]\n";
        compiled.top.disassemble(std::cout);
        for (auto& [name, c] : compiled.functions) {
            std::cout << "[DUMP fn " << name << "]\n";
            c.disassemble(std::cout);
        }
        // 主 chunk + 逐函数 chunk 全量反汇编 ——
        // "let a = 1 + 2;" 能看到 CONST/ADD/STORE_GLOBAL 的真身（教学利器）
        return 0;
    } catch (const MyccError& e) {
        std::cerr << e.what() << "\n";
        return e.exitCode();
    } catch (const std::exception& e) {
        std::cerr << "[Internal] " << e.what() << "\n";
        return 99;
    }
}

// 仅类型检查（tcheck 命令：只跑前三段）
static int typecheck(const std::string& src, const std::string& filename) {
    try {
        Lexer lex(src, filename);
        auto toks = lex.scanAll();
        Parser psr(std::move(toks), filename);
        auto prog = psr.parseProgram();
        TypeChecker tc(filename);
        tc.check(prog);
        std::cout << "[TypeCheck] OK\n";
        return 0;
        // print 1 + "abc" 在这里报 [Type] arithmetic requires Num ——
        // REPL 教学演示"编译期就能拦住的错误"
    } catch (const MyccError& e) {
        std::cerr << e.what() << "\n";
        return e.exitCode();
    } catch (const std::exception& e) {
        std::cerr << "[Internal] " << e.what() << "\n";
        return 99;
    }
}

// 文件模式：读整个文件 → 走 run
static int runFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        std::cerr << "[IO] cannot open: " << path << "\n";
        return 1;
        // 通用 IO 错误码 1（与源码错误 2/3/4 区分 —— 谱系见 error.h）
    }
    std::stringstream ss;
    ss << in.rdbuf();       // 整文件灌入（rdbuf 零拷贝转接，保留换行）
    return run(ss.str(), path);
    // 文件名随源码下传 —— 异常里的 "examples/bad.mycc:" 定位即源于此
}

// REPL 模式：读一行 → 命令分派或当作源码执行 → 循环
static int runRepl() {
    std::cout << "Mycc 0.1 — 输入 h 查看帮助，q 退出\n";
    std::string line;
    while (true) {
        std::cout << "Mycc> " << std::flush;      // 提示符 + 冲刷（立即上屏）
        if (!std::getline(std::cin, line)) break;  // Ctrl-D 优雅退出
        if (line.empty()) continue;               // 空行静默跳过

        // 提取命令名和剩余部分
        std::string cmd = line, rest;
        if (auto sp = line.find(' '); sp != std::string::npos) {
            cmd  = line.substr(0, sp);       // 第一个空格前 = 命令
            rest = line.substr(sp + 1);     // 之后全部 = 参数/源码
        }
        // 去掉冒号前缀，:q 和 q 等价
        if (!cmd.empty() && cmd[0] == ':') cmd = cmd.substr(1);
        // 【: 前缀惯例】向 Scheme/Lisp 的 REPL 致敬 —— 冒号命令与普通源码
        //   混排时仍可区分（let 不是命令，因为不在命令表里）

        if (cmd == "q" || cmd == "quit" || cmd == "exit") break;
        // 退出三别名：最短可用性（q）+ 全词（quit/exit）
        if (cmd == "h" || cmd == "help") {
            std::cout << "  run <code>    编译并执行\n"
                         "  dump <code>   反汇编字节码\n"
                         "  tcheck <code> 仅类型检查\n"
                         "  q / quit      退出\n"
                         "  h / help      帮助\n"
                         "  其他输入       当作 Mycc 源码执行\n";
            continue;
        }
        if (cmd == "dump")   { dump(rest, "<repl>"); continue; }
        if (cmd == "tcheck") { typecheck(rest, "<repl>"); continue; }
        if (cmd == "run")    { run(rest, "<repl>"); continue; }

        // 其他：当作源码执行
        run(line, "<repl>");
        // 【教学版 REPL 的已知局限】每行都是"独立编译 + 全新 VM"——
        //   前一行 let 的全局变量在下一行查不到（每行一个新世界）。
        //   需要跨行状态时用 run 把多条语句放一行：
        //   run fn fib(n){...} print fib(10);   ← README 示例正是这么写的。
        //   修复方向：VM/globals 提到循环外复用 + 增量编译（教学挑战题）
    }
    return 0;
}

// 入口分派：无参数 REPL / 有参数文件模式
int main(int argc, char** argv) {
    if (argc == 1) return runRepl();
    if (argc == 2) return runFile(argv[1]);
    std::cerr << "Usage: Mycc [file.mycc]\n";
    return 1;
    // 最朴素的命令行处理：不引参数解析库（同 MiniKV 的取舍）
}
