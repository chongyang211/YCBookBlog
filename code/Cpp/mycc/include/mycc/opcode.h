#pragma once

#include <cstdint>

namespace mycc {

// ========================================
//  字节码指令集（共 36 条）
//  栈式 VM——所有运算都在求值栈上完成
// ========================================
enum class OpCode : uint8_t {
    // -- 字面量与常量池 --
    CONST,          // CONST i        → push constants[i]
    NIL,            //                  → push nil
    TRUE,           //                  → push true
    FALSE,          //                  → push false

    // -- 算术 --
    ADD,            // pop b, pop a    → push a+b
    SUB,
    MUL,
    DIV,
    MOD,
    NEG,            // pop a           → push -a

    // -- 比较 --
    EQ,             // pop b, pop a    → push (a==b)
    NEQ,
    LT,
    LE,
    GT,
    GE,

    // -- 逻辑 --
    NOT,            // pop a           → push !a
    AND,            // 短路靠 JUMP_IF_FALSE 实现
    OR,

    // -- 变量 --
    LOAD_GLOBAL,    // LOAD_GLOBAL i   → push globals[constants[i]]
    STORE_GLOBAL,   // STORE_GLOBAL i  → globals[constants[i]] = pop
    LOAD_LOCAL,     // LOAD_LOCAL  i   → push locals[i]
    STORE_LOCAL,    // STORE_LOCAL i   → locals[i] = peek（不弹栈）

    // -- 控制流（带 2 字节相对偏移） --
    JUMP,           // JUMP        off → ip += off
    JUMP_IF_FALSE,  // JUMP_IF_FALSE off → if (peek == false) ip += off
    JUMP_IF_TRUE,   // 用于 ||
    POP_JUMP_IF_FALSE,  // 同时弹栈
    LOOP,           // LOOP        off → ip -= off（回跳）

    // -- 函数 --
    CALL,           // CALL n          → 调用栈顶函数，n = 实参个数
    RETURN,         // 弹出当前栈帧

    // -- I/O 与控制 --
    PRINT,          // pop a           → 输出
    POP,            //                  → 弃栈顶
    DUP,            //                  → 复制栈顶
    HALT,           // 停机
};

// 反汇编辅助：把 OpCode 转成字符串
const char* opcodeName(OpCode op);

}  // namespace mycc
