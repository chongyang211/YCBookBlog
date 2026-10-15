#pragma once

#include "ast.h"      // 产物类型
#include "token.h"    // 输入类型
#include <vector>

namespace Mycc {

// ============================================================================
// 【模块定位】parser.h —— 递归下降语法分析器（教程阶段④：Token 流 → AST）
// ----------------------------------------------------------------------------
// 【设计思想·递归下降】"每个非终结符一个函数"——文法规则直接映射成函数：
//   program → statement* ；expression 按优先级拆成 assignment→or→and→…→primary
//   的调用链。表达式优先级用"分层函数"表达（教程称"优先级爬升"的教学版）：
//   越靠下的函数优先级越高，上层循环左折叠同优先级的连续运算（左结合）。
// 【两个造BUG教学点（教程§5.6）】
//   ① 左结合写错（a-b-c 变 a-(b-c)）→ 1-2-3 输出 2 —— 用循环而非递归即可避免；
//   ② 本实现采用 while 循环左折叠 —— 正确的左结合范本。
// ============================================================================
class Parser {
public:
    // 按值收 Token vector（所有权转移）+ 文件名（报错用）
    Parser(std::vector<Token> tokens, std::string filename = "<repl>")
        : toks(std::move(tokens)), file_(std::move(filename)) {}

    std::shared_ptr<Program> parseProgram();   // 主入口：解析整个文件
    // 返回具体类型 shared_ptr<Program>（而非 AstPtr）—— 调用方
    //   （TypeChecker::check / Codegen::compile）需要具体根类型，类型即文档

private:
    std::vector<Token> toks;   // 输入：词法产物（随机访问的 Token 数组）
    std::string file_;         // 文件名：随 ParseError 上报
    size_t      pos = 0;       // 游标：当前 Token 下标（vector 版的"前进指针"）

    // ---- 游标四件套（Token 版）----
    const Token& peek()    const { return toks[pos]; }
    // 不判越界：EOF 哨兵 Token 保证 peek 永远合法（token.h 的设计回报）
    const Token& advance()       { return toks[pos++]; }
    // "看一眼并前进"——返回 const 引用（Token 归 vector 所有，解析只读）
    bool         check(TokKind k) const { return peek().kind == k; }
    // 只看不动的"测试"
    bool         match(TokKind k) {
        // "有则吃掉返回 true，无则原样返回 false"—— 可选结构（else 分支、
        // 逗号分隔）靠它一行写出"要就有没有拉倒"的试探
        if (check(k)) { advance(); return true; }
        return false;
    }
    const Token& expect(TokKind k, const char* what);
    // "必须有否则报错"——必选结构（分号、右括号）的守门员；实现在 .cpp

    // ---- 表达式（按优先级分层）----
    // 【分层链总览】低优先级函数调用高优先级函数，返回时用 while 把
    //   同级连续运算左折叠 —— 一层一个优先级，文法即代码：
    //   assignment(= 右结合) → || → && → == != → < <= > >= → + - → * / % → 一元 → 调用 → 原子
    AstPtr parseExpression();      // 最低优先级入口（= parseAssignment 的别名）
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
    // 【结合性在代码里的形状】左结合 → while 循环折叠（a-b-c → (a-b)-c）；
    //   右结合 → 递归调自己（a=b=c → a=(b=c)；- -x → -(-x)）—— 详见 .cpp

    // ---- 语句 ----
    AstPtr parseStatement();   // 总分派：按首 Token 的词类路由
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
