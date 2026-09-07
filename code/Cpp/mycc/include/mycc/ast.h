#pragma once

#include "token.h"
#include <memory>
#include <string>
#include <vector>

namespace mycc {

// 类型标签——TypeChecker 阶段会给每个表达式贴上
enum class Type { Unknown, Num, Bool, Str, Void };

// 前置声明 Visitor
template <typename R> class AstVisitor;

// 所有节点的公共基类
class AstNode {
public:
    int  line = 1;                       // 出错时用
    Type ty   = Type::Unknown;           // TypeChecker 标注

    AstNode() = default;
    explicit AstNode(int ln) : line(ln) {}
    virtual ~AstNode() = default;

    // ⭐ 双访问者接口：一个返回 Type、一个返回 void
    virtual Type acceptType(AstVisitor<Type>& v) = 0;
    virtual void acceptCode(AstVisitor<void>& v) = 0;
};

using AstPtr = std::shared_ptr<AstNode>;

// === 表达式节点 ===

class NumLit : public AstNode {
public:
    double value;
    NumLit(double v, int ln) : AstNode(ln), value(v) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class StringLit : public AstNode {
public:
    std::string value;
    StringLit(std::string s, int ln) : AstNode(ln), value(std::move(s)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class BoolLit : public AstNode {
public:
    bool value;
    BoolLit(bool b, int ln) : AstNode(ln), value(b) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class VarRef : public AstNode {
public:
    std::string name;
    VarRef(std::string n, int ln) : AstNode(ln), name(std::move(n)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class BinOp : public AstNode {
public:
    TokKind op;             // +, -, *, /, %, ==, !=, <, <=, >, >=, &&, ||
    AstPtr  lhs, rhs;
    BinOp(TokKind o, AstPtr l, AstPtr r, int ln)
        : AstNode(ln), op(o), lhs(std::move(l)), rhs(std::move(r)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class UnaryOp : public AstNode {
public:
    TokKind op;             // -（取负）, !（逻辑非）
    AstPtr  operand;
    UnaryOp(TokKind o, AstPtr e, int ln)
        : AstNode(ln), op(o), operand(std::move(e)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class Assign : public AstNode {
public:
    std::string name;       // 只支持 x = expr 形式
    AstPtr      value;
    Assign(std::string n, AstPtr v, int ln)
        : AstNode(ln), name(std::move(n)), value(std::move(v)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class Call : public AstNode {
public:
    std::string         callee;
    std::vector<AstPtr> args;
    Call(std::string c, std::vector<AstPtr> a, int ln)
        : AstNode(ln), callee(std::move(c)), args(std::move(a)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

// === 语句节点 ===

class ExprStmt : public AstNode {        // 单纯的表达式语句：foo(); x = 1;
public:
    AstPtr expr;
    ExprStmt(AstPtr e, int ln) : AstNode(ln), expr(std::move(e)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class LetDecl : public AstNode {
public:
    std::string name;
    AstPtr      init;
    LetDecl(std::string n, AstPtr i, int ln)
        : AstNode(ln), name(std::move(n)), init(std::move(i)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class IfStmt : public AstNode {
public:
    AstPtr cond, thenBranch, elseBranch;     // elseBranch 可为 nullptr
    IfStmt(AstPtr c, AstPtr t, AstPtr e, int ln)
        : AstNode(ln), cond(std::move(c)),
          thenBranch(std::move(t)), elseBranch(std::move(e)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class WhileStmt : public AstNode {
public:
    AstPtr cond, body;
    WhileStmt(AstPtr c, AstPtr b, int ln)
        : AstNode(ln), cond(std::move(c)), body(std::move(b)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class BlockStmt : public AstNode {           // 大括号包起来的语句块
public:
    std::vector<AstPtr> stmts;
    BlockStmt(std::vector<AstPtr> s, int ln) : AstNode(ln), stmts(std::move(s)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class FuncDecl : public AstNode {
public:
    std::string              name;
    std::vector<std::string> params;
    std::vector<AstPtr>      body;        // 函数体语句列表
    FuncDecl(std::string n, std::vector<std::string> p, std::vector<AstPtr> b, int ln)
        : AstNode(ln), name(std::move(n)),
          params(std::move(p)), body(std::move(b)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class ReturnStmt : public AstNode {
public:
    AstPtr value;        // 可为 nullptr
    ReturnStmt(AstPtr v, int ln) : AstNode(ln), value(std::move(v)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class PrintStmt : public AstNode {
public:
    AstPtr expr;
    PrintStmt(AstPtr e, int ln) : AstNode(ln), expr(std::move(e)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

// 顶层程序：一组语句和函数声明
class Program : public AstNode {
public:
    std::vector<AstPtr> stmts;
    Program(std::vector<AstPtr> s, int ln) : AstNode(ln), stmts(std::move(s)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

}  // namespace mycc

// === accept 实现（需要 visitor.h 的完整定义）===
#include "visitor.h"

namespace mycc {

inline Type NumLit::acceptType(AstVisitor<Type>& v)      { return v.visit(*this); }
inline void NumLit::acceptCode(AstVisitor<void>& v)      {        v.visit(*this); }

inline Type StringLit::acceptType(AstVisitor<Type>& v)   { return v.visit(*this); }
inline void StringLit::acceptCode(AstVisitor<void>& v)   {        v.visit(*this); }

inline Type BoolLit::acceptType(AstVisitor<Type>& v)     { return v.visit(*this); }
inline void BoolLit::acceptCode(AstVisitor<void>& v)     {        v.visit(*this); }

inline Type VarRef::acceptType(AstVisitor<Type>& v)      { return v.visit(*this); }
inline void VarRef::acceptCode(AstVisitor<void>& v)      {        v.visit(*this); }

inline Type BinOp::acceptType(AstVisitor<Type>& v)       { return v.visit(*this); }
inline void BinOp::acceptCode(AstVisitor<void>& v)       {        v.visit(*this); }

inline Type UnaryOp::acceptType(AstVisitor<Type>& v)     { return v.visit(*this); }
inline void UnaryOp::acceptCode(AstVisitor<void>& v)     {        v.visit(*this); }

inline Type Assign::acceptType(AstVisitor<Type>& v)      { return v.visit(*this); }
inline void Assign::acceptCode(AstVisitor<void>& v)      {        v.visit(*this); }

inline Type Call::acceptType(AstVisitor<Type>& v)        { return v.visit(*this); }
inline void Call::acceptCode(AstVisitor<void>& v)        {        v.visit(*this); }

inline Type ExprStmt::acceptType(AstVisitor<Type>& v)    { return v.visit(*this); }
inline void ExprStmt::acceptCode(AstVisitor<void>& v)    {        v.visit(*this); }

inline Type LetDecl::acceptType(AstVisitor<Type>& v)     { return v.visit(*this); }
inline void LetDecl::acceptCode(AstVisitor<void>& v)     {        v.visit(*this); }

inline Type IfStmt::acceptType(AstVisitor<Type>& v)      { return v.visit(*this); }
inline void IfStmt::acceptCode(AstVisitor<void>& v)      {        v.visit(*this); }

inline Type WhileStmt::acceptType(AstVisitor<Type>& v)   { return v.visit(*this); }
inline void WhileStmt::acceptCode(AstVisitor<void>& v)   {        v.visit(*this); }

inline Type BlockStmt::acceptType(AstVisitor<Type>& v)   { return v.visit(*this); }
inline void BlockStmt::acceptCode(AstVisitor<void>& v)   {        v.visit(*this); }

inline Type FuncDecl::acceptType(AstVisitor<Type>& v)    { return v.visit(*this); }
inline void FuncDecl::acceptCode(AstVisitor<void>& v)    {        v.visit(*this); }

inline Type ReturnStmt::acceptType(AstVisitor<Type>& v)  { return v.visit(*this); }
inline void ReturnStmt::acceptCode(AstVisitor<void>& v)  {        v.visit(*this); }

inline Type PrintStmt::acceptType(AstVisitor<Type>& v)   { return v.visit(*this); }
inline void PrintStmt::acceptCode(AstVisitor<void>& v)   {        v.visit(*this); }

inline Type Program::acceptType(AstVisitor<Type>& v)     { return v.visit(*this); }
inline void Program::acceptCode(AstVisitor<void>& v)     {        v.visit(*this); }

}  // namespace mycc
