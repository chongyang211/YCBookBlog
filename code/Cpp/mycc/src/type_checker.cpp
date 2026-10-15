// ============================================================================
// 【文件定位】type_checker.cpp —— 语义检查访问者实现（第一遍遍历：查错+贴标签）
// ----------------------------------------------------------------------------
// 【实现套路】每个 visit：先递归检查子节点（acceptType 回调进来）→
//   按规则推导自身类型 → n.ty = 结果 返回；任一子检查失败抛 TypeError，
//   异常沿调用栈直接冲出（不必层层 if —— 编译错误"一点就炸"是好体验）。
// ============================================================================
#include "mycc/type_checker.h"
#include "mycc/error.h"

namespace Mycc {

// 入口：包一层"全局作用域"的开合（check 后栈必须回到空 —— RAII 式对称）
void TypeChecker::check(AstPtr program) {
    enterScope();           // 全局作用域
    program->acceptType(*this);   // 双分派进 visit(Program&)（见 ast.h 的 accept 注释）
    leaveScope();
}

// 定义变量：写进"当前层"（栈顶）；同名已定义返回 false
bool TypeChecker::define(const std::string& name, Type t) {
    auto& cur = scopes.back();
    if (cur.count(name)) return false;
    // 只查当前层：内层同名是"遮蔽"（合法），不叫重复定义 ——
    //   "重定义"专指同一作用域里出现两次
    cur[name] = t;
    return true;
}

// 查找：从最内层往外 —— 词法作用域 + 遮蔽语义的核心
Type TypeChecker::lookup(const std::string& name, int line) {
    // 从内向外查（最近作用域优先）
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        // rbegin：从栈顶（最内层）开始 —— "最近的定义生效"
        if (auto f = it->find(name); f != it->end()) return f->second;
        // 命中即返回：外层同名被遮蔽（shadowing），内层优先
    }
    throw TypeError("undefined variable '" + name + "'", line, file_);
    // 全栈扫完没有 —— 未定义引用，编译期最经典的错误（带名字+行号）
}

// 变量引用：类型 = 定义时的类型（符号表查出来的）
Type TypeChecker::visit(VarRef& n) {
    return n.ty = lookup(n.name, n.line);
    // 贴标签 + 上交一体的惯用句式（此处标签就是查得结果）
}

// 二元运算：左右子树先查 → 按运算符类别做"操作数类型 → 结果类型"推导
Type TypeChecker::visit(BinOp& n) {
    Type lt = n.lhs->acceptType(*this);
    Type rt = n.rhs->acceptType(*this);
    // 后序遍历：先子后己 —— 子树的类型没确定前父节点无从推导

    auto fail = [&](const std::string& msg) {
        // 局部 lambda 统一"带行号带文件抛 TypeError"的样板（闭包捕 n/file_）
        throw TypeError(msg, n.line, file_);
    };

    switch (n.op) {
        case TokKind::Plus: case TokKind::Minus:
        case TokKind::Star: case TokKind::Slash: case TokKind::Percent:
            if (lt != Type::Num || rt != Type::Num) fail("arithmetic requires Num operands");
            // 算术只吃数字：print 1 + "abc" 在这里被拦截（REPL 的 tcheck 命令演示点）
            return n.ty = Type::Num;    // 数字 op 数字 → 数字

        case TokKind::EqEq: case TokKind::BangEq:
            if (lt != rt) fail("== / != requires operands of the same type");
            // 相等比较允许任何类型、但两边必须同型（数字==字符串无意义）
            return n.ty = Type::Bool;   // 比较结果恒为布尔

        case TokKind::Lt: case TokKind::Le: case TokKind::Gt: case TokKind::Ge:
            if (lt != Type::Num || rt != Type::Num) fail("comparison requires Num operands");
            return n.ty = Type::Bool;

        case TokKind::AndAnd: case TokKind::OrOr:
            if (lt != Type::Bool || rt != Type::Bool) fail("logical op requires Bool operands");
            // 【严格布尔】逻辑运算两侧必须是 Bool（不像 JS 那样"万物皆真"）——
            //   1 && true 直接报错，类型系统在编译期挡住隐式转换的坑
            return n.ty = Type::Bool;

        default: fail("unknown binary operator");
    }
    return Type::Unknown;
    // switch 全覆盖后的形式收尾（fail 是抛异常路径，此行实际不可达）
}

// 一元运算：-要数字、!要布尔
Type TypeChecker::visit(UnaryOp& n) {
    Type t = n.operand->acceptType(*this);
    if (n.op == TokKind::Minus) {
        if (t != Type::Num) throw TypeError("unary '-' requires Num", n.line, file_);
        return n.ty = Type::Num;      // -Num → Num
    }
    if (n.op == TokKind::Bang) {
        if (t != Type::Bool) throw TypeError("'!' requires Bool", n.line, file_);
        return n.ty = Type::Bool;     // !Bool → Bool
    }
    return Type::Unknown;
}

// 赋值：先求右值类型，再查左值符号 —— 两边必须同型（静态语言纪律）
Type TypeChecker::visit(Assign& n) {
    Type rhs = n.value->acceptType(*this);
    Type lhs = lookup(n.name, n.line);
    // 注意顺序：右值表达式先检查（它可能引用别的变量），
    //   再查左值名是否存在 —— 两个错误都可能触发，先后只影响报哪个
    if (lhs != rhs) throw TypeError("assignment type mismatch", n.line, file_);
    // 【设计决策】赋值不改变量类型 —— let a = 1; a = "x"; 被拒绝 ——
    //   变量的类型在定义时钉死（简单可靠的静态类型语义）
    return n.ty = rhs;
    // 赋值是表达式：值 = 右值（a = b = 1 链的类型由此贯通）
}

