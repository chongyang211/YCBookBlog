#pragma once

// ⚠️ 本文件只前置声明所有节点类型，不 include ast.h
// 因为 ast.h 已经前置声明了 AstVisitor<R>，避免循环引用
// 【循环打破的对称手法】ast.h 与 visitor.h 互相需要对方的名字：
//   ast.h 里 acceptType(AstVisitor<Type>&) 需要 visitor 的名字；
//   visitor.h 里 visit(NumLit&) 需要 ast 的名字 ——
//   两侧都用"前置声明 + 自己的 .cpp 里 include 完整定义"化解：
//   指针/引用参数只需类名，不需要对象大小和成员布局。

namespace Mycc {

// 前置声明所有节点（与 ast.h 中的派生类一一对应）
class NumLit; class StringLit; class BoolLit; class VarRef;
class BinOp;  class UnaryOp; class Assign;  class Call;
class ExprStmt; class LetDecl; class IfStmt; class WhileStmt;
class BlockStmt; class FuncDecl; class ReturnStmt; class PrintStmt;
class Program;

// ⭐ 模板访问者基类——每种返回类型 R 一份特化
// ============================================================================
// 【模块定位】visitor.h —— 访问者模式的模板化实现（教程阶段⑤）
// ----------------------------------------------------------------------------
// 【设计思想·为什么需要访问者】AST 有 17 种节点、有两种互不相关的遍历
//   （类型检查 / 代码生成）。若把两者写成 AST 类的成员函数：
//   ① 每加一种遍历要改 17 个类（开闭原则反面）；
//   ② TypeChecker 需要携带"符号表栈"、Codegen 需要"帧栈"——这些状态
//     放在节点类里完全不合适。
//   访问者模式把"遍历逻辑 + 遍历状态"打包进 Visitor 类，节点只留一个
//   accept 钩子 —— 加新遍历 = 新增一个 Visitor 子类，节点零改动。
// 【模板化突破（对比 GoF 经典版）】经典 Visitor 所有 visit 返回统一类型 ——
//   但 TypeChecker 天然要返回 Type、Codegen 只要 void。模板参数 R 让
//   "同一套接口形状"实例化出两份：AstVisitor<Type> 与 AstVisitor<void>。
//   这是 C++ 泛型对 GoF 设计模式的扩展（README"关键设计取舍"第二条）。
// ============================================================================
template <typename R>
class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    // 虚析构：Visitor 经基类指针使用/销毁（delete 基类指针走虚表才正确析构）

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
    virtual R visit(WhileStmt&) = 0;
    virtual R visit(BlockStmt&)  = 0;
    virtual R visit(FuncDecl&)   = 0;
    virtual R visit(ReturnStmt&) = 0;
    virtual R visit(PrintStmt&)  = 0;
    virtual R visit(Program&)    = 0;
    // 【纯虚 = 强制契约】17 个 visit 全部 = 0：子类必须逐个实现 ——
    //   新增节点类型时，两个 Visitor（TypeChecker/Codegen）忘实现任一个
    //   都会在编译期暴露（这正是"访问者模式的编译期完整性检查"红利）。
    // 【参数是"具体节点引用"】每个 visit 一个重载 —— 调用方（accept）
    //   静态已知节点类型，直接重载决议到对应 visit（double dispatch 的
    //   第一跳靠 accept 的虚函数、第二跳靠 visit 的重载决议）。
};

}  // namespace Mycc
