#pragma once

#include <cstdint>   // uint8_t：指令的物理宽度

namespace Mycc {

// ========================================
//  字节码指令集（共 36 条）
//  栈式 VM——所有运算都在求值栈上完成
// ========================================
// 【设计思想·为什么选字节码（README 取舍）】对比"直接在 AST 上树遍历求值"：
//   ① 性能约 3 倍（AST 遍历每次都要走虚函数分发，字节码是 switch+内存数组）；
//   ② 跳转回填是汇编器/链接器/JIT 的通用模式 —— 学它就是学系统编程；
//   ③ "栈式"而非"寄存器式"：指令不必指定操作数位置（都在栈顶）——
//     编译器实现极简，且与 AST 的"表达式→栈顶"语义天然对齐。
// enum 底层类型 uint8_t：指令在 code 流里就是一个字节（物理布局即语义）
enum class OpCode : uint8_t {
    // -- 字面量与常量池 --
    CONST,          // CONST i        → push constants[i]
    // 【为什么有常量池】数字/字符串直接塞指令流会变长变慢 ——
    //   集中存放、指令只带 2 字节索引（同 Chunk::addConstant 去重）
    NIL,            //                  → push nil
    TRUE,           //                  → push true
    FALSE,          //                  → push false
    // 布尔/nil 高频且无数据 → 专用指令免去常量池寻址（指令集设计的取舍示例）

    // -- 算术 --
    ADD,            // pop b, pop a    → push a+b
    SUB,            // 【栈序注意】先弹的是右操作数 ——
                    //   a - b 必须按"先 b 后 a"弹出（vm.cpp 的 pop 顺序即语义）
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
    // 比较产布尔 —— 与 TypeChecker 的 BinOp→Bool 推导一一对应

    // -- 逻辑 --
    NOT,            // pop a           → push !a
    AND,            // 短路靠 JUMP_IF_FALSE 实现
    OR,
    // 【AND/OR 指令与短路的分工】Codegen 对 && || 完全不生成 AND/OR 指令 ——
    //   短路求值用 JUMP_IF_FALSE/JUMP_IF_TRUE + POP 编排（见 codegen.cpp）。
    //   这两条指令保留在指令集里但实际不用（教学对照："逻辑运算的两种实现"）

    // -- 变量 --
    LOAD_GLOBAL,    // LOAD_GLOBAL i   → push globals[constants[i]]
    // 全局变量按"名字"经常量池查找（哈希表 O(1)，跨帧共享）
    STORE_GLOBAL,   // STORE_GLOBAL i  → globals[constants[i]] = pop
    LOAD_LOCAL,     // LOAD_LOCAL  i   → push locals[i]
    // 局部变量按"栈偏移"直达：i 是 frame.locals 里的下标 ——
    //   编译期解析（Codegen::resolveLocal），运行期零查找（快路径）
    STORE_LOCAL,    // STORE_LOCAL i   → locals[i] = peek（不弹栈）
    // 【peek 不 pop】赋值是表达式：右值既要存给变量又要留在栈上作为表达式的值 ——
    //   所以"偷看不拿走"（配合 Codegen 的 DUP 也有同款考虑）

    // -- 控制流（带 2 字节相对偏移）--
    JUMP,           // JUMP        off → ip += off
    // 【相对跳转 vs 绝对地址】相对偏移让字节码与位置无关（整体搬移不影响执行）；
    //   2 字节 = 64KB 跳程上限（chunk.cpp 有对应防越界检查）
    JUMP_IF_FALSE,  // JUMP_IF_FALSE off → if (peek == false) ip += off
    JUMP_IF_TRUE,   // 用于 ||
    POP_JUMP_IF_FALSE,  // 同时弹栈
    // 【为什么两种"假跳"】&& 短路要保留左值（它就是结果）→ 用不弹版 JUMP_IF_FALSE；
    //   if/while 的条件是"消费型"（跳完不再用）→ 用弹版 POP_JUMP_IF_FALSE ——
    //   一条指令之差，栈平衡的分寸就在这（教程§8 的核心训练点）
    LOOP,           // LOOP        off → ip -= off（回跳）
    // 【LOOP 与 JUMP 的符号差】前跳 +=，回跳 -= —— while 编译的专用通道
    //   （emitLoop 生成，跳向"循环条件"处）

    // -- 函数 --
    CALL,           // CALL n          → 调用栈顶函数，n = 实参个数
    // 【栈布局约定】[..., 函数值, arg0, arg1, ...] ← 调用前必须按此排列；
    //   函数值在 args 底下第 n+1 个位置（vm.cpp CALL 用 size()-argc-1 定位）
    RETURN,         // 弹出当前栈帧

    // -- I/O 与控制 --
    PRINT,          // pop a           → 输出
    POP,            //                  → 弃栈顶
    // 【POP 的四大用途】表达式语句丢弃值 / && 短路后弹左值 /
    //   endScope 弹块内局部 / 全局 let 存储后弹初始值 ——
    //   栈平衡的"万能守门员"
    DUP,            //                  → 复制栈顶
    // 【DUP 的用途】赋值表达式：一份存变量、一份留作表达式的值（codegen.cpp）
    HALT,           // 停机
    // 主 chunk 的终点指令：VM 见 HALT 即返回（函数 chunk 用 RETURN 收尾）
};

// 反汇编辅助：把 OpCode 转成字符串
const char* opcodeName(OpCode op);
// dump 命令的可读化：教学调试链路的一环

}  // namespace Mycc
