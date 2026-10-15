#pragma once

#include "ast.h"      // AstPtr / Type
#include "visitor.h"   // AstVisitor<void> 基类
#include "chunk.h"    // Chunk / Constant（字节码容器）
#include <unordered_map>
#include <vector>
#include <string>

namespace Mycc {

// ============================================================================
// 【模块定位】codegen.h —— 代码生成访问者（教程阶段⑥：AST → 字节码）
// ----------------------------------------------------------------------------
// 【设计思想】第二遍遍历（TypeChecker 之后），返回 void（AstVisitor<void>）——
//   类型信息已由第一遍贴在节点上，本遍只"翻译"。
// 【帧栈】函数嵌套编译需要"上下文切换"：进入 FuncDecl push 新 CodegenFrame
//   （独立 Chunk + 自己的局部变量表 + 作用域深度），函数体编完 pop 回主帧 ——
//   "编译期的调用栈"，与 VM 的运行期 CallFrame 遥相呼应。
// 【局部变量解析】Codegen 期决定"每个名字是全局还是局部、局部的栈槽号"——
//   运行期 LOAD_LOCAL 只带数字（零查找）；名字→槽位的映射编译期全部完成
//   （这正是"编译"相对"解释"的性能红利来源）。
// ============================================================================

// 局部变量信息：名字 + 它的作用域深度
struct Local {
    std::string name;
    int         depth;     // 0 = 全局，1+ = 块内层级
    // depth 配合 scopeDepth 做"出作用域弹谁"：endScope 弹出所有
    //   depth > 当前 scopeDepth 的局部（见 codegen.cpp endScope）
};

// 一个函数 = 一个 CodegenFrame
struct CodegenFrame {
    Chunk              chunk;      // 本函数的字节码容器
    std::vector<Local> locals;     // 本函数的局部变量表（下标 = 栈槽号）
    int                scopeDepth = 0;   // 当前块嵌套深度
    // 【"帧"的三要素】指令流 + 变量表 + 深度 —— top 帧是主程序、
    //   每个 FuncDecl 开新帧；帧之间完全隔离（函数的局部互不可见）
    explicit CodegenFrame(std::string name) : chunk(std::move(name)) {}
};

class Codegen : public AstVisitor<void> {
    // 继承"void 版访问者"：17 个 visit 不返回值 ——
    //   表达式的"结果"全走求值栈（VM 的栈就是返回通道）
public:
    Codegen();

    // 顶层入口：返回主 Chunk
    Chunk compile(Program& prog);
    // 【入口设计】只上交主 chunk，函数 chunks 走下面的 public 成员 ——
    //   main 拿两者喂给 VM（Compiled 结构体的两个字段）

    // 函数名 → 函数 Chunk（VM 用）
    std::unordered_map<std::string, Chunk> functions;
    // 【public 成员的取舍】编译产物直接暴露给调用方（简单）——
    //   工程上应封装成 getter；教学版省一层包装

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
    // 【帧栈】vector 实现：push_back/pop_back 当调用栈 —— 同 TypeChecker
    //   的 scopes 用 vector 的理由（README 取舍：还要反向遍历查 locals）
    CodegenFrame& cur() { return frames_.back(); }
    Chunk&        chk() { return cur().chunk; }
    // 两个速记：所有 emit 都落在"当前帧的 chunk" —— visit 里高频出现，
    //   一层封装让代码主干只剩业务语义

    // ----- 作用域 -----
    void beginScope() { cur().scopeDepth++; }
    // 深度 +1 而不立即改 locals —— 新局部在 declareLocal 时带当前深度入表
    void endScope();   // 弹出本作用域所有 locals + 发出 POP
    // 【编译期作用域 → 运行期 POP】块退出时栈上还压着块内局部 ——
    //   必须按"每个局部一条 POP"把栈退干净（栈平衡的编译期责任）

    // ----- 变量解析 -----
    int  resolveLocal(const std::string& name);   // 返回栈偏移；-1 = 不是局部
    // 从帧内 locals 反向查名字：命中返回槽号（最近的同名遮蔽外层 ——
    //   与 TypeChecker::lookup 的 rbegin 遍历语义对齐）
    void declareLocal(const std::string& name, int line);
    // 局部入表：槽号 = locals.size()（即它在求值栈上的相对位置）

    // ----- 字面量辅助 -----
    void emitConst(Constant c, int line);
    // "常量入池 + CONST 下标"两步合一 —— 字面量代码生成的高频原子操作
};

}  // namespace Mycc
