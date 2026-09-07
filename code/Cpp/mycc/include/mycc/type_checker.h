#pragma once

#include "ast.h"
#include "visitor.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace Mycc {

// 函数符号——记录函数的参数个数（本案例不严格检查参数类型）
struct FnSig {
    int paramCount;
    int line;
};

class TypeChecker : public AstVisitor<Type> {
public:
    explicit TypeChecker(std::string filename = "<repl>") : file_(std::move(filename)) {}

    // 入口：进入全局作用域后对整棵树做类型检查
    void check(AstPtr program);

    // 17 个 visit 实现
    Type visit(NumLit& n)     override { return n.ty = Type::Num; }
    Type visit(StringLit& n)  override { return n.ty = Type::Str; }
    Type visit(BoolLit& n)    override { return n.ty = Type::Bool; }
    Type visit(VarRef& n)     override;
    Type visit(BinOp& n)      override;
    Type visit(UnaryOp& n)    override;
    Type visit(Assign& n)     override;
    Type visit(Call& n)       override;
    Type visit(ExprStmt& n)   override { n.expr->acceptType(*this); return Type::Void; }
    Type visit(LetDecl& n)    override;
    Type visit(IfStmt& n)     override;
    Type visit(WhileStmt& n)  override;
    Type visit(BlockStmt& n)  override;
    Type visit(FuncDecl& n)   override;
    Type visit(ReturnStmt& n) override;
    Type visit(PrintStmt& n)  override { n.expr->acceptType(*this); return Type::Void; }
    Type visit(Program& n)    override;

private:
    // 符号表栈：每个元素是一层作用域
    std::vector<std::unordered_map<std::string, Type>> scopes;
    std::unordered_map<std::string, FnSig>             functions;
    std::string                                        file_;

    void enterScope() { scopes.emplace_back(); }
    void leaveScope() { scopes.pop_back();    }

    bool define(const std::string& name, Type t);
    Type lookup(const std::string& name, int line);
};

}  // namespace Mycc
