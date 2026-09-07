#pragma once

// ⚠️ 本文件只前置声明所有节点类型，不 include ast.h
// 因为 ast.h 已经前置声明了 AstVisitor<R>，避免循环引用

namespace Mycc {

// 前置声明所有节点（与 ast.h 中的派生类一一对应）
class NumLit; class StringLit; class BoolLit; class VarRef;
class BinOp;  class UnaryOp; class Assign;  class Call;
class ExprStmt; class LetDecl; class IfStmt; class WhileStmt;
class BlockStmt; class FuncDecl; class ReturnStmt; class PrintStmt;
class Program;

// ⭐ 模板访问者基类——每种返回类型 R 一份特化
template <typename R>
class AstVisitor {
public:
    virtual ~AstVisitor() = default;

    // 表达式
    virtual R visit(NumLit&)     = 0;
    virtual R visit(StringLit&)  = 0;
    virtual R visit(BoolLit&)    = 0;
    virtual R visit(VarRef&)     = 0;
    virtual R visit(BinOp&)      = 0;
    virtual R visit(UnaryOp&)    = 0;
    virtual R visit(Assign&)     = 0;
    virtual R visit(Call&)       = 0;
    // 语句
    virtual R visit(ExprStmt&)   = 0;
    virtual R visit(LetDecl&)    = 0;
    virtual R visit(IfStmt&)     = 0;
    virtual R visit(WhileStmt&)  = 0;
    virtual R visit(BlockStmt&)  = 0;
    virtual R visit(FuncDecl&)   = 0;
    virtual R visit(ReturnStmt&) = 0;
    virtual R visit(PrintStmt&)  = 0;
    virtual R visit(Program&)    = 0;
};

}  // namespace Mycc
