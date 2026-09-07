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

using namespace mycc;

// 一次编译的产物：主 chunk + 函数表
struct Compiled {
    Chunk top;
    std::unordered_map<std::string, Chunk> functions;
};

// 完整编译管线：源码 → Token → AST → 类型检查 → 字节码（可能抛 MyccError）
static Compiled compileSource(const std::string& src, const std::string& filename) {
    Lexer lex(src, filename);
    auto toks = lex.scanAll();

    Parser psr(std::move(toks), filename);
    auto prog = psr.parseProgram();

    TypeChecker tc(filename);
    tc.check(prog);

    Codegen cg;
    Chunk top = cg.compile(*prog);
    return Compiled{ std::move(top), std::move(cg.functions) };
}

// 编译并执行
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
    } catch (const std::exception& e) {
        std::cerr << "[Internal] " << e.what() << "\n";
        return 99;
    }
}

// 编译并反汇编
static int dump(const std::string& src, const std::string& filename) {
    try {
        auto compiled = compileSource(src, filename);
        std::cout << "[DUMP main]\n";
        compiled.top.disassemble(std::cout);
        for (auto& [name, c] : compiled.functions) {
            std::cout << "[DUMP fn " << name << "]\n";
            c.disassemble(std::cout);
        }
        return 0;
    } catch (const MyccError& e) {
        std::cerr << e.what() << "\n";
        return e.exitCode();
    } catch (const std::exception& e) {
        std::cerr << "[Internal] " << e.what() << "\n";
        return 99;
    }
}

// 仅类型检查
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
    } catch (const MyccError& e) {
        std::cerr << e.what() << "\n";
        return e.exitCode();
    } catch (const std::exception& e) {
        std::cerr << "[Internal] " << e.what() << "\n";
        return 99;
    }
}

// 文件模式
static int runFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        std::cerr << "[IO] cannot open: " << path << "\n";
        return 1;
    }
    std::stringstream ss;
    ss << in.rdbuf();
    return run(ss.str(), path);
}

// REPL 模式
static int runRepl() {
    std::cout << "mycc 0.1 — :h for help, :q to quit\n";
    std::string line;
    while (true) {
        std::cout << "mycc> " << std::flush;
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;

        std::string cmd = line, rest;
        if (auto sp = line.find(' '); sp != std::string::npos) {
            cmd  = line.substr(0, sp);
            rest = line.substr(sp + 1);
        }

        if (cmd == ":q") break;
        if (cmd == ":h") {
            std::cout << "  :run    <code>  编译并执行\n"
                         "  :dump   <code>  显示字节码\n"
                         "  :tcheck <code>  仅类型检查\n"
                         "  :q              退出\n"
                         "  直接输入代码     等价于 :run\n";
            continue;
        }
        if (cmd == ":dump")   { dump(rest, "<repl>"); continue; }
        if (cmd == ":tcheck") { typecheck(rest, "<repl>"); continue; }

        // :run 或普通代码
        std::string code = (cmd[0] == ':') ? rest : line;
        run(code, "<repl>");
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc == 1) return runRepl();
    if (argc == 2) return runFile(argv[1]);
    std::cerr << "Usage: mycc [file.mycc]\n";
    return 1;
}
