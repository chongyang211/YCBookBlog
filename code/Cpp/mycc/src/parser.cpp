#include "mycc/parser.h"
#include "mycc/error.h"

#include <memory>
#include <string>

namespace mycc {

const Token& Parser::expect(TokKind k, const char* what) {
    if (!check(k)) {
        throw ParseError(std::string("expected ") + what + " but got " + tokKindName(peek().kind),
                         peek().line, file_);
    }
    return advance();
}

// ============== 顶层 ==============
std::shared_ptr<Program> Parser::parseProgram() {
    std::vector<AstPtr> stmts;
    int firstLine = check(TokKind::Eof) ? 1 : peek().line;
    while (!check(TokKind::Eof)) {
        stmts.push_back(parseStatement());
    }
    return std::make_shared<Program>(std::move(stmts), firstLine);
}

// ============== 语句 ==============
AstPtr Parser::parseStatement() {
    if (check(TokKind::Let))    return parseLetDecl();
    if (check(TokKind::Print))  return parsePrintStmt();
    if (check(TokKind::If))     return parseIfStmt();
    if (check(TokKind::While))  return parseWhileStmt();
    if (check(TokKind::Return)) return parseReturnStmt();
    if (check(TokKind::LBrace)) return parseBlock();
    if (check(TokKind::Fn))     return parseFnDecl();
    return parseExprStmt();
}

AstPtr Parser::parseLetDecl() {
    int ln = advance().line;             // 吃 let
    const auto& nameTok = expect(TokKind::Ident, "变量名");
    expect(TokKind::Assign, "=");
    AstPtr init = parseExpression();
    expect(TokKind::Semicolon, ";");
    return std::make_shared<LetDecl>(std::get<std::string>(nameTok.value), init, ln);
}

AstPtr Parser::parsePrintStmt() {
    int ln = advance().line;             // 吃 print
    auto e = parseExpression();
    expect(TokKind::Semicolon, ";");
    return std::make_shared<PrintStmt>(e, ln);
}

AstPtr Parser::parseIfStmt() {
    int ln = advance().line;             // 吃 if
    expect(TokKind::LParen, "(");
    auto cond = parseExpression();
    expect(TokKind::RParen, ")");
    auto thenBranch = parseStatement();
    AstPtr elseBranch = nullptr;
    if (match(TokKind::Else)) elseBranch = parseStatement();
    return std::make_shared<IfStmt>(cond, thenBranch, elseBranch, ln);
}

AstPtr Parser::parseWhileStmt() {
    int ln = advance().line;             // 吃 while
    expect(TokKind::LParen, "(");
    auto cond = parseExpression();
    expect(TokKind::RParen, ")");
    auto body = parseStatement();
    return std::make_shared<WhileStmt>(cond, body, ln);
}

AstPtr Parser::parseBlock() {
    int ln = advance().line;             // 吃 {
    std::vector<AstPtr> stmts;
    while (!check(TokKind::RBrace) && !check(TokKind::Eof)) {
        stmts.push_back(parseStatement());
    }
    expect(TokKind::RBrace, "}");
    return std::make_shared<BlockStmt>(std::move(stmts), ln);
}

AstPtr Parser::parseReturnStmt() {
    int ln = advance().line;             // 吃 return
    AstPtr v = nullptr;
    if (!check(TokKind::Semicolon)) v = parseExpression();
    expect(TokKind::Semicolon, ";");
    return std::make_shared<ReturnStmt>(v, ln);
}

AstPtr Parser::parseFnDecl() {
    int ln = advance().line;             // 吃 fn
    const auto& nameTok = expect(TokKind::Ident, "函数名");
    expect(TokKind::LParen, "(");

    std::vector<std::string> params;
    if (!check(TokKind::RParen)) {
        params.push_back(std::get<std::string>(expect(TokKind::Ident, "参数名").value));
        while (match(TokKind::Comma)) {
            params.push_back(std::get<std::string>(expect(TokKind::Ident, "参数名").value));
        }
    }
    expect(TokKind::RParen, ")");

    // 函数体：{ stmt* }，直接解析成语句列表
    expect(TokKind::LBrace, "{");
    std::vector<AstPtr> body;
    while (!check(TokKind::RBrace) && !check(TokKind::Eof)) {
        body.push_back(parseStatement());
    }
    expect(TokKind::RBrace, "}");
    return std::make_shared<FuncDecl>(std::get<std::string>(nameTok.value),
                                      std::move(params), std::move(body), ln);
}

AstPtr Parser::parseExprStmt() {
    auto e = parseExpression();
    expect(TokKind::Semicolon, ";");
    return std::make_shared<ExprStmt>(e, e->line);
}

// ============== 表达式（按优先级分层）==============
AstPtr Parser::parseExpression()    { return parseAssignment(); }

AstPtr Parser::parseAssignment() {
    auto lhs = parseLogicOr();
    if (check(TokKind::Assign)) {
        advance();
        auto rhs = parseAssignment();        // 右结合：a = b = 1 合法
        if (auto v = std::dynamic_pointer_cast<VarRef>(lhs)) {
            return std::make_shared<Assign>(v->name, rhs, lhs->line);
        }
        throw ParseError("assignment target must be a variable", lhs->line, file_);
    }
    return lhs;
}

AstPtr Parser::parseLogicOr() {
    auto lhs = parseLogicAnd();
    while (check(TokKind::OrOr)) {
        advance();
        auto rhs = parseLogicAnd();
        lhs = std::make_shared<BinOp>(TokKind::OrOr, lhs, rhs, lhs->line);
    }
    return lhs;
}

AstPtr Parser::parseLogicAnd() {
    auto lhs = parseEquality();
    while (check(TokKind::AndAnd)) {
        advance();
        auto rhs = parseEquality();
        lhs = std::make_shared<BinOp>(TokKind::AndAnd, lhs, rhs, lhs->line);
    }
    return lhs;
}

AstPtr Parser::parseEquality() {
    auto lhs = parseComparison();
    while (check(TokKind::EqEq) || check(TokKind::BangEq)) {
        TokKind op = advance().kind;
        auto rhs = parseComparison();
        lhs = std::make_shared<BinOp>(op, lhs, rhs, lhs->line);
    }
    return lhs;
}

AstPtr Parser::parseComparison() {
    auto lhs = parseAddition();
    while (check(TokKind::Lt) || check(TokKind::Le) || check(TokKind::Gt) || check(TokKind::Ge)) {
        TokKind op = advance().kind;
        auto rhs = parseAddition();
        lhs = std::make_shared<BinOp>(op, lhs, rhs, lhs->line);
    }
    return lhs;
}

AstPtr Parser::parseAddition() {
    auto lhs = parseMultiplication();
    while (check(TokKind::Plus) || check(TokKind::Minus)) {
        TokKind op = advance().kind;
        auto rhs = parseMultiplication();
        lhs = std::make_shared<BinOp>(op, lhs, rhs, lhs->line);
    }
    return lhs;
}

AstPtr Parser::parseMultiplication() {
    auto lhs = parseUnary();
    while (check(TokKind::Star) || check(TokKind::Slash) || check(TokKind::Percent)) {
        TokKind op = advance().kind;
        auto rhs = parseUnary();
        lhs = std::make_shared<BinOp>(op, lhs, rhs, lhs->line);
    }
    return lhs;
}

AstPtr Parser::parseUnary() {
    if (check(TokKind::Minus) || check(TokKind::Bang)) {
        TokKind op = advance().kind;
        auto operand = parseUnary();             // 右结合：--x 合法
        return std::make_shared<UnaryOp>(op, operand, operand->line);
    }
    return parseCall();
}

AstPtr Parser::parseCall() {
    auto expr = parsePrimary();
    // 函数调用：foo(a, b)
    if (auto v = std::dynamic_pointer_cast<VarRef>(expr); v && check(TokKind::LParen)) {
        advance();      // 吃 (
        std::vector<AstPtr> args;
        if (!check(TokKind::RParen)) {
            args.push_back(parseExpression());
            while (match(TokKind::Comma)) args.push_back(parseExpression());
        }
        expect(TokKind::RParen, ")");
        return std::make_shared<Call>(v->name, std::move(args), v->line);
    }
    return expr;
}

AstPtr Parser::parsePrimary() {
    const Token& t = peek();
    if (t.kind == TokKind::Number) {
        advance();
        return std::make_shared<NumLit>(std::get<double>(t.value), t.line);
    }
    if (t.kind == TokKind::String) {
        advance();
        return std::make_shared<StringLit>(std::get<std::string>(t.value), t.line);
    }
    if (t.kind == TokKind::True)  { advance(); return std::make_shared<BoolLit>(true,  t.line); }
    if (t.kind == TokKind::False) { advance(); return std::make_shared<BoolLit>(false, t.line); }
    if (t.kind == TokKind::Ident) {
        advance();
        return std::make_shared<VarRef>(std::get<std::string>(t.value), t.line);
    }
    if (t.kind == TokKind::LParen) {
        advance();
        auto inner = parseExpression();
        expect(TokKind::RParen, ")");
        return inner;
    }
    throw ParseError(std::string("expected expression but got ") + tokKindName(t.kind),
                     t.line, file_);
}

}  // namespace mycc
