#pragma once

#include "ast.h"
#include "token.h"
#include <vector>

namespace Mycc {

class Parser {
public:
    Parser(std::vector<Token> tokens, std::string filename = "<repl>")
        : toks(std::move(tokens)), file_(std::move(filename)) {}

    std::shared_ptr<Program> parseProgram();   // 主入口：解析整个文件

private:
    std::vector<Token> toks;
    std::string file_;
    size_t             pos = 0;

    const Token& peek()    const { return toks[pos]; }
    const Token& advance()       { return toks[pos++]; }
    bool         check(TokKind k) const { return peek().kind == k; }
    bool         match(TokKind k) {
        if (check(k)) { advance(); return true; }
        return false;
    }
    const Token& expect(TokKind k, const char* what);

    // 表达式（按优先级分层）
    AstPtr parseExpression();      // 最低优先级入口
    AstPtr parseAssignment();      // 等号赋值（右结合）
    AstPtr parseLogicOr();         // ||
    AstPtr parseLogicAnd();        // &&
    AstPtr parseEquality();        // == !=
    AstPtr parseComparison();      // < <= > >=
    AstPtr parseAddition();        // + -
    AstPtr parseMultiplication();  // * / %
    AstPtr parseUnary();           // - !
    AstPtr parseCall();            // 函数调用 后缀
    AstPtr parsePrimary();         // 字面量/变量/括号

    // 语句
    AstPtr parseStatement();
    AstPtr parseLetDecl();
    AstPtr parsePrintStmt();
    AstPtr parseIfStmt();
    AstPtr parseWhileStmt();
    AstPtr parseBlock();
    AstPtr parseReturnStmt();
    AstPtr parseFnDecl();
    AstPtr parseExprStmt();
};

}  // namespace Mycc
