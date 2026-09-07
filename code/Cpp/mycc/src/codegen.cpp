#include "mycc/codegen.h"
#include <stdexcept>

namespace mycc {

Codegen::Codegen() {
    frames_.emplace_back("<top>");   // 主程序栈帧
}

Chunk Codegen::compile(Program& prog) {
    visit(prog);
    chk().emit(OpCode::HALT, 0);
    return std::move(frames_.front().chunk);
}

void Codegen::emitConst(Constant c, int line) {
    uint16_t idx = chk().addConstant(c);
    chk().emitWithU16(OpCode::CONST, idx, line);
}

// ============== 字面量 ==============
void Codegen::visit(NumLit& n)    { emitConst(n.value, n.line); }
void Codegen::visit(StringLit& n) { emitConst(n.value, n.line); }
void Codegen::visit(BoolLit& n)   { chk().emit(n.value ? OpCode::TRUE : OpCode::FALSE, n.line); }

// ============== 变量引用 ==============
void Codegen::visit(VarRef& n) {
    int slot = resolveLocal(n.name);
    if (slot >= 0) {
        chk().emitWithU16(OpCode::LOAD_LOCAL, static_cast<uint16_t>(slot), n.line);
    } else {
        uint16_t idx = chk().addConstant(n.name);
        chk().emitWithU16(OpCode::LOAD_GLOBAL, idx, n.line);
    }
}

// ============== 二元运算 ==============
void Codegen::visit(BinOp& n) {
    // —— 短路求值：&& 和 || 必须特判 ——
    if (n.op == TokKind::AndAnd) {
        n.lhs->acceptCode(*this);
        size_t end = chk().emitJump(OpCode::JUMP_IF_FALSE, n.line);
        chk().emit(OpCode::POP, n.line);
        n.rhs->acceptCode(*this);
        chk().patchJump(end);
        return;
    }
    if (n.op == TokKind::OrOr) {
        n.lhs->acceptCode(*this);
        size_t end = chk().emitJump(OpCode::JUMP_IF_TRUE, n.line);
        chk().emit(OpCode::POP, n.line);
        n.rhs->acceptCode(*this);
        chk().patchJump(end);
        return;
    }

    // —— 普通二元运算 ——
    n.lhs->acceptCode(*this);
    n.rhs->acceptCode(*this);
    switch (n.op) {
        case TokKind::Plus:    chk().emit(OpCode::ADD, n.line); break;
        case TokKind::Minus:   chk().emit(OpCode::SUB, n.line); break;
        case TokKind::Star:    chk().emit(OpCode::MUL, n.line); break;
        case TokKind::Slash:   chk().emit(OpCode::DIV, n.line); break;
        case TokKind::Percent: chk().emit(OpCode::MOD, n.line); break;
        case TokKind::EqEq:    chk().emit(OpCode::EQ, n.line); break;
        case TokKind::BangEq:  chk().emit(OpCode::NEQ, n.line); break;
        case TokKind::Lt:      chk().emit(OpCode::LT, n.line); break;
        case TokKind::Le:      chk().emit(OpCode::LE, n.line); break;
        case TokKind::Gt:      chk().emit(OpCode::GT, n.line); break;
        case TokKind::Ge:      chk().emit(OpCode::GE, n.line); break;
        default: throw std::runtime_error("codegen: bad binary op");
    }
}

// ============== 一元 ==============
void Codegen::visit(UnaryOp& n) {
    n.operand->acceptCode(*this);
    switch (n.op) {
        case TokKind::Minus: chk().emit(OpCode::NEG, n.line); break;
        case TokKind::Bang:  chk().emit(OpCode::NOT, n.line); break;
        default: throw std::runtime_error("codegen: bad unary op");
    }
}

// ============== 赋值 ==============
void Codegen::visit(Assign& n) {
    n.value->acceptCode(*this);   // 求出右值放栈顶

    // 赋值是表达式，结果 = 右值，所以要 DUP 留一份
    int slot = resolveLocal(n.name);
    if (slot >= 0) {
        chk().emit(OpCode::DUP, n.line);
        chk().emitWithU16(OpCode::STORE_LOCAL, static_cast<uint16_t>(slot), n.line);
    } else {
        chk().emit(OpCode::DUP, n.line);
        uint16_t idx = chk().addConstant(n.name);
        chk().emitWithU16(OpCode::STORE_GLOBAL, idx, n.line);
    }
}

// ============== 函数调用 ==============
void Codegen::visit(Call& n) {
    // 把函数名当成全局变量加载（VM 会从 functions 表查）
    uint16_t idx = chk().addConstant(n.callee);
    chk().emitWithU16(OpCode::LOAD_GLOBAL, idx, n.line);

    for (auto& arg : n.args) {
        arg->acceptCode(*this);
    }
    if (n.args.size() > 255) {
        throw std::runtime_error("too many arguments");
    }
    chk().emitWithU16(OpCode::CALL, static_cast<uint16_t>(n.args.size()), n.line);
}

// ============== 表达式语句 ==============
void Codegen::visit(ExprStmt& n) {
    n.expr->acceptCode(*this);
    chk().emit(OpCode::POP, n.line);   // 表达式语句的值要弃掉
}

// ============== let 声明 ==============
void Codegen::visit(LetDecl& n) {
    n.init->acceptCode(*this);
    if (cur().scopeDepth == 0) {
        // 全局
        uint16_t idx = chk().addConstant(n.name);
        chk().emitWithU16(OpCode::STORE_GLOBAL, idx, n.line);
        chk().emit(OpCode::POP, n.line);
    } else {
        // 局部：值已经在栈上，注册一个 Local 即可
        declareLocal(n.name, n.line);
    }
}

// ============== if 语句（跳转回填经典案例）==============
void Codegen::visit(IfStmt& n) {
    n.cond->acceptCode(*this);
    size_t elseJump = chk().emitJump(OpCode::POP_JUMP_IF_FALSE, n.line);

    n.thenBranch->acceptCode(*this);

    if (n.elseBranch) {
        size_t endJump = chk().emitJump(OpCode::JUMP, n.line);
        chk().patchJump(elseJump);            // ① else 入口
        n.elseBranch->acceptCode(*this);
        chk().patchJump(endJump);             // ② 整个 if 结束
    } else {
        chk().patchJump(elseJump);
    }
}

// ============== while 语句 ==============
void Codegen::visit(WhileStmt& n) {
    size_t loopStart = chk().code.size();    // 回跳目标（条件之前）
    n.cond->acceptCode(*this);
    size_t exitJump = chk().emitJump(OpCode::POP_JUMP_IF_FALSE, n.line);

    n.body->acceptCode(*this);
    chk().emitLoop(loopStart, n.line);       // 回跳

    chk().patchJump(exitJump);               // 假分支落点
}

// ============== 块 ==============
void Codegen::visit(BlockStmt& n) {
    beginScope();
    for (auto& s : n.stmts) s->acceptCode(*this);
    endScope();
}

void Codegen::endScope() {
    cur().scopeDepth--;
    while (!cur().locals.empty() &&
           cur().locals.back().depth > cur().scopeDepth) {
        chk().emit(OpCode::POP, 0);          // 块结束，弹掉本块所有局部
        cur().locals.pop_back();
    }
}

// ============== 函数声明 ==============
void Codegen::visit(FuncDecl& n) {
    // 切到新栈帧编译函数体
    frames_.emplace_back(n.name);
    beginScope();
    for (auto& p : n.params) {
        declareLocal(p, n.line);
    }
    for (auto& s : n.body) s->acceptCode(*this);
    // 兜底：函数末尾若没显式 return，自动 push nil + RETURN
    chk().emit(OpCode::NIL, n.line);
    chk().emit(OpCode::RETURN, n.line);

    Chunk c = std::move(frames_.back().chunk);
    frames_.pop_back();
    functions[n.name] = std::move(c);
}

// ============== return ==============
void Codegen::visit(ReturnStmt& n) {
    if (n.value) n.value->acceptCode(*this);
    else         chk().emit(OpCode::NIL, n.line);
    chk().emit(OpCode::RETURN, n.line);
}

// ============== print ==============
void Codegen::visit(PrintStmt& n) {
    n.expr->acceptCode(*this);
    chk().emit(OpCode::PRINT, n.line);
}

// ============== Program ==============
void Codegen::visit(Program& n) {
    // 第一遍：先把所有函数编译完（支持相互递归、顺序无关）
    for (auto& s : n.stmts) {
        if (auto* fd = dynamic_cast<FuncDecl*>(s.get())) {
            fd->acceptCode(*this);
        }
    }
    // 第二遍：编译顶层非函数语句到主 chunk
    for (auto& s : n.stmts) {
        if (!dynamic_cast<FuncDecl*>(s.get())) {
            s->acceptCode(*this);
        }
    }
}

// ============== 辅助：作用域 / 局部变量 ==============
int Codegen::resolveLocal(const std::string& name) {
    auto& locals = cur().locals;
    for (int i = static_cast<int>(locals.size()) - 1; i >= 0; --i) {
        if (locals[i].name == name) return i;
    }
    return -1;
}

void Codegen::declareLocal(const std::string& name, int /*line*/) {
    if (cur().locals.size() >= 65535) {
        throw std::runtime_error("too many locals");
    }
    cur().locals.push_back({ name, cur().scopeDepth });
}

}  // namespace mycc
