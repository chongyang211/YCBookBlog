#pragma once

#include "token.h"   // TokKind（BinOp/UnaryOp 存运算符词类）
#include <memory>    // shared_ptr / make_shared
#include <string>
#include <vector>

namespace Mycc {

// ============================================================================
// 【模块定位】ast.h —— AST：17 类语法节点（教程阶段③④，多态的主场）
// ----------------------------------------------------------------------------
// 【设计思想】"每个语法构造一个类"：字面量/变量/运算/调用是表达式，
//   let/if/while/块/函数/return/print 是语句，Program 是根 ——
//   树形结构 = 语法结构的直接映射（代码文本的形状就是 AST 的形状）。
// 【shared_ptr vs unique_ptr（README"关键设计取舍"第一条）】
//   教学选 shared：REPL 中树被多次遍历、示例代码引用语义直观；
//   工程应改 unique（所有权独占、更轻）—— 引用计数不是免费的。
// ============================================================================

// 类型标签——TypeChecker 阶段会给每个表达式贴上
enum class Type { Unknown, Num, Bool, Str, Void };
// 【"标注"式语义分析】类型检查不改树结构，只往节点上"贴标签"：
//   Codegen/后续阶段凭 ty 读取结论 —— 树是"带注解的中间表示"。
//   Unknown 是"还没查过"的初始态；Void 表达式语句/声明的"无值"。

// 前置声明 Visitor
template <typename R> class AstVisitor;

// 所有节点的公共基类
class AstNode {
public:
    int  line = 1;                       // 出错时用
    // 行号从 Token 一路带到节点：错误报告的定位信息在"构造 AST 时"就已就位
    Type ty   = Type::Unknown;           // TypeChecker 标注

    AstNode() = default;
    explicit AstNode(int ln) : line(ln) {}
    // explicit：int 到节点的隐式转换无意义，禁掉
    virtual ~AstNode() = default;
    // 虚析构：节点经 AstPtr（基类指针）delete —— 必须虚才能析构派生部分

