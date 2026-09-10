/* 36 条字节码指令（§07）：栈式 VM，大部分 1 字节，带操作数的 1+2 字节（小端）*/
#ifndef MYCC_OPCODE_H
#define MYCC_OPCODE_H

typedef enum {
    // -- 字面量 --
    OP_CONST,           // CONST i   → push constants[i]
    OP_NIL,             //            → push nil
    OP_TRUE,            //            → push true
    OP_FALSE,           //            → push false

    // -- 算术 --
    OP_ADD,             // pop b, pop a → push a+b
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_NEG,             // pop a       → push -a

    // -- 比较 --
    OP_EQ,              // pop b, pop a → push (a==b)
    OP_NEQ,
    OP_LT,
    OP_LE,
    OP_GT,
    OP_GE,

    // -- 逻辑（短路由 JUMP_IF_* 实现，这两条保留给反汇编完备性）--
    OP_NOT,             // pop a       → push !a
    OP_AND,
    OP_OR,

    // -- 变量 --
    OP_LOAD_GLOBAL,     // i = 名字常量索引（优先函数表 → v_fn）
    OP_STORE_GLOBAL,
    OP_LOAD_LOCAL,      // i = 栈帧偏移
    OP_STORE_LOCAL,

    // -- 控制流（带 2 字节相对偏移）--
    OP_JUMP,
    OP_JUMP_IF_FALSE,   // 不弹栈——用于 &&
    OP_JUMP_IF_TRUE,    // 不弹栈——用于 ||
    OP_POP_JUMP_IF_FALSE,   // 弹栈——用于 if/while
    OP_LOOP,            // 负偏移回跳

    // -- 函数 --
    OP_CALL,            // CALL n  → 调用栈顶函数，n=实参个数（1 字节操作数）
    OP_RETURN,

    // -- I/O 与控制 --
    OP_PRINT,
    OP_POP,
    OP_DUP,
    OP_HALT,
} OpCode;

const char *opcode_name(OpCode op);

#endif
