// ============================================================================
// 【文件定位】vm.cpp —— 栈式虚拟机实现（字节码 → 执行结果）
// ----------------------------------------------------------------------------
// 【主循环的三段式】fetch（readByte 取指令）→ decode（switch 分派）→
//   execute（各 case 操作栈/ip）—— 与真实 CPU 的取指-译码-执行同构，
//   教程阶段⑦的收官现场：examples/fib.mycc 的每个数字都是这里算出来的。
// ============================================================================
#include "mycc/vm.h"
#include "mycc/error.h"

#include <cmath>       // std::fmod（MOD 的浮点取模）
#include <iostream>
#include <stdexcept>
#include <variant>

namespace Mycc {

// 装载：收编编译产物，建立"索引化函数表"加速 CALL
void VM::load(Chunk topChunk, std::unordered_map<std::string, Chunk> fns) {
    top_ = std::move(topChunk);
    fns_ = std::move(fns);
    fnTable_.clear();
    fnIndex_.clear();
    for (auto& [name, c] : fns_) {
        // 【C++17 结构化绑定】解包 map 的 pair<const string, Chunk>
        fnIndex_[name] = fnTable_.size();
        fnTable_.push_back(&c);
        // 双表同步建：名字→索引（LOAD_GLOBAL 查）、索引→chunk 指针（CALL 查）
        //（fns_ 是成员且不再增删 → 指针长期有效，const Chunk* 只读共享）
    }
}

// ---- 三个解码原语：都作用于"当前帧"（frames_.back()）----
uint8_t VM::readByte() {
    auto& f = frames_.back();
    return f.chunk->code[f.ip++];   // 取一字节并推进程序计数器
    // 【fetch 即推进】"取指令"与"ip++"是同一动作 —— 无预取的教学简化
}
uint16_t VM::readU16() {
    uint16_t lo = readByte();
    uint16_t hi = readByte();
    return lo | (hi << 8);
    // 小端解码 —— 与 chunk.cpp 的 emitWithU16 编码严格互逆
    //（两次 readByte 保证跨帧字节流上的 ip 推进正确）
}
Constant VM::readConstant() {
    return frames_.back().chunk->constants[readU16()];
    // 先读 2 字节下标再查当前帧的常量池 —— "每帧独立常量池"的解码侧体现
}

// 运行时错误：消息 + 全调用栈回溯（"in fib / in <top>"的栈轨迹）
[[noreturn]] void VM::runtimeError(const std::string& msg, int line) {
    std::string trace;
    for (auto it = frames_.rbegin(); it != frames_.rend(); ++it) {
        // 反向遍历：最近调用的帧在最上（"栈顶先说"的回溯惯例）
        trace += "\n  in " + it->name;
    }
    throw RuntimeError(msg + trace, line);
    // 带行号抛给 main —— REPL 接住打印，文件模式以退出码 4 结束
}

// 算术：弹 b、弹 a（顺序即语义）→ 双 double 校验 → 按指令计算
void VM::binaryArith(OpCode op, int line) {
    Value b = pop(), a = pop();
    // 【先弹是右操作数】栈是 LIFO：后压的 b（右）在顶 —— a-b 的被减数先弹 a
    if (!std::holds_alternative<double>(a) || !std::holds_alternative<double>(b)) {
        runtimeError("operands must be numbers", line);
        // 运行期类型防御：TypeChecker 应已拦住 —— 这里兜底"逃逸类型"
        //（错误消息远好于 variant 取值崩溃）
    }
    double x = std::get<double>(a), y = std::get<double>(b);
    switch (op) {
        case OpCode::ADD: push(x + y); break;
        case OpCode::SUB: push(x - y); break;
        case OpCode::MUL: push(x * y); break;
        case OpCode::DIV:
            if (y == 0.0) runtimeError("divide by zero", line);
            // 【运行时错误的样本】除零在编译期不可判定（y 是运行值）——
            //   examples/divzero.mycc 的演示目标
            push(x / y); break;
        case OpCode::MOD:
            if (y == 0.0) runtimeError("mod by zero", line);
            push(std::fmod(x, y)); break;   // 浮点取模（C 的 % 不支持 double）
        default: break;
    }
}

// 比较：EQ/NEQ 全类型可比（variant==），大小比较仅限数字
void VM::binaryCmp(OpCode op, int line) {
    Value b = pop(), a = pop();
    if (op == OpCode::EQ)  { push(a == b); return; }
    // variant 的 ==：逐备选比较（Nil 有手写 ==、函数按索引比）——
    //   "1 == 1.0"成立（同为 double 备选）；"1" == 1 不成立（类型不同）
    if (op == OpCode::NEQ) { push(!(a == b)); return; }
    if (!std::holds_alternative<double>(a) || !std::holds_alternative<double>(b)) {
        runtimeError("comparison operands must be numbers", line);
    }
    double x = std::get<double>(a), y = std::get<double>(b);
    switch (op) {
        case OpCode::LT: push(x <  y); break;
        case OpCode::LE: push(x <= y); break;
        case OpCode::GT: push(x >  y); break;
        case OpCode::GE: push(x >= y); break;
        default: break;
    }
}

// ============================================================
//  主循环——fetch / decode / execute
// ============================================================
void VM::run() {
    frames_.clear();
    frames_.push_back({ &top_, 0, 0, "<top>" });
    // 重置运行期状态：初始帧 = 主 chunk、ip=0、slotBase=0 —— 每帧四字段的
    //   聚合初始化顺序与 CallFrame 声明序严格一致
    stack_.clear();
    //（同一 VM 可重复 run：stateless 起点保证幂等）

    while (true) {
        CallFrame& fr = frames_.back();
        // 帧引用每轮重取：CALL/RETURN 会改栈顶帧 —— 缓存引用会失效（悬垂）
        int line = fr.chunk->lines[fr.ip];   // 当前指令行号（出错时用）
        // 取指令前先查行号表 —— 与 code[fr.ip] 同下标（chunk.h 的设计回报）
        OpCode op = static_cast<OpCode>(readByte());
        // fetch + decode：字节转枚举，switch 分派

        switch (op) {
            // -------- 字面量 --------
            case OpCode::CONST: {
                Constant c = readConstant();
                std::visit([&](auto&& v) { push(Value{v}); }, c);
                // 常量池元素（double/string/bool）逐型转入 Value ——
                // variant→variant 的 visit 桥接（类型集合不同，逐成员构造）
                break;
            }
            case OpCode::NIL:   push(Nil{});   break;
            case OpCode::TRUE:  push(true);    break;
            case OpCode::FALSE: push(false);   break;

            // -------- 算术 / 比较 --------
            case OpCode::ADD: case OpCode::SUB: case OpCode::MUL:
            case OpCode::DIV: case OpCode::MOD:
                binaryArith(op, line); break;
            // 弹二推一（栈深 -1）—— 二元指令的统一栈效果

            case OpCode::EQ: case OpCode::NEQ: case OpCode::LT:
            case OpCode::LE: case OpCode::GT: case OpCode::GE:
                binaryCmp(op, line); break;

            case OpCode::NEG: {
                Value a = pop();
                if (!std::holds_alternative<double>(a))
                    runtimeError("unary '-' needs number", line);
                push(-std::get<double>(a));
                break;
            }
            case OpCode::NOT: {
                Value a = pop();
                push(!isTruthy(a));
                // NOT 走真值判定（!5 → !truthy(5) → false）—— 逻辑非的语言语义
                break;
            }

            // -------- 变量 --------
            case OpCode::LOAD_GLOBAL: {
                Constant c = readConstant();
                const auto& name = std::get<std::string>(c);
                auto it = fnIndex_.find(name);
                if (it != fnIndex_.end()) { push(it->second); break; }
                // 【函数优先查找】LOAD_GLOBAL 的"一名两用"：先查函数表 ——
                //   命中则 push 函数索引（Call 的编译前提）；函数也是
                //   "全局符号"（教学语言的简化：函数与全局变量共享命名空间）
                auto it2 = globals_.find(name);
                if (it2 == globals_.end())
                    runtimeError("undefined variable: " + name, line);
                push(it2->second);
                break;
            }
            case OpCode::STORE_GLOBAL: {
                Constant c = readConstant();
                globals_[std::get<std::string>(c)] = peek();
                // peek 不弹：右值留在栈上（赋值是表达式，值即结果）
                break;
            }
            case OpCode::LOAD_LOCAL: {
                uint16_t slot = readU16();
                push(stack_[fr.slotBase + slot]);
                // 绝对定位：帧基址 + 编译期槽号 —— O(1) 直取（局部快的兑现）
                break;
            }
            case OpCode::STORE_LOCAL: {
                uint16_t slot = readU16();
                stack_[fr.slotBase + slot] = peek();   // 不弹（赋值是表达式）
                break;
            }

            // -------- 控制流 --------
            // 【跳转族共同点】操作数都是"2 字节相对偏移"，改写 fr.ip：
            case OpCode::JUMP: {
                uint16_t off = readU16();
                fr.ip += off;           // 无条件前跳（if-else 的"跳过 else"）
                break;
            }
            case OpCode::JUMP_IF_FALSE: {
                uint16_t off = readU16();
                if (!isTruthy(peek())) fr.ip += off;
                // 不弹版：&&/|| 短路用 —— 假左值留作结果
                break;
            }
            case OpCode::JUMP_IF_TRUE: {
                uint16_t off = readU16();
                if (isTruthy(peek())) fr.ip += off;
                // || 短路专用（真左值即结果）
                break;
            }
            case OpCode::POP_JUMP_IF_FALSE: {
                uint16_t off = readU16();
                Value v = pop();
                if (!isTruthy(v)) fr.ip += off;
                // 弹版：if/while 条件消费型 —— 判断完即扔（栈平衡）
                break;
            }
            case OpCode::LOOP: {
                uint16_t off = readU16();
                fr.ip -= off;           // 回跳（while 尾部 → 循环条件处）
                break;
            }

            // -------- 函数调用 --------
            case OpCode::CALL: {
                uint16_t argc = readU16();
                // 栈布局：[..., fnIndex, arg0, arg1, ...] ←TOP
                Value& callee = stack_[stack_.size() - argc - 1];
                // 【栈上定位被调函数】实参底下第 argc+1 个 —— Codegen 先
                //   LOAD_GLOBAL（函数值）再压实参的布局在此兑现
                if (!std::holds_alternative<std::size_t>(callee))
                    runtimeError("can only call functions", line);
                // 按值不可调（let x = 1; x(); → 1 不是函数索引）
                size_t fnIdx = std::get<std::size_t>(callee);
                if (fnIdx >= fnTable_.size())
                    runtimeError("invalid function index", line);
                // 索引越界防御（fnTable_ 数组访问前的守门）

                if (frames_.size() >= 64)
                    runtimeError("stack overflow (recursion too deep)", line);
                // 【帧深上限】用户递归在"堆上的 frames_"里跑（C++ 栈安全）——
                //   但帧也是内存，无上限递归会吃光 RAM —— 64 帧的教学阈值
                //   （生产版应高几个数量级+可配）

                CallFrame nf;
                nf.chunk    = fnTable_[fnIdx];
                nf.ip       = 0;
                nf.slotBase = stack_.size() - argc;     // 实参就地变成 locals
                // 【传参零拷贝的魔法】不搬实参 —— 只记"从哪开始是我的栈底"：
                //   arg0..argn 原地变成新帧的 slot 0..n-1（Codegen 把形参
                //   注册为槽 0..n-1，两端约定严丝合缝）
                nf.name     = nf.chunk->name;
                frames_.push_back(nf);
                // push 后下一轮循环的 frames_.back() 即新帧 —— 主循环继续
                // 在新 chunk 上 fetch（"切流"只换帧指针，代码零变化）
                break;
            }

            case OpCode::RETURN: {
                Value rv = pop();
                // 栈顶 = 返回值（Codegen 的 RETURN 契约：必有值，裸 return 配 NIL）
                CallFrame done = frames_.back();
                frames_.pop_back();
                // 弹掉被调函数的所有 locals + callee 本身
                stack_.resize(done.slotBase - 1);
                // 【一剪子截栈】resize 到 slotBase-1：
                //   slotBase 是 arg0 的位置，-1 连"函数值"一起剪掉 ——
                //   [fn, args..., locals...] 整段作废（比逐个 pop 快）
                //（若此时帧已弹光、下方是 <top> —— 循环自然回到主 chunk）
                push(std::move(rv));
                // 返回值落在"调用点正上方的栈顶" —— 调用表达式的值就位
                break;
            }

            // -------- I/O / 控制 --------
            case OpCode::PRINT: {
                std::cout << pop() << "\n";
                // operator<<(Value) 在 value.h —— variant 的 visit 分派打印
                break;
            }
            case OpCode::POP:   pop(); break;
            case OpCode::DUP:   push(peek()); break;   // 复制栈顶（赋值的两份值）
            case OpCode::HALT:  return;
            // 主 chunk 终点：函数返回 main —— run 结束
            //（递归帧在时不会遇到 HALT：它只在 <top> 末尾由 Codegen 生成）

            default:
                runtimeError("unknown opcode", line);
                // 未知字节防御（损坏的字节码流 —— 教学版正常路径不可达）
        }
    }
}

}  // namespace Mycc