    // ⭐ 双访问者接口：一个返回 Type、一个返回 void
    virtual Type acceptType(AstVisitor<Type>& v) = 0;
    virtual void acceptCode(AstVisitor<void>& v) = 0;
    // 【为什么是两个 accept 而不是一个】C++ 虚函数不能模板化 ——
    //   accept<AstVisitor<R>>(v) 无法做成虚函数，于是按 R 的两种实例
    //   （Type/void）各写一个纯虚钩子。双分派的"第一跳"由此发生：
    //   node->acceptType(checker) —— 虚表定位到"NumLit 的 acceptType"，
    //   它内部调 v.visit(*this)（静态类型 NumLit&）完成第二跳，
    //   重载决议到 TypeChecker::visit(NumLit&)。
    //   【对比 GoF】经典访问者一个 accept 配所有 Visitor；本项目
    //   "两个 accept + 返回类型模板"是 C++ 强类型下的务实变体。
};

using AstPtr = std::shared_ptr<AstNode>;
// 全树用 AstPtr 装配 —— 编译器各阶段之间传递的是 AstPtr/AstPtr 树

// === 表达式节点 ===
// 【共同点】都有 accept 双钩子 + 构造时带行号 —— 以下逐类只标注差异

class NumLit : public AstNode {
public:
    double value;
    // double 统一承载整数/浮点（与 Token::Number 的载荷一致）
    NumLit(double v, int ln) : AstNode(ln), value(v) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class StringLit : public AstNode {
public:
    std::string value;
    // 按值存 string：节点拥有内容（Lexer 产出的 token 字符串被 move 进来）
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
    std::string name;      // 变量引用只存名字 —— "查名字得值"推迟到检查/运行阶段
    VarRef(std::string n, int ln) : AstNode(ln), name(std::move(n)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class BinOp : public AstNode {
public:
    TokKind op;             // +, -, *, /, %, ==, !=, <, <=, >, >=, &&, ||
    // 【复用 TokKind 而非新枚举】运算符的"身份"词法阶段已定义 ——
    //   AST 不发明第二套名字（避免词法/语法两套枚举的同步负担）
    AstPtr  lhs, rhs;       // 左右子树（shared_ptr 拼出递归形状）
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
    // 【设计简化】赋值目标是"名字"不是"表达式" —— 不支持 a[0]=、f().x=
    //   这类复杂左值（语法层就限制了，见 parser.cpp 的 dynamic_cast 检查）
    AstPtr      value;
    Assign(std::string n, AstPtr v, int ln)
        : AstNode(ln), name(std::move(n)), value(std::move(v)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class Call : public AstNode {
public:
    std::string         callee;    // 函数名（简单语言：函数不是值，按名调用）
    std::vector<AstPtr> args;      // 实参表达式列表
    Call(std::string c, std::vector<AstPtr> a, int ln)
        : AstNode(ln), callee(std::move(c)), args(std::move(a)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

// === 语句节点 ===

class ExprStmt : public AstNode {        // 单纯的表达式语句：foo(); x = 1;
public:
    // 【为什么表达式要包成语句】语句有";"分隔与"产生副作用不产生值"的
    //   语义 —— 表达式求值的残留值由 Codegen 主动 POP（见 codegen.cpp）
    AstPtr expr;
    ExprStmt(AstPtr e, int ln) : AstNode(ln), expr(std::move(e)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class LetDecl : public AstNode {
public:
    std::string name;
    AstPtr      init;      // 右值表达式（let 必带初值 —— 不支持"声明后赋值"）
    LetDecl(std::string n, AstPtr i, int ln)
        : AstNode(ln), name(std::move(n)), init(std::move(i)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class IfStmt : public AstNode {
public:
    AstPtr cond, thenBranch, elseBranch;     // elseBranch 可为 nullptr
    // 【nullptr 表"没有 else"】可选子句用空指针表达 —— 访问者里
    //   if (n.elseBranch) 判空（见 type_checker/codegen 的同一写法）
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
    // 语句列表（而非嵌套链表）：vector 顺序即书写顺序 —— 词法作用域的
    // "块"边界就是 BlockStmt 的进出（TypeChecker/Codegen 都据此开/关作用域）
    BlockStmt(std::vector<AstPtr> s, int ln) : AstNode(ln), stmts(std::move(s)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class FuncDecl : public AstNode {
public:
    std::string              name;
    std::vector<std::string> params;      // 形参只是名字列表（无类型标注——动态风格）
    std::vector<AstPtr>      body;        // 函数体语句列表
    // 【body 为何不是 BlockStmt】设计选择：函数体直接装语句 ——
    //   BlockStmt 在 Codegen 里意味着"开新作用域"，函数作用域由
    //   FuncDecl 的帧（CodegenFrame）自己管理，避免双重作用域
    FuncDecl(std::string n, std::vector<std::string> p, std::vector<AstPtr> b, int ln)
        : AstNode(ln), name(std::move(n)),
          params(std::move(p)), body(std::move(b)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class ReturnStmt : public AstNode {
public:
    AstPtr value;        // 可为 nullptr（裸 return;）
    ReturnStmt(AstPtr v, int ln) : AstNode(ln), value(std::move(v)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

class PrintStmt : public AstNode {
public:
    AstPtr expr;
    // print 是语句而非内置函数 —— 语言级输出原语（Codegen 翻译为 PRINT 指令）
    PrintStmt(AstPtr e, int ln) : AstNode(ln), expr(std::move(e)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

// 顶层程序：一组语句和函数声明
class Program : public AstNode {
public:
    std::vector<AstPtr> stmts;
    // 根节点：顶层语句与函数声明混排（访问者按需分拣，见 codegen.cpp
    //   Program 的"两遍遍历"与 type_checker.cpp 的同名结构）
    Program(std::vector<AstPtr> s, int ln) : AstNode(ln), stmts(std::move(s)) {}
    Type acceptType(AstVisitor<Type>& v) override;
    void acceptCode(AstVisitor<void>& v) override;
};

}  // namespace Mycc

// === accept 实现（需要 visitor.h 的完整定义）===
// 【文件尾部 include 的原因】accept 的函数体要调 v.visit(*this)——
//   必须看到 AstVisitor 的完整定义。放在文件尾部使 17 个类先声明完、
//   再统一给出这 34 行"一行式"实现 —— 头文件自洽（包含者无需手动顺序）。
#include "visitor.h"

namespace Mycc {

// 17 组 accept：模式完全一致 —— 虚函数分派进节点，节点再转发给
//   visitor 的对应 visit 重载（双分派闭环）。这里逐类写全而不宏批量
//   生成，保持"阅读时一跳直达"的教学清晰度。
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

}  // namespace Mycc
