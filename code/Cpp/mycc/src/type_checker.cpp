#include "mycc/type_checker.h"
#include "mycc/error.h"

namespace Mycc {

void TypeChecker::check(AstPtr program) {
    enterScope();           // 全局作用域
    program->acceptType(*this);
    leaveScope();
}

bool TypeChecker::define(const std::string& name, Type t) {
    auto& cur = scopes.back();
    if (cur.count(name)) return false;
    cur[name] = t;
    return true;
}

Type TypeChecker::lookup(const std::string& name, int line) {
    // 从内向外查（最近作用域优先）
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (auto f = it->find(name); f != it->end()) return f->second;
    }
    throw TypeError("undefined variable '" + name + "'", line, file_);
}

Type TypeChecker::visit(VarRef& n) {
    return n.ty = lookup(n.name, n.line);
}

Type TypeChecker::visit(BinOp& n) {
    Type lt = n.lhs->acceptType(*this);
    Type rt = n.rhs->acceptType(*this);

    auto fail = [&](const std::string& msg) {
        throw TypeError(msg, n.line, file_);
    };

    switch (n.op) {
        case TokKind::Plus: case TokKind::Minus:
        case TokKind::Star: case TokKind::Slash: case TokKind::Percent:
            if (lt != Type::Num || rt != Type::Num) fail("arithmetic requires Num operands");
            return n.ty = Type::Num;

        case TokKind::EqEq: case TokKind::BangEq:
            if (lt != rt) fail("== / != requires operands of the same type");
            return n.ty = Type::Bool;

        case TokKind::Lt: case TokKind::Le: case TokKind::Gt: case TokKind::Ge:
            if (lt != Type::Num || rt != Type::Num) fail("comparison requires Num operands");
            return n.ty = Type::Bool;

        case TokKind::AndAnd: case TokKind::OrOr:
            if (lt != Type::Bool || rt != Type::Bool) fail("logical op requires Bool operands");
            return n.ty = Type::Bool;

        default: fail("unknown binary operator");
    }
    return Type::Unknown;
}

Type TypeChecker::visit(UnaryOp& n) {
    Type t = n.operand->acceptType(*this);
    if (n.op == TokKind::Minus) {
        if (t != Type::Num) throw TypeError("unary '-' requires Num", n.line, file_);
        return n.ty = Type::Num;
    }
    if (n.op == TokKind::Bang) {
        if (t != Type::Bool) throw TypeError("'!' requires Bool", n.line, file_);
        return n.ty = Type::Bool;
    }
    return Type::Unknown;
}

Type TypeChecker::visit(Assign& n) {
    Type rhs = n.value->acceptType(*this);
    Type lhs = lookup(n.name, n.line);
    if (lhs != rhs) throw TypeError("assignment type mismatch", n.line, file_);
    return n.ty = rhs;
}

Type TypeChecker::visit(Call& n) {
    auto it = functions.find(n.callee);
    if (it == functions.end()) {
        throw TypeError("undefined function '" + n.callee + "'", n.line, file_);
    }
    if ((int)n.args.size() != it->second.paramCount) {
        throw TypeError("function " + n.callee + " expects " +
                        std::to_string(it->second.paramCount) + " args but got " +
                        std::to_string(n.args.size()), n.line, file_);
    }
    for (auto& a : n.args) a->acceptType(*this);
    // 简化：所有函数返回 Num（真实编译器要做返回类型推导）
    return n.ty = Type::Num;
}

Type TypeChecker::visit(LetDecl& n) {
    Type t = n.init->acceptType(*this);
    if (!define(n.name, t)) {
        throw TypeError("variable " + n.name + " already defined", n.line, file_);
    }
    return Type::Void;
}

Type TypeChecker::visit(BlockStmt& n) {
    enterScope();
    for (auto& s : n.stmts) s->acceptType(*this);
    leaveScope();
    return Type::Void;
}

Type TypeChecker::visit(IfStmt& n) {
    if (n.cond->acceptType(*this) != Type::Bool) {
        throw TypeError("if condition requires Bool", n.line, file_);
    }
    n.thenBranch->acceptType(*this);
    if (n.elseBranch) n.elseBranch->acceptType(*this);
    return Type::Void;
}

Type TypeChecker::visit(WhileStmt& n) {
    if (n.cond->acceptType(*this) != Type::Bool) {
        throw TypeError("while condition requires Bool", n.line, file_);
    }
    n.body->acceptType(*this);
    return Type::Void;
}

Type TypeChecker::visit(FuncDecl& n) {
    // 1. 先把函数签名注册到全局——支持递归调用
    functions[n.name] = FnSig{(int)n.params.size(), n.line};

    // 2. 进入新作用域，把参数当作变量（简化：参数都按 Num 处理）
    enterScope();
    for (auto& p : n.params) define(p, Type::Num);
    for (auto& s : n.body) s->acceptType(*this);
    leaveScope();
    return Type::Void;
}

Type TypeChecker::visit(ReturnStmt& n) {
    if (n.value) n.value->acceptType(*this);
    return Type::Void;
}

Type TypeChecker::visit(Program& n) {
    // 第一遍：先注册所有顶层函数（让函数能彼此调用、不依赖声明顺序）
    for (auto& d : n.stmts) {
        if (auto fn = std::dynamic_pointer_cast<FuncDecl>(d)) {
            functions[fn->name] = FnSig{(int)fn->params.size(), fn->line};
        }
    }
    // 第二遍：真正访问所有声明
    for (auto& d : n.stmts) d->acceptType(*this);
    return Type::Void;
}

}  // namespace Mycc
