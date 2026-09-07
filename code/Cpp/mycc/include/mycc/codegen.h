#pragma once

#include "ast.h"
#include "visitor.h"
#include "chunk.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace Mycc {

// 局部变量信息：名字 + 它的作用域深度
struct Local {
    std::string name;
    int         depth;     // 0 = 全局，1+ = 块内层级
};

// 一个函数 = 一个 CodegenFrame
struct CodegenFrame {
    Chunk              chunk;
    std::vector<Local> locals;
    int                scopeDepth = 0;   // 当前块嵌套深度

    explicit CodegenFrame(std::string name) : chunk(std::move(name)) {}
};

class Codegen : public AstVisitor<void> {
public:
    Codegen();

    // 顶层入口：返回主 Chunk
    Chunk compile(Program& prog);

    // 函数名 → 函数 Chunk（VM 用）
    std::unordered_map<std::string, Chunk> functions;

    // ----- 17 个 visit 重载 -----
    void visit(NumLit&)      override;
    void visit(StringLit&)   override;
    void visit(BoolLit&)     override;
    void visit(VarRef&)      override;
    void visit(BinOp&)       override;
    void visit(UnaryOp&)     override;
    void visit(Assign&)      override;
    void visit(Call&)        override;

    void visit(ExprStmt&)    override;
    void visit(LetDecl&)     override;
    void visit(IfStmt&)      override;
    void visit(WhileStmt&)   override;
    void visit(BlockStmt&)   override;
    void visit(FuncDecl&)    override;
    void visit(ReturnStmt&)  override;
    void visit(PrintStmt&)   override;
    void visit(Program&)     override;

private:
    // 当前正在写的栈帧（栈顶 = 当前函数）
    std::vector<CodegenFrame> frames_;
    CodegenFrame& cur() { return frames_.back(); }
    Chunk&        chk() { return cur().chunk; }

    // ----- 作用域 -----
    void beginScope() { cur().scopeDepth++; }
    void endScope();   // 弹出本作用域所有 locals + 发出 POP

    // ----- 变量解析 -----
    int  resolveLocal(const std::string& name);   // 返回栈偏移；-1 = 不是局部
    void declareLocal(const std::string& name, int line);

    // ----- 字面量辅助 -----
    void emitConst(Constant c, int line);
};

}  // namespace Mycc
