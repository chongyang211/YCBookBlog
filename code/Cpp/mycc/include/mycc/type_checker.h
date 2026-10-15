#pragma once

#include "ast.h"      // AstPtr / Type（ast.h 里定义的枚举）
#include "visitor.h"   // AstVisitor<Type> 基类
#include <unordered_map>
#include <vector>
#include <string>

namespace Mycc {

// ============================================================================
// 【模块定位】type_checker.h —— 语义检查访问者（教程阶段⑤：AST → 标注后的 AST）
// ----------------------------------------------------------------------------
// 【设计思想】编译器的第一遍遍历：不改树、只"贴标签"（节点.ty）+ 查错。
//   词法/语法管"形状对不对"，类型检查管"意思通不通"：
//   未定义变量、算术撞字符串、条件不是布尔、参数个数不符……
//   都在这一遍暴露 —— 错误越早发现，修复成本越低（教程"五段式"的分界意义）。
// 【符号表栈】vector<map> 表达词法作用域：进块 push 层、出块 pop 层、
//   查找从最内层往外（遮蔽 shadowing 语义）—— "栈"是抽象，vector 是
//   实现（README 取舍：vector 还能反向遍历，std::stack 不能）。
// ============================================================================

// 函数符号——记录函数的参数个数（本案例不严格检查参数类型）
struct FnSig {
    int paramCount;
    int line;       // 函数声明行号（潜在错误可引用）
};
// 【"先注册签名后查调用"】函数调用允许"先调用后声明"甚至互相递归 ——
//   诀窍是 Program 的两遍遍历（见 type_checker.cpp）：第一遍只收集签名，
//   第二遍才逐句检查 —— 真实编译器的 pass 划分思想（预遍历/主遍历）

class TypeChecker : public AstVisitor<Type> {
// 继承模板基类 AstVisitor<Type>：17 个 visit 返回 Type ——
// 模板实例化出"返回类型的访问者"（见 visitor.h 的模板设计说明）
public:
    explicit TypeChecker(std::string filename = "<repl>") : file_(std::move(filename)) {}
    // explicit 单参防隐式；文件名仅用于报错

    // 入口：进入全局作用域后对整棵树做类型检查
    void check(AstPtr program);
    // 便捷入口：包一层"开全局作用域"的仪式（start 检查的独立可测）

    // 17 个 visit 实现
    // 【三类简单节点的类内实现】字面量的类型就是它自己 —— 一行即可写完，
    //   类内定义（隐式 inline）免进 .cpp；复杂节点才放 .cpp
    Type visit(NumLit& n)     override { return n.ty = Type::Num; }
    Type visit(StringLit& n)  override { return n.ty = Type::Str; }
    Type visit(BoolLit& n)    override { return n.ty = Type::Bool; }
    // 【统一的"贴标签"句式】n.ty = X 是本访问者的核心动作：
    //   把推导结果写进节点（后续阶段可读）并作为返回值上交父节点
    Type visit(VarRef& n)     override;
    Type visit(BinOp& n)      override;
    Type visit(UnaryOp& n)    override;
    Type visit(Assign& n)     override;
    Type visit(Call& n)       override;
    Type visit(ExprStmt& n)   override { n.expr->acceptType(*this); return Type::Void; }
    // 表达式语句：检查子表达式（触发其报错）但丢弃值类型 —— 语句永远是 Void
    Type visit(LetDecl& n)    override;
    Type visit(IfStmt& n)     override;
    Type visit(WhileStmt& n)  override;
    Type visit(BlockStmt& n)  override;
    Type visit(FuncDecl& n)   override;
    Type visit(ReturnStmt& n) override;
    Type visit(PrintStmt& n)  override { n.expr->acceptType(*this); return Type::Void; }
    // print 对表达式类型"宽容"——任何类型都可打印，只查子树合法性
    Type visit(Program& n)    override;

private:
    // 符号表栈：每个元素是一层作用域
    std::vector<std::unordered_map<std::string, Type>> scopes;
    // 【vector 而非 std::stack 的理由】查找要"从内向外反向遍历" ——
    //   std::stack 无迭代器；vector 的 rbegin/rend 天然支持（README 取舍）
    std::unordered_map<std::string, FnSig>             functions;
    // 函数签名表：与变量作用域分离（函数归全局，本语言函数不嵌套）
    std::string                                        file_;
    // 文件名：只服务于报错

    void enterScope() { scopes.emplace_back(); }
    void leaveScope() { scopes.pop_back();    }
    // 作用域 = vector 的进出 —— "栈"的抽象压在 vector 上（emplace_back 空表）

    bool define(const std::string& name, Type t);
    Type lookup(const std::string& name, int line);
    // 增/查两个原语：define 只写当前层（同名检查），lookup 全栈反查 ——
    // 实现（含遮蔽语义）见 type_checker.cpp
};

}  // namespace Mycc
