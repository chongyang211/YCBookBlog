// ============================================================================
// 【文件定位】codegen.cpp —— 代码生成访问者实现（AST → 字节码）
// ----------------------------------------------------------------------------
// 【实现总则】每个表达式编译后"恰好在栈顶留一个值"（栈平衡不变量）；
//   每个语句编译后"栈恢复原状"（值要么被 POP、要么被存走）。
//   if/while/&&/|| 靠 emitJump/patchJump 编排跳转 —— 教程§8.4 的
//   "没有回填会崩"造 BUG 现场就是这些代码的存在理由。
// ============================================================================
#include "mycc/codegen.h"
#include <stdexcept>

namespace Mycc {

// 构造：预置主程序帧 "<top>"（一切编译从这帧开始）
Codegen::Codegen() {
    frames_.emplace_back("<top>");   // 主程序栈帧
}

// 入口：遍历整树生成主 chunk，末尾 HALT，移交所有权
Chunk Codegen::compile(Program& prog) {
    visit(prog);                       // 双分派进 visit(Program&)（两遍编译都在那）
    chk().emit(OpCode::HALT, 0);       // 主 chunk 终点指令（VM 见 HALT 即停）
    return std::move(frames_.front().chunk);
    // move 交出 —— compile 后 Codegen 对象的主帧已被掏空（一次性使用）
}

// 字面量辅助：常量入池 + 发 CONST 下标
void Codegen::emitConst(Constant c, int line) {
    uint16_t idx = chk().addConstant(c);
    chk().emitWithU16(OpCode::CONST, idx, line);
}

// ============== 字面量 ==============
// 【栈效果】每条都在栈顶留下"表达式的值"——所有表达式编译的共同不变量
void Codegen::visit(NumLit& n)    { emitConst(n.value, n.line); }      // push 数字
void Codegen::visit(StringLit& n) { emitConst(n.value, n.line); }      // push 字符串
void Codegen::visit(BoolLit& n)   { chk().emit(n.value ? OpCode::TRUE : OpCode::FALSE, n.line); }
// 布尔走专用指令（TRUE/FALSE）—— 不占常量池（opcode.h 的取舍）

// ============== 变量引用 ==============
// 名字 → 局部（栈槽直达）或全局（经常量池按名查哈希表）
void Codegen::visit(VarRef& n) {
    int slot = resolveLocal(n.name);
    // 【编译期解析】"这个名字是局部还是全局"在编译时一锤定音 ——
    //   运行期照单执行，不做任何查找回退（编译器优化意识的最小样本）
    if (slot >= 0) {
        chk().emitWithU16(OpCode::LOAD_LOCAL, static_cast<uint16_t>(slot), n.line);
        // 局部快路径：2 字节槽号 → 运行期 stack_[slotBase+slot] 直取
    } else {
        uint16_t idx = chk().addConstant(n.name);
        chk().emitWithU16(OpCode::LOAD_GLOBAL, idx, n.line);
        // 全局：名字入常量池，运行期查 globals_（或函数表 —— VM 的
        //   LOAD_GLOBAL 先查函数再查全局，见 vm.cpp）
    }
}

// ============== 二元运算 ==============
// 顺序：先编译左、再编译右（栈上依次压入 a、b）→ 运算指令弹二推一
void Codegen::visit(BinOp& n) {
    // —— 短路求值：&& 和 || 必须特判 ——
    // 【为什么不能生成 AND/OR 指令】短路语义 = "左边定了就不算右边" ——
    //   普通编译会先算完两边，短路必须"边算边决定"，只能用跳转编排：
    if (n.op == TokKind::AndAnd) {
        n.lhs->acceptCode(*this);                              // 栈顶：左值
        size_t end = chk().emitJump(OpCode::JUMP_IF_FALSE, n.line);
        // 左值为假 → 直接跳到末尾（右值不必算）—— 假即结果
        chk().emit(OpCode::POP, n.line);                        // 跳前先弹掉左值
        // 【不弹版 JUMP_IF_FALSE 的配套】跳走时左值就是结果，留在栈上；
        //   不跳（继续算右值）时必须先弹左值给右值腾位 —— 栈平衡的分寸
        n.rhs->acceptCode(*this);                               // 栈顶：右值（真路径的结果）
        chk().patchJump(end);                                   // 回填：假路径落到这（栈上是左值）
        return;
        // 【栈平衡验证】两条路径各留恰好一个值：假路径留左值、真路径留右值
    }
    if (n.op == TokKind::OrOr) {
        n.lhs->acceptCode(*this);
        size_t end = chk().emitJump(OpCode::JUMP_IF_TRUE, n.line);
        // 与 && 镜像：左值为真 → 短路出结果
        chk().emit(OpCode::POP, n.line);
        n.rhs->acceptCode(*this);
        chk().patchJump(end);
        return;
    }

    // —— 普通二元运算 ——
    n.lhs->acceptCode(*this);     // push a
    n.rhs->acceptCode(*this);     // push b（栈序：a 在下 b 在上）
    switch (n.op) {
        // 运算符 → 指令的一对一翻译（TokKind 到 OpCode 的映射表即编译"后端"）
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
        // &&/|| 不会到这（前面 return 了）
        default: throw std::runtime_error("codegen: bad binary op");
    }
}

// ============== 一元 ==============
// 先编译操作数（栈顶），再发"弹出→推回"的单目指令
void Codegen::visit(UnaryOp& n) {
    n.operand->acceptCode(*this);
    switch (n.op) {
        case TokKind::Minus: chk().emit(OpCode::NEG, n.line); break;
        case TokKind::Bang:  chk().emit(OpCode::NOT, n.line); break;
        default: throw std::runtime_error("codegen: bad unary op");
    }
}

// ============== 赋值 ==============
// 赋值是表达式：右值"一份存变量、一份留栈"—— DUP 的用武之地
void Codegen::visit(Assign& n) {
    n.value->acceptCode(*this);   // 求出右值放栈顶

    // 赋值是表达式，结果 = 右值，所以要 DUP 留一份
    int slot = resolveLocal(n.name);
    if (slot >= 0) {
        chk().emit(OpCode::DUP, n.line);          // 复制一份（栈上两份右值）
        chk().emitWithU16(OpCode::STORE_LOCAL, static_cast<uint16_t>(slot), n.line);
        // STORE_LOCAL 是"peek 存"不弹 —— 存掉下面那份，栈顶那份是表达式的值
    } else {
        chk().emit(OpCode::DUP, n.line);
        uint16_t idx = chk().addConstant(n.name);
        chk().emitWithU16(OpCode::STORE_GLOBAL, idx, n.line);
        // STORE_GLOBAL 也是 peek 存（vm.cpp 里 globals_[name]=peek()）——
        //   DUP+peek 的组合让"存了一份、留一份"一次到位
    }
}

// ============== 函数调用 ==============
void Codegen::visit(Call& n) {
    // 把函数名当成全局变量加载（VM 会从 functions 表查）
    uint16_t idx = chk().addConstant(n.callee);
    chk().emitWithU16(OpCode::LOAD_GLOBAL, idx, n.line);
    // 函数值入栈（size_t 索引）—— CALL 指令的栈布局要求"函数值在实参底下"
    //（LOAD_GLOBAL 在 VM 里先查函数表 —— 函数名被解析为索引，见 vm.cpp）

    for (auto& arg : n.args) {
        arg->acceptCode(*this);      // 实参依次入栈（求值顺序 = 书写顺序）
    }
    if (n.args.size() > 255) {
        throw std::runtime_error("too many arguments");
        // CALL 的 argc 是 uint16 但用 255 上限 —— 单字节够用且给栈布局留余量
    }
    chk().emitWithU16(OpCode::CALL, static_cast<uint16_t>(n.args.size()), n.line);
    // CALL n：VM 弹出调用帧 —— 栈上 [fn, arg0..argn] 整体变成新帧的初始格局
}

// ============== 表达式语句 ==============
void Codegen::visit(ExprStmt& n) {
    n.expr->acceptCode(*this);
    chk().emit(OpCode::POP, n.line);   // 表达式语句的值要弃掉
    // "表达式语句"的语义：只求副作用不求值 —— 值残留在栈上会破坏平衡，
    //   一条 POP 干净利落（x = 1; 的 DUP 副本在这里被弹掉）
}

// ============== let 声明 ==============
void Codegen::visit(LetDecl& n) {
    n.init->acceptCode(*this);    // 右值先入栈（编译顺序 = 执行顺序）
    if (cur().scopeDepth == 0) {
        // 全局
        uint16_t idx = chk().addConstant(n.name);
        chk().emitWithU16(OpCode::STORE_GLOBAL, idx, n.line);
        chk().emit(OpCode::POP, n.line);
        // 全局 let：STORE_GLOBAL 是 peek 存 → 值还在栈上 → 主动 POP
        //（声明是语句不留值，与赋值表达式（留值）的对比即"编译器必须懂语义"）
    } else {
        // 局部：值已经在栈上，注册一个 Local 即可
        declareLocal(n.name, n.line);
        // 【零指令声明】局部变量声明不发任何指令 —— 值本来就压在栈顶，
        //   declareLocal 只是"给这个栈位置起名字"（槽号 = locals.size()）：
        //   编译器"聪明"的极致样本，也是栈式 VM 局部变量快的原因
    }
}

// ============== if 语句（跳转回填经典案例）==============
// 【目标字节码形态】（无 else）：
//     cond...  POP_JUMP_IF_FALSE  <else 之后>   // 假 → 跳过 then
//     then...
//   <else 之后>:                                // 回填点
// 【含 else】多一个 JUMP（then 末尾跳过 else 块）+ 第二个回填点
void Codegen::visit(IfStmt& n) {
    n.cond->acceptCode(*this);                                // 条件值入栈
    size_t elseJump = chk().emitJump(OpCode::POP_JUMP_IF_FALSE, n.line);
    // 条件是"消费型"：用弹版假跳（栈平衡）—— 占位下标存 elseJump

    n.thenBranch->acceptCode(*this);                          // then 分支体

    if (n.elseBranch) {
        size_t endJump = chk().emitJump(OpCode::JUMP, n.line);
        // then 走完必须跳过 else —— 此时 else 还没编译、长度未知 → 再占一位
        chk().patchJump(elseJump);            // ① else 入口
        // 条件为假的落点 = 此刻位置（then 编完、else 未编）—— 第一个回填
        n.elseBranch->acceptCode(*this);                      // else 分支体
        chk().patchJump(endJump);             // ② 整个 if 结束
        // then 出口的落点 = 此刻位置（else 也编完）—— 第二个回填
    } else {
        chk().patchJump(elseJump);
        // 无 else：假路径直接落到 if 之后（一个回填点就够）
    }
}

// ============== while 语句 ==============
// 【目标字节码形态】
//   <loopStart>: cond...  POP_JUMP_IF_FALSE <exit>   // 假 → 出循环
//               body...
//               LOOP <loopStart>                     // 无条件回跳
//   <exit>:                                          // 回填点
void Codegen::visit(WhileStmt& n) {
    size_t loopStart = chk().code.size();    // 回跳目标（条件之前）
    // 先记位置再编条件 —— LOOP 的相对偏移要以此为基准倒算
    n.cond->acceptCode(*this);
    size_t exitJump = chk().emitJump(OpCode::POP_JUMP_IF_FALSE, n.line);
    // 假 → 出循环（占位，循环体长度未知）

    n.body->acceptCode(*this);
    chk().emitLoop(loopStart, n.line);       // 回跳
    // 回跳目标已知 → emitLoop 当场算偏移（与前跳的"回填"形成对照）

    chk().patchJump(exitJump);               // 假分支落点
    // 循环体编完才知出口位置 —— 回填收尾
}

// ============== 块 ==============
void Codegen::visit(BlockStmt& n) {
    beginScope();      // 深度 +1：本块内声明的局部都带此深度
    for (auto& s : n.stmts) s->acceptCode(*this);
    endScope();        // 深度 -1 + 弹出本块局部 + 发 POP 清栈
}

// 出作用域：弹出所有"深度大于当前"的局部，每弹一个发一条 POP
void Codegen::endScope() {
    cur().scopeDepth--;
    while (!cur().locals.empty() &&
           cur().locals.back().depth > cur().scopeDepth) {
        // 【从尾部弹的理由】locals 按声明序追加 → 尾部 = 最内层后声明的 ——
        //   "深度 > 当前"的连续尾部即"本块的所有局部"（栈式后进先出）
        chk().emit(OpCode::POP, 0);          // 块结束，弹掉本块所有局部
        // 【栈平衡的编译期责任】块内 let 压在栈上的值，块退出必须清掉 ——
        //   编译器数着"块里声明了几个局部"就发几条 POP
        cur().locals.pop_back();
    }
}

// ============== 函数声明 ==============
// 切帧编译：新帧（新 chunk/新 locals 表）→ 形参入局 → 编函数体 → 收帧归档
void Codegen::visit(FuncDecl& n) {
    // 切到新栈帧编译函数体
    frames_.emplace_back(n.name);
    // 【编译期"调用"】push 新帧 = 语义上的"进入函数"—— 后续所有
    //   chk() 都指向新 chunk，主程序的编码现场被完整保存（帧栈的意义）
    beginScope();
    for (auto& p : n.params) {
        declareLocal(p, n.line);
        // 【形参 = 栈槽 0..n-1】VM 调用时把实参就地排进新帧栈底 ——
        //   编译期把形参名注册为"槽 0/1/2..."，与运行期布局严丝合缝
    }
    for (auto& s : n.body) s->acceptCode(*this);
    // 兜底：函数末尾若没显式 return，自动 push nil + RETURN
    chk().emit(OpCode::NIL, n.line);
    chk().emit(OpCode::RETURN, n.line);
    // 【为什么兜底】函数可能中途 return、也可能自然落尾 —— 落尾没有
    //   RETURN 的话 VM 会把帧外的字节码当函数体继续执行（灾难）。
    //   "统一出口"保证任何路径离开函数都恰有一条 RETURN
    //（显式 return 时这段兜底永不执行 —— ip 早被 RETURN 弹走了，无害）

    Chunk c = std::move(frames_.back().chunk);
    frames_.pop_back();
    // 收帧：chunk 搬走、栈帧弹出 —— 编码现场切回主帧
    functions[n.name] = std::move(c);
    // 归档：函数名 → 字节码（VM 的函数表即这份映射）
}

// ============== return ==============
void Codegen::visit(ReturnStmt& n) {
    if (n.value) n.value->acceptCode(*this);   // 有返回值：栈顶留值
    else         chk().emit(OpCode::NIL, n.line);   // 裸 return：补一个 nil
    chk().emit(OpCode::RETURN, n.line);
    // RETURN 的契约：栈顶必有一个值作为返回值（return; 的 nil 兜底）
}

// ============== print ==============
void Codegen::visit(PrintStmt& n) {
    n.expr->acceptCode(*this);
    chk().emit(OpCode::PRINT, n.line);
    // PRINT 弹值输出 —— 语句级栈平衡由"弹掉"天然保证
}

// ============== Program ==============
void Codegen::visit(Program& n) {
    // 第一遍：先把所有函数编译完（支持相互递归、顺序无关）
    for (auto& s : n.stmts) {
        if (auto* fd = dynamic_cast<FuncDecl*>(s.get())) {
            // dynamic_cast 原始指针版（s.get() 是 AstNode*）：按类型挑函数声明
            fd->acceptCode(*this);
            // 代价：每个 FuncDecl 会进"第一遍 + 第二遍跳过"——两遍各查一次类型，
            // 换来"函数先全部就位，主代码里的 CALL 都能链接到"（与
            // TypeChecker 的 Program 两遍遍历完全同构 —— 编译器多 pass 的呼应）
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
// 从当前帧反向查名字：命中返回槽号，-1 = 不是局部（→ 全局）
int Codegen::resolveLocal(const std::string& name) {
    auto& locals = cur().locals;
    for (int i = static_cast<int>(locals.size()) - 1; i >= 0; --i) {
        // 反向遍历 = 从最内层找起 —— 同名遮蔽时最近的生效
        if (locals[i].name == name) return i;
        // 【i 的双重身份】vector 下标 == 运行期栈槽号 —— 因为
        //   "声明序 = 压栈序"（局部声明时值已在栈顶、槽号递增分配）
    }
    return -1;   // 本帧没有 → 调用方走全局路径
}

void Codegen::declareLocal(const std::string& name, int /*line*/) {
    if (cur().locals.size() >= 65535) {
        throw std::runtime_error("too many locals");
        // LOAD_LOCAL 操作数 2 字节 → 65535 槽上限（编码格式的物理边界）
    }
    cur().locals.push_back({ name, cur().scopeDepth });
    // 入表三要素：名字 + 当前深度 —— 槽号由 push_back 的位置天然决定
}

}  // namespace Mycc
