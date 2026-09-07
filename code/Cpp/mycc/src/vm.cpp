#include "mycc/vm.h"
#include "mycc/error.h"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <variant>

namespace mycc {

void VM::load(Chunk topChunk, std::unordered_map<std::string, Chunk> fns) {
    top_ = std::move(topChunk);
    fns_ = std::move(fns);
    fnTable_.clear();
    fnIndex_.clear();
    for (auto& [name, c] : fns_) {
        fnIndex_[name] = fnTable_.size();
        fnTable_.push_back(&c);
    }
}

uint8_t VM::readByte() {
    auto& f = frames_.back();
    return f.chunk->code[f.ip++];
}
uint16_t VM::readU16() {
    uint16_t lo = readByte();
    uint16_t hi = readByte();
    return lo | (hi << 8);
}
Constant VM::readConstant() {
    return frames_.back().chunk->constants[readU16()];
}

[[noreturn]] void VM::runtimeError(const std::string& msg, int line) {
    std::string trace;
    for (auto it = frames_.rbegin(); it != frames_.rend(); ++it) {
        trace += "\n  in " + it->name;
    }
    throw RuntimeError(msg + trace, line);
}

void VM::binaryArith(OpCode op, int line) {
    Value b = pop(), a = pop();
    if (!std::holds_alternative<double>(a) || !std::holds_alternative<double>(b)) {
        runtimeError("operands must be numbers", line);
    }
    double x = std::get<double>(a), y = std::get<double>(b);
    switch (op) {
        case OpCode::ADD: push(x + y); break;
        case OpCode::SUB: push(x - y); break;
        case OpCode::MUL: push(x * y); break;
        case OpCode::DIV:
            if (y == 0.0) runtimeError("divide by zero", line);
            push(x / y); break;
        case OpCode::MOD:
            if (y == 0.0) runtimeError("mod by zero", line);
            push(std::fmod(x, y)); break;
        default: break;
    }
}

void VM::binaryCmp(OpCode op, int line) {
    Value b = pop(), a = pop();
    if (op == OpCode::EQ)  { push(a == b); return; }
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
    stack_.clear();

    while (true) {
        CallFrame& fr = frames_.back();
        int line = fr.chunk->lines[fr.ip];   // 当前指令行号（出错时用）
        OpCode op = static_cast<OpCode>(readByte());

        switch (op) {
            // -------- 字面量 --------
            case OpCode::CONST: {
                Constant c = readConstant();
                std::visit([&](auto&& v) { push(Value{v}); }, c);
                break;
            }
            case OpCode::NIL:   push(Nil{});   break;
            case OpCode::TRUE:  push(true);    break;
            case OpCode::FALSE: push(false);   break;

            // -------- 算术 / 比较 --------
            case OpCode::ADD: case OpCode::SUB: case OpCode::MUL:
            case OpCode::DIV: case OpCode::MOD:
                binaryArith(op, line); break;

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
                break;
            }

            // -------- 变量 --------
            case OpCode::LOAD_GLOBAL: {
                Constant c = readConstant();
                const auto& name = std::get<std::string>(c);
                auto it = fnIndex_.find(name);
                if (it != fnIndex_.end()) { push(it->second); break; }
                auto it2 = globals_.find(name);
                if (it2 == globals_.end())
                    runtimeError("undefined variable: " + name, line);
                push(it2->second);
                break;
            }
            case OpCode::STORE_GLOBAL: {
                Constant c = readConstant();
                globals_[std::get<std::string>(c)] = peek();
                break;
            }
            case OpCode::LOAD_LOCAL: {
                uint16_t slot = readU16();
                push(stack_[fr.slotBase + slot]);
                break;
            }
            case OpCode::STORE_LOCAL: {
                uint16_t slot = readU16();
                stack_[fr.slotBase + slot] = peek();   // 不弹（赋值是表达式）
                break;
            }

            // -------- 控制流 --------
            case OpCode::JUMP: {
                uint16_t off = readU16();
                fr.ip += off;
                break;
            }
            case OpCode::JUMP_IF_FALSE: {
                uint16_t off = readU16();
                if (!isTruthy(peek())) fr.ip += off;
                break;
            }
            case OpCode::JUMP_IF_TRUE: {
                uint16_t off = readU16();
                if (isTruthy(peek())) fr.ip += off;
                break;
            }
            case OpCode::POP_JUMP_IF_FALSE: {
                uint16_t off = readU16();
                Value v = pop();
                if (!isTruthy(v)) fr.ip += off;
                break;
            }
            case OpCode::LOOP: {
                uint16_t off = readU16();
                fr.ip -= off;
                break;
            }

            // -------- 函数调用 --------
            case OpCode::CALL: {
                uint16_t argc = readU16();
                // 栈布局：[..., fnIndex, arg0, arg1, ...] ←TOP
                Value& callee = stack_[stack_.size() - argc - 1];
                if (!std::holds_alternative<std::size_t>(callee))
                    runtimeError("can only call functions", line);
                size_t fnIdx = std::get<std::size_t>(callee);
                if (fnIdx >= fnTable_.size())
                    runtimeError("invalid function index", line);

                if (frames_.size() >= 64)
                    runtimeError("stack overflow (recursion too deep)", line);

                CallFrame nf;
                nf.chunk    = fnTable_[fnIdx];
                nf.ip       = 0;
                nf.slotBase = stack_.size() - argc;     // 实参就地变成 locals
                nf.name     = nf.chunk->name;
                frames_.push_back(nf);
                break;
            }

            case OpCode::RETURN: {
                Value rv = pop();
                CallFrame done = frames_.back();
                frames_.pop_back();
                // 弹掉被调函数的所有 locals + callee 本身
                stack_.resize(done.slotBase - 1);
                push(std::move(rv));
                break;
            }

            // -------- I/O / 控制 --------
            case OpCode::PRINT: {
                std::cout << pop() << "\n";
                break;
            }
            case OpCode::POP:   pop(); break;
            case OpCode::DUP:   push(peek()); break;
            case OpCode::HALT:  return;

            default:
                runtimeError("unknown opcode", line);
        }
    }
}

}  // namespace mycc
