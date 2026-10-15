// ============================================================================
// 【文件定位】parser.cpp —— 递归下降语法分析器实现（Token 流 → AST）
// ----------------------------------------------------------------------------
// 【阅读顺序】expect 守门员 → 顶层/语句 → 表达式优先级链 → primary。
//   每个函数开头"吃掉引导词类"（advance）、结尾 expect 必选词类 ——
//   "进入函数时的约定由文法保证，离开时的约定由 expect 保证"。
// ============================================================================
#include "mycc/parser.h"
#include "mycc/error.h"

#include <memory>
#include <string>

namespace Mycc {

// 必选词类守门员：不是 k 就抛"期望 X 实得 Y"（带行号与词类名翻译）
const Token& Parser::expect(TokKind k, const char* what) {
    if (!check(k)) {
        throw ParseError(std::string("expected ") + what + " but got " + tokKindName(peek().kind),
                         peek().line, file_);
        // 错误消息"人话化"：what 是中文描述（"变量名"/";"），
        //   实际拿到的词类经 tokKindName 翻译 —— 用户能直接理解差什么
    }
    return advance();
}

// ============== 顶层 ==============
// program → statement* EOF：循环吃语句直到哨兵
std::shared_ptr<Program> Parser::parseProgram() {
    std::vector<AstPtr> stmts;
    int firstLine = check(TokKind::Eof) ? 1 : peek().line;
    // 空程序时 peek() 是 EOF，读它的 line 也能拿到合法行号；这里特判为 1
    while (!check(TokKind::Eof)) {
        stmts.push_back(parseStatement());
        // 每条语句一个子树；EOF 哨兵让循环条件干净（无需判空/越界）
    }
    return std::make_shared<Program>(std::move(stmts), firstLine);
}

// ============== 语句 ==============
// 总分派：按首 Token 词类路由到具体语句函数；都不是 → 表达式语句兜底
AstPtr Parser::parseStatement() {
    // "看首词类定语句种类"——语言的保留字即文法的分岔口：
    if (check(TokKind::Let))    return parseLetDecl();
    if (check(TokKind::Print))  return parsePrintStmt();
    if (check(TokKind::If))     return parseIfStmt();
    if (check(TokKind::While))  return parseWhileStmt();
    if (check(TokKind::Return)) return parseReturnStmt();
    if (check(TokKind::LBrace)) return parseBlock();
    if (check(TokKind::Fn))     return parseFnDecl();
    return parseExprStmt();
    // 兜底 = 表达式语句（x = 1; / foo();）—— 任何非保留字开头的都是它
}

// let Ident = expr ;
AstPtr Parser::parseLetDecl() {
    int ln = advance().line;             // 吃 let（顺带取行号）
    const auto& nameTok = expect(TokKind::Ident, "变量名");
    expect(TokKind::Assign, "=");
    AstPtr init = parseExpression();
    expect(TokKind::Semicolon, ";");
    // "吃引导词 → 必选结构逐个 expect → 子表达式递归"的标准语句骨架
    return std::make_shared<LetDecl>(std::get<std::string>(nameTok.value), init, ln);
    // get<string>：Ident 的载荷必是 string（词法阶段的类型保证，无风险）
}

// print expr ;
AstPtr Parser::parsePrintStmt() {
    int ln = advance().line;             // 吃 print
    auto e = parseExpression();
    expect(TokKind::Semicolon, ";");
    return std::make_shared<PrintStmt>(e, ln);
}

// if ( expr ) stmt [else stmt]
AstPtr Parser::parseIfStmt() {
    int ln = advance().line;             // 吃 if
    expect(TokKind::LParen, "(");
    auto cond = parseExpression();
    expect(TokKind::RParen, ")");
    auto thenBranch = parseStatement();
    // 分支体是"语句"不是"块"—— if (c) x = 1; 单语句也合法，
    //   要多条语句自己写 {}（BlockStmt 又是一条语句）
    AstPtr elseBranch = nullptr;
    if (match(TokKind::Else)) elseBranch = parseStatement();
    // 可选 else：match 一行试探（有则吃掉并解析下一条语句）
    return std::make_shared<IfStmt>(cond, thenBranch, elseBranch, ln);
}

// while ( expr ) stmt
AstPtr Parser::parseWhileStmt() {
    int ln = advance().line;             // 吃 while
    expect(TokKind::LParen, "(");
    auto cond = parseExpression();
    expect(TokKind::RParen, ")");
    auto body = parseStatement();
    return std::make_shared<WhileStmt>(cond, body, ln);
}

// { stmt* }
AstPtr Parser::parseBlock() {
    int ln = advance().line;             // 吃 {
    std::vector<AstPtr> stmts;
    while (!check(TokKind::RBrace) && !check(TokKind::Eof)) {
        stmts.push_back(parseStatement());
        // 双终止条件：} 正常收尾 / EOF 兜底——缺 } 时最终靠 expect 报错
    }
    expect(TokKind::RBrace, "}");
    return std::make_shared<BlockStmt>(std::move(stmts), ln);
}

// return [expr] ;
AstPtr Parser::parseReturnStmt() {
    int ln = advance().line;             // 吃 return
    AstPtr v = nullptr;
    if (!check(TokKind::Semicolon)) v = parseExpression();
    // 语法试探决定"有没有返回值"：return 后紧跟分号 → 裸 return
    expect(TokKind::Semicolon, ";");
    return std::make_shared<ReturnStmt>(v, ln);
}

// fn Ident ( [Ident (, Ident)*] ) { stmt* }
AstPtr Parser::parseFnDecl() {
    int ln = advance().line;             // 吃 fn
    const auto& nameTok = expect(TokKind::Ident, "函数名");
    expect(TokKind::LParen, "(");

    std::vector<std::string> params;
    if (!check(TokKind::RParen)) {
        // 非空参数列表：第一个参数（无逗号前缀）+ "逗号引导"的后续参数
        params.push_back(std::get<std::string>(expect(TokKind::Ident, "参数名").value));
        while (match(TokKind::Comma)) {
            // match 试探逗号 —— "有逗号就必有一个参数"的经典循环写法
            params.push_back(std::get<std::string>(expect(TokKind::Ident, "参数名").value));
        }
    }
    expect(TokKind::RParen, ")");

    // 函数体：{ stmt* }，直接解析成语句列表
    // 不复用 parseBlock 的原因见 ast.h FuncDecl 注释（作用域管理归属帧）
    expect(TokKind::LBrace, "{");
    std::vector<AstPtr> body;
    while (!check(TokKind::RBrace) && !check(TokKind::Eof)) {
        body.push_back(parseStatement());
    }
    expect(TokKind::RBrace, "}");
    return std::make_shared<FuncDecl>(std::get<std::string>(nameTok.value),
                                      std::move(params), std::move(body), ln);
}

// expr ;（兜底语句）
AstPtr Parser::parseExprStmt() {
    auto e = parseExpression();
    expect(TokKind::Semicolon, ";");
    return std::make_shared<ExprStmt>(e, e->line);
}

// ============== 表达式（按优先级分层）==============
// 【分层总览（从低到高）】assignment → or → and → equality → comparison
//   → addition → multiplication → unary → call → primary
//   每层的通用形状：先递归调"下一层"拿左操作数，再 while 吃同级运算符
//   右折叠 —— 左结合的秘密就在"循环"，教程§5.6 的左结合错误现场正是
//   把循环写成了递归（1-2-3 变 1-(2-3)=2）。

AstPtr Parser::parseExpression()    { return parseAssignment(); }
// 表达式入口 = 最低优先级（赋值）—— 优先级链的最外层包装

// 赋值：唯一"右结合"的层 —— a = b = 1 是 a = (b = 1)
AstPtr Parser::parseAssignment() {
    auto lhs = parseLogicOr();
    // 先按更高优先级解析左部（可能是普通表达式）
    if (check(TokKind::Assign)) {
        advance();
        auto rhs = parseAssignment();        // 右结合：a = b = 1 合法
        // 递归调"自己"（而非下一层）—— 右结合的形状就是"递归自身"
        if (auto v = std::dynamic_pointer_cast<VarRef>(lhs)) {
            // 【语义限制】只有 VarRef 能当赋值目标 —— a+b=1 被语法层拒绝。
            //   dynamic_pointer_cast 向下转型：运行时验证 lhs 真是变量引用，
            //   失败返回空指针走报错（C++ 里"结构不合法"在 parse 阶段拦截）
            return std::make_shared<Assign>(v->name, rhs, lhs->line);
        }
        throw ParseError("assignment target must be a variable", lhs->line, file_);
    }
    return lhs;      // 没有 = → 原样上交（分层链的通用出口）
}

// ||：左结合，优先级次低
AstPtr Parser::parseLogicOr() {
    auto lhs = parseLogicAnd();
    while (check(TokKind::OrOr)) {
        // while 循环折叠同级：a || b || c → (a || b) || c —— 左结合范本
        advance();
        auto rhs = parseLogicAnd();
        lhs = std::make_shared<BinOp>(TokKind::OrOr, lhs, rhs, lhs->line);
        // 新左部 = 上一轮的树 —— 每圈长高一节，循环结束即得左结合树
    }
    return lhs;
}

// &&：与 || 同构（一个 TokKind 之差）
AstPtr Parser::parseLogicAnd() {
    auto lhs = parseEquality();
    while (check(TokKind::AndAnd)) {
        advance();
        auto rhs = parseEquality();
        lhs = std::make_shared<BinOp>(TokKind::AndAnd, lhs, rhs, lhs->line);
    }
    return lhs;
}

// == !=：同一优先级（两个词类共享一个循环）
AstPtr Parser::parseEquality() {
    auto lhs = parseComparison();
    while (check(TokKind::EqEq) || check(TokKind::BangEq)) {
        TokKind op = advance().kind;      // 记住是哪种运算再前进
        auto rhs = parseComparison();
        lhs = std::make_shared<BinOp>(op, lhs, rhs, lhs->line);
    }
    return lhs;
}

// < <= > >=：同一优先级
AstPtr Parser::parseComparison() {
    auto lhs = parseAddition();
    while (check(TokKind::Lt) || check(TokKind::Le) || check(TokKind::Gt) || check(TokKind::Ge)) {
        TokKind op = advance().kind;
        auto rhs = parseAddition();
        lhs = std::make_shared<BinOp>(op, lhs, rhs, lhs->line);
    }
    return lhs;
}

// + -：同一优先级
AstPtr Parser::parseAddition() {
    auto lhs = parseMultiplication();
    while (check(TokKind::Plus) || check(TokKind::Minus)) {
        TokKind op = advance().kind;
        auto rhs = parseMultiplication();
        lhs = std::make_shared<BinOp>(op, lhs, rhs, lhs->line);
    }
    return lhs;
}

// * / %：同一优先级（比加减高一层 → 乘除先结合，优先级的全部秘密）
AstPtr Parser::parseMultiplication() {
    auto lhs = parseUnary();
    while (check(TokKind::Star) || check(TokKind::Slash) || check(TokKind::Percent)) {
        TokKind op = advance().kind;
        auto rhs = parseUnary();
        lhs = std::make_shared<BinOp>(op, lhs, rhs, lhs->line);
    }
    return lhs;
}

// 一元 - !：右结合（- - x 合法 → -(-x)）
AstPtr Parser::parseUnary() {
    if (check(TokKind::Minus) || check(TokKind::Bang)) {
        TokKind op = advance().kind;
        auto operand = parseUnary();             // 右结合：--x 合法
        // 递归调"自己"—— 一元前缀天然右结合（-!x 是 -( !x)）
        return std::make_shared<UnaryOp>(op, operand, operand->line);
    }
    return parseCall();     // 无一元符号 → 落到更高优先级的调用层
}

// 后缀：函数调用（比一元高 —— -fib(2) 是 -(fib(2))）
AstPtr Parser::parseCall() {
    auto expr = parsePrimary();
    // 函数调用：foo(a, b)
    if (auto v = std::dynamic_pointer_cast<VarRef>(expr); v && check(TokKind::LParen)) {
        // 【后缀识别】先拿到原子，再看后面是否跟"("：
        //   只有 VarRef 后跟 ( 才算调用 —— "3(4)" 不是调用（会走到返回，
        //   随后外层语句 expect 分号时报错）
        advance();      // 吃 (
        std::vector<AstPtr> args;
        if (!check(TokKind::RParen)) {       // 空参数表特判
            args.push_back(parseExpression());
            while (match(TokKind::Comma)) args.push_back(parseExpression());
            // "逗号引导参数"循环 —— 与形参列表同构
        }
        expect(TokKind::RParen, ")");
        return std::make_shared<Call>(v->name, std::move(args), v->line);
    }
    return expr;      // 不带 ( → 原样上交
}

// 原子层：表达式的最小单元（优先级链的终点）
AstPtr Parser::parsePrimary() {
    const Token& t = peek();
    // 逐词类分派 —— 每种原子一个分支，最后 default 报错
    if (t.kind == TokKind::Number) {
        advance();
        return std::make_shared<NumLit>(std::get<double>(t.value), t.line);
        // get<double>：Number 载荷必为 double（词法保证）
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
        return inner;     // 括号不解成节点：解包后直接返回内部表达式
        // 【括号的语义】只影响结合（优先级提升），本身无内容 ——
        //   返回 inner 而非包一层，AST 更干净
    }
    throw ParseError(std::string("expected expression but got ") + tokKindName(t.kind),
                     t.line, file_);
    // 都不是 → "此处应是表达式"（把 ; 或 EOF 当表达式时走这里）
}

}  // namespace Mycc