// 函数调用：查签名表 → 校验参数个数 → 逐个检查实参
Type TypeChecker::visit(Call& n) {
    auto it = functions.find(n.callee);
    if (it == functions.end()) {
        throw TypeError("undefined function '" + n.callee + "'", n.line, file_);
        // 调了没声明的函数（签名表在 Program/FuncDecl 两处都会填充）
    }
    if ((int)n.args.size() != it->second.paramCount) {
        throw TypeError("function " + n.callee + " expects " +
                        std::to_string(it->second.paramCount) + " args but got " +
                        std::to_string(n.args.size()), n.line, file_);
        // 参数个数硬校验（教学语言不查参数类型 —— FnSig 只存 count）
    }
    for (auto& a : n.args) a->acceptType(*this);
    // 实参逐个过一遍（触发其中的未定义变量/类型错误检查）
    // 简化：所有函数返回 Num（真实编译器要做返回类型推导）
    return n.ty = Type::Num;
    // 【教学简化】返回类型一律 Num —— print fib(3) + 1 能过，print fib(3) + "x" 报错
}

// let 声明：查右值 → 在当前层定义（重复定义报错）
Type TypeChecker::visit(LetDecl& n) {
    Type t = n.init->acceptType(*this);
    if (!define(n.name, t)) {
        throw TypeError("variable " + n.name + " already defined", n.line, file_);
        // define 返回 false = 当前层同名已存在 —— 重复定义拦截
    }
    return Type::Void;
    // 声明是语句：无值（Void）—— let x = 1; 作为表达式用会在赋值/运算处报类型错
}

// 块：进新作用域 → 逐句检查 → 出作用域（词法作用域的进出对）
Type TypeChecker::visit(BlockStmt& n) {
    enterScope();
    for (auto& s : n.stmts) s->acceptType(*this);
    leaveScope();
    // 出块即弃层：块内定义的变量从此不可见 —— 符号表栈 pop 自动完成
    return Type::Void;
}

// if：条件必须是 Bool，分支各自检查
Type TypeChecker::visit(IfStmt& n) {
    if (n.cond->acceptType(*this) != Type::Bool) {
        throw TypeError("if condition requires Bool", n.line, file_);
        // if (1) 不合法 —— 数字不做隐式真值（与 C 的历史宽松对比鲜明）
    }
    n.thenBranch->acceptType(*this);
    if (n.elseBranch) n.elseBranch->acceptType(*this);
    // else 可空（nullptr）—— 可选子句的标准判空
    return Type::Void;
}

// while：与 if 同构（条件 Bool + 体检查）
Type TypeChecker::visit(WhileStmt& n) {
    if (n.cond->acceptType(*this) != Type::Bool) {
        throw TypeError("while condition requires Bool", n.line, file_);
    }
    n.body->acceptType(*this);
    return Type::Void;
}

// 函数声明：先注册签名 → 开新作用域 → 参数当变量 → 检查函数体
Type TypeChecker::visit(FuncDecl& n) {
    // 1. 先把函数签名注册到全局——支持递归调用
    functions[n.name] = FnSig{(int)n.params.size(), n.line};
    // 【"先注册后查体"】fib 的函数体里有 fib(n-1) —— 进体之前签名已就位，
    //   自递归天然通过（否则体内查不到自己）

    // 2. 进入新作用域，把参数当作变量（简化：参数都按 Num 处理）
    enterScope();
    for (auto& p : n.params) define(p, Type::Num);
    // 参数即局部变量：同名于参数的变量 = 形参（与 Codegen 的
    // declareLocal 完全对位 —— 检查器和代码生成器共享同一套心智模型）
    for (auto& s : n.body) s->acceptType(*this);
    leaveScope();
    // 出函数作用域：体内定义对全局不可见（函数体是独立词法环境）
    return Type::Void;
}

// return：只检查返回值表达式存在时的合法性（不校验与函数返回类型匹配——教学简化）
Type TypeChecker::visit(ReturnStmt& n) {
    if (n.value) n.value->acceptType(*this);
    return Type::Void;
}

// Program：两遍遍历 —— 签名收集 + 逐句检查
Type TypeChecker::visit(Program& n) {
    // 第一遍：先注册所有顶层函数（让函数能彼此调用、不依赖声明顺序）
    for (auto& d : n.stmts) {
        if (auto fn = std::dynamic_pointer_cast<FuncDecl>(d)) {
            // dynamic_pointer_cast 按类型过滤：只挑函数声明，跳过普通语句
            functions[fn->name] = FnSig{(int)fn->params.size(), fn->line};
        }
    }
    // 【两遍的意义】fn a() { b(); } fn b() { return 1; } ——
    //   a 在文本上先于 b，单遍时 a 体内查不到 b 的签名。
    //   第一遍"只见森林不见树叶"地登记所有签名，第二遍才深入体检查 ——
    //   声明顺序依赖被两遍结构消除（真实编译器多 pass 的最小化教学版）
    // 第二遍：真正访问所有声明
    for (auto& d : n.stmts) d->acceptType(*this);
    return Type::Void;
}

}  // namespace Mycc
